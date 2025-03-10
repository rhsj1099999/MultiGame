﻿#include "CoreLib.h"

CircularQueue::CircularQueue(int Bytesize)
{
    _capacity = Bytesize;
    _data = new char[_capacity];
    memset(_data, 0xff, Bytesize);
    _front = _rear = -1;
}

CircularQueue::~CircularQueue()
{
    delete[] _data;
}

bool CircularQueue::isEmpty()
{
    
    return _front == -1;
}

bool CircularQueue::isFull()
{
    return _size == _capacity;
}

bool CircularQueue::isFull_Add(int size)
{
    return (_size + size) >= _capacity;
}

void CircularQueue::enqueue_Int(int item)
{
    
    if (isFull() || isFull_Add(sizeof(int)))
    {
        return;
    }
    else if (isEmpty())
    {
        _front = _rear = 0;
    }

    int spaceToEnd = _capacity - _rear - 1;
    if (_front > _rear && sizeof(int) > spaceToEnd)
    {
        memcpy(&_data + _rear + 1, &item, spaceToEnd);
        memcpy(&_data, &item + spaceToEnd, sizeof(int) - spaceToEnd);
    }
    else
    {
        memcpy(&_data[_rear], &item, sizeof(int));
    }

    _rear = (_rear + sizeof(int)) % _capacity;
    _size += sizeof(int);
}

void CircularQueue::Enqueqe_Ptr(void* Ptr, int memsize)
{
    bool bMessageReady = false;

    if (isFull() || isFull_Add(sizeof(memsize)))
    {
        return;
    }
    else if (isEmpty())
    {
        _front = _rear = 0;
        bMessageReady = true;
    }

    int spaceToEnd = _capacity - _rear - 1;
    if (_front > _rear && memsize > spaceToEnd)
    {
        memcpy(&_data + _rear + 1, Ptr, spaceToEnd);
        memcpy(&_data, (char*)Ptr + spaceToEnd, memsize - spaceToEnd);
    }
    else
    {
        memcpy(&_data[_rear], Ptr, memsize);
    }
    
    _rear = (_rear + memsize) % _capacity;
    _size += memsize;
}

int CircularQueue::dequeue_Int()
{
    
    if (isEmpty())
    {
        return -1;
    }
    else if (_front == _rear)
    {
        int item = _data[_front];
        _front = _rear = -1;
        return item;
    }
    else
    {
        int item = _data[_front];
        _front = (_front + 1) % _capacity;
        return item;
    }
}

void CircularQueue::Dequeqe_Size(int memsize)
{
    if (isEmpty())
    {
        return;
    }
    else if (_front == _rear)
    {
        _front = _rear = -1;
        return;
    }
    else
    {
        _front = (_front + memsize) % _capacity;
        return;
    }
}

void CircularQueue::display()
{
    if (isEmpty())
    {
        cout << "Queue is empty.\n";
        return;
    }
    else if (_front <= _rear)
    {
        cout << "Queue: ";
        for (int i = _front; i <= _rear; i++)
        {
            cout << _data[i] << " ";
        }
    }
    else
    {
        cout << "Queue: ";
        for (int i = _front; i < _capacity; i++)
        {
            cout << _data[i] << " ";
        }

        for (int i = 0; i <= _rear; i++)
        {
            cout << _data[i] << " ";
        }
    }
    cout << endl;
}



void* CircularQueue::GetFrontOffsetPtr(int Offset)
{
    
    int EndPoint = _front + Offset;

    if (_size - 1 < Offset)
    {
        return nullptr;
    }

    if (_front > _rear)
    {
        EndPoint %= _capacity;
        return &_data[EndPoint];
    }
    else
    {
        return &_data[EndPoint];
    }
}

void CircularQueue::Dequq_N(int size)
{
    if (isEmpty())
    {
        return;
    }
    if (_size <= 0 || size > _size)
    {
        return;
    }
    _front = (_front + size) % _capacity;
    _size -= size;

    if (_front == _rear)
    {
        _front = _rear = -1;
        memset(_data, 0xff, _capacity);
        return;
    }
}

char* CircularQueue::GetBuffer() { return _data; }

int CircularQueue::GetSize()
{
    return _size;
}


bool ReadStringFromFile(string* directory, string* ret)
{
    ifstream file((*directory).c_str()); //VC Try

    if (file.is_open() == false)
    {
        file.close();
        return false;
    }

    if (!getline(file, *ret))
    {
        file.close();
        return false;
    }

    file.close();
    return true;
}




bool WSAGetLastError_MessageReport(int ret)
{
    if (ret == 0)
    {
        return true;
    }

    int errorCode = WSAGetLastError();

    std::cerr << "WSAStartup failed with error: " << errorCode << std::endl;

    switch (errorCode) 
    {
    case WSASYSNOTREADY:
        std::cerr << "Network subsystem is not ready." << std::endl;
        break;
    case WSAVERNOTSUPPORTED:
        std::cerr << "Requested Windows Sockets version is not supported." << std::endl;
        break;
    case WSAEINPROGRESS:
        std::cerr << "A blocking operation is in progress." << std::endl;
        break;
    case WSAEPROCLIM:
        std::cerr << "Too many processes using WinSock." << std::endl;
        break;
    case WSAEFAULT:
        std::cerr << "Invalid WSADATA pointer." << std::endl;
        break;
    default:
        std::cerr << "Unknown error occurred." << std::endl;
    }

    return false;
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
                continue;
            }

            WSABUF DataBuf;
            DataBuf.buf = pSession->_buffer;
            DataBuf.len = BUFSIZE;

            DWORD recvLen = 0;
            DWORD flag = 0;

            switch (pSession->_ioType)
            {
                case ClientSession::IOType::Read:
                    WSARecv(pSession->_socket, &DataBuf, 1, &recvLen, &flag, (LPOVERLAPPED)&pOverlap, NULL);
                    break;

                case ClientSession::IOType::Write:
                    WSASend(pSession->_socket, &DataBuf, 1, &recvLen, flag, (LPOVERLAPPED)&pOverlap, nullptr);
                    break;

                default:
                    break;
            }
        }
    }
}
