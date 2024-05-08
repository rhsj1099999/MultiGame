#include "CoreLib.h"

CMyCQ::CMyCQ(int ByteSize)
{
    capacity = ByteSize;
    queue = new char[capacity];
    memset(queue, 0xff, ByteSize);
    front = rear = -1;
}

CMyCQ::~CMyCQ()
{
    delete[] queue;
}

bool CMyCQ::isEmpty()
{
    
    return front == -1;
}

bool CMyCQ::isFull()
{
    
    return Size == capacity;
}

bool CMyCQ::isFull_Add(int iSize)
{
    
    return (Size + iSize) >= capacity;
}

void CMyCQ::enqueue_Int(int item)
{
    
    if (isFull() || isFull_Add(sizeof(int)))
    {
        return;
    }
    else if (isEmpty())
    {
        front = rear = 0;
    }

    int spaceToEnd = capacity - rear - 1;
    if (front > rear && sizeof(int) > spaceToEnd)
    {
        memcpy(&queue + rear + 1, &item, spaceToEnd);
        memcpy(&queue, &item + spaceToEnd, sizeof(int) - spaceToEnd);
    }
    else
    {
        memcpy(&queue[rear], &item, sizeof(int));
    }



    rear = (rear + sizeof(int)) % capacity;
    Size += sizeof(int);
}

void CMyCQ::Enqueqe_Ptr(void* Ptr, int memSize)
{
    bool bMessageReady = false;

    if (isFull() || isFull_Add(sizeof(memSize)))
    {
        return;
    }
    else if (isEmpty())
    {
        front = rear = 0;
        bMessageReady = true;
    }

    int spaceToEnd = capacity - rear - 1;
    if (front > rear && memSize > spaceToEnd)
    {
        memcpy(&queue + rear + 1, Ptr, spaceToEnd);
        memcpy(&queue, (char*)Ptr + spaceToEnd, memSize - spaceToEnd);
    }
    else
    {
        memcpy(&queue[rear], Ptr, memSize);
    }
    
    rear = (rear + memSize) % capacity;
    Size += memSize;
}

int CMyCQ::dequeue_Int()
{
    
    if (isEmpty())
    {
        return -1;
    }
    else if (front == rear)
    {
        int item = queue[front];
        front = rear = -1;
        return item;
    }
    else
    {
        int item = queue[front];
        front = (front + 1) % capacity;
        return item;
    }
}

void CMyCQ::Dequeqe_Size(int memSize)
{
    
    if (isEmpty())
    {
        return;
    }
    else if (front == rear)
    {
        front = rear = -1;
        return;
    }
    else
    {
        front = (front + memSize) % capacity;
        return;
    }
}

void CMyCQ::display()
{
    
    if (isEmpty())
    {
        cout << "Queue is empty.\n";
        return;
    }
    else if (front <= rear)
    {
        cout << "Queue: ";
        for (int i = front; i <= rear; i++)
        {
            cout << queue[i] << " ";
        }
    }
    else
    {
        cout << "Queue: ";
        for (int i = front; i < capacity; i++)
        {
            cout << queue[i] << " ";
        }

        for (int i = 0; i <= rear; i++)
        {
            cout << queue[i] << " ";
        }
    }
    cout << endl;
}

void* CMyCQ::GetFrontPtr() {  return &queue[front]; }

void* CMyCQ::GetRearPtr() {  return &queue[rear]; }

void* CMyCQ::GetFrontOffsetPtr(int Offset)
{
    
    int EndPoint = front + Offset;

    if (Size - 1 < Offset)
    {
        return nullptr;
    }

    if (front > rear)
    {
        EndPoint %= capacity;
        return &queue[EndPoint];
    }
    else
    {
        return &queue[EndPoint];
    }
}

void CMyCQ::Dequq_N(int iSize)
{
    
    if (isEmpty())
    {
        return;
    }
    if (Size <= 0 || iSize > Size)
    {
        return;
    }
    front = (front + iSize) % capacity;
    Size -= iSize;

    if (front == rear)
    {
        front = rear = -1;
        memset(queue, 0xff, capacity);
        return;
    }
}

char* CMyCQ::GetBuffer() { return queue; }

int CMyCQ::GetSize()
{
    return Size;
}

bool MySend_Ptr(ClientSession* pSession, void* Ptr, int Size, PREDATA::OrderType Type)
{
    bool Return = true;

    if (pSession->soc == INVALID_SOCKET)
        return false;

    CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());

    bool bTempMessageSend = false;
    if (pSession->CQPtr->GetSize() == 0)
        bTempMessageSend = true;

    pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
    (
        Size,
        Type
    ));
    pSession->CQPtr->Enqueqe_Ptr(Ptr, Size);

    if (bTempMessageSend == true)
    {
        DWORD recvLen = 0;
        DWORD flag = 0;
        pSession->wsaBuf_Send.buf = (char*)pSession->CQPtr->GetFrontPtr();
        pSession->wsaBuf_Send.len = pSession->CQPtr->GetSize();

        if (WSASend((pSession)->soc, &pSession->wsaBuf_Send, 1, &recvLen, flag, &(pSession)->Overlapped_Send, NULL) == SOCKET_ERROR)
        {
            int ERR = WSAGetLastError();
            if (ERR != WSAEWOULDBLOCK && ERR != WSA_IO_PENDING)
            {
                Return = false;
                //MSGBOX("Error_MySend. Not EWB, PENDING");
            }
        }
    }

    return Return;
}

void WorkerEntry(HANDLE hHandle, WSABUF* pOut)
{
    while (true)
    {
        while (true)
        {
            DWORD Bytes = 0;
            ClientSession* pSession = nullptr;
            LPOVERLAPPED pOverlap = nullptr;

            bool bRet = GetQueuedCompletionStatus(hHandle, &Bytes, (ULONG_PTR*)&pSession, (LPOVERLAPPED*)&pOverlap, INFINITE);

            if (bRet == FALSE || Bytes == 0)
            {
                //Close Socket
                continue;
            }

            switch (pSession->eType)
            {
            case READ:
            {
                WSABUF DataBuf;
                DataBuf.buf = pSession->recvBuffer;
                DataBuf.len = BUFSIZE;

                DWORD recvLen = 0;
                DWORD flag = 0;
                WSARecv(pSession->soc, &DataBuf, 1, &recvLen, &flag, (LPOVERLAPPED)&pOverlap, NULL);
            }
            break;

            case WRITE:
            {
            }
            break;

            case QUEUEWATING:
            {
                WSABUF DataBuf;
                DataBuf.buf = pSession->recvBuffer;
                DataBuf.len = BUFSIZE;

                DWORD recvLen = 0;
                DWORD flag = 0;
                WSASend(pSession->soc, &DataBuf, 1, &recvLen, flag, (LPOVERLAPPED)&pOverlap, nullptr);
            }
            break;


            default:
                break;
            }
        }
    }
}
