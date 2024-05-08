#pragma once
#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <mutex>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

#define MSGBOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Fail_"), MB_OK)
#define MSGBOX2(MESSAGE) MessageBox(0, MESSAGE, TEXT("Fail_"), MB_OK)

using namespace std;

#define HOLE_HORIZON 8
#define HOLE_VERTICAL 3
#define SENDHEARTBEATCYCLE 2000
#define MAXCONNETTIME 10000

#define CLIENT3 3
//#define CLIENT3 1
#define CLIENT1 1
#define MAXCHATLEN 50
#define MAXCHATLEN_TOC 100
#define CMPCHAT 150
#define NULLSIZE 1

#define BUF64 64
#define BUF128 128
#define BUF256 256
#define BUF384 384
#define BUF512 512
#define BUF1024 1024

#define BUFSIZE 100
#define BUFSIZE_100 100
#define BUFSIZE_1000 1000
#define MAXCHATTINGSHOW 4000

#define MESSAGEYDIFF 30

#define MAXTIMEOUT 3000
#define MAXLATECOUNT 2

enum class MSGType
{
    Sys,
    User,
    END,
};

struct Session
{
    SOCKET soc = INVALID_SOCKET;
    char recvqueue[BUFSIZE_100] = {};
    __int32 recvSize = 0;
    __int32 sendSize = 0;
};

enum IO_TYPE
{
    READ,
    WRITE,
    ACCEPT,
    CONNECT,
    QUEUEWATING,
    END,
};

struct OverlappedExtend
{
    WSAOVERLAPPED OverlappedEvent = {};
    __int32 eType = 0;
};



struct PREDATA
{
    enum class OrderType
    {
        USERCOUNT,
        MESSAGECHANGE,
        SCENECHANGE_TOPLAY,
        SCENECHANGE_TOWORLD,
        TURNOFF,
        TURNON,
        TURNCHANGED,
        ROTATEANGLE,
        FOLLOWANGLE,
        PLAYERBLADEINSERTTRY,
        PLAYERBLADEINSERTED,
        FOLLOWINDEX,
        HEARTBEAT,
        SERVERCHATSHOOT,
        CLIENTCHATSHOOT,
        SOMECLIENTDEAD,
        GAMEEND,
        END, //ERROR
    };

    PREDATA()
    {}

    PREDATA
    (
        int Size,
        PREDATA::OrderType eOrder
    )
        : iSizeStandby(Size)
        , eOrderType(eOrder)
    {}



    int iSizeStandby = 0; //가변길이라서 아무도 모름
    OrderType eOrderType = OrderType::END;
};

class CMyCQ
{
public:
    class LockGuard
    {
    public:
        LockGuard(mutex& _m)
            : m(&_m)
        {
            m->lock();
        }
        ~LockGuard()
        {
            m->unlock();
        }
    public:
        void ForcedUnlock() { m->unlock(); }
    private:
        mutex* m = nullptr;
    };

private:
    char* queue;
    int capacity;
    int front, rear;
    int Size = 0;
    int m_iMessageCount = 0;
    mutex m;
public:
    CMyCQ() = delete;
    CMyCQ(int ByteSize);
    ~CMyCQ();

    mutex& GetMutex() { return m; }


    bool isEmpty();
    bool isFull();
    bool isFull_Add(int iSize);
    void enqueue_Int(int item);
    int dequeue_Int();
    void Dequeqe_Size(int memSize);
    void display();
    void* GetFrontPtr();
    void* GetRearPtr();
    void* GetFrontOffsetPtr(int Offset);
    void Dequq_N(int iSize);
    char* GetBuffer();
    int GetSize();

    void Enqueqe_Ptr(void* Ptr, int memSize);

    template<typename T>
    void Enqueqe_Instance(T& Instance)
    {
        bool bMessageReady = false;

        if (isFull() || isFull_Add(sizeof(T)))
        {
            return;
        }
        else if (isEmpty())
        {
            front = rear = 0;
            bMessageReady = true;
        }

        int spaceToEnd = capacity - rear - 1;
        if (front > rear && sizeof(T) > spaceToEnd)
        {
            memcpy(&queue + rear + 1, &Instance, spaceToEnd);
            memcpy(&queue, &Instance + spaceToEnd, sizeof(T) - spaceToEnd);
        }
        else
        {
            memcpy(&queue[rear], &Instance, sizeof(T));
        }

        rear = (rear + sizeof(T)) % capacity;
        Size += sizeof(T);
    }

    template<typename T>
    void Enqueqe_InstanceRVal(T&& Instance)
    {
        bool bMessageReady = false;
        if (isFull() || isFull_Add(sizeof(T)))
        {
            return;
        }
        else if (isEmpty())
        {
            front = rear = 0;
            //패킷을 넣는 순간에 비어있었다면 자고있을 스레드 = 누구도 Send를 걸지 않았고, GQCS에 걸려있을것
            bMessageReady = true;
        }

        int spaceToEnd = capacity - rear - 1;
        if (front > rear && sizeof(T) > spaceToEnd)
        {
            memcpy(&queue + rear + 1, &Instance, spaceToEnd);
            memcpy(&queue, &Instance + spaceToEnd, sizeof(T) - spaceToEnd);
        }
        else
        {
            memcpy(&queue[rear], &Instance, sizeof(T));
        }


        rear = (rear + sizeof(T)) % capacity;
        Size += sizeof(T);
    }

    template<typename T>
    T* GetBufferT() { return (T*)queue; }

    template<typename T>
    void DisplayAll()
    {
        int iTypeSize = sizeof(T);

        if (isEmpty())
        {
            cout << "Queue is empty.\n";
            return;
        }
        else if (front <= rear)
        {
            T* ReadPtr = nullptr;

            cout << "Queue: ";
            for (int i = front; i <= rear / iTypeSize; i++)
            {
                ReadPtr = (T*)(&queue[i * iTypeSize]);
                cout << *ReadPtr << " ";
            }
        }
        else
        {
            T* ReadPtr = nullptr;

            cout << "Queue: ";
            for (int i = front; i <= rear / iTypeSize; i++)
            {
                ReadPtr = (T*)(&queue[i * iTypeSize]);
                cout << *ReadPtr << " ";
            }

            for (int i = front; i <= rear / iTypeSize; i++)
            {
                ReadPtr = (T*)(&queue[i * iTypeSize]);
                cout << *ReadPtr << " ";
            }
        }
        cout << endl;
    }
};

struct PlayingRoomSessionDesc
{
    int MyNumber = -1;
    void* MyRoomPtr = nullptr; //For Server;
};

struct PAK_ROTATEANGLE
{
    PlayingRoomSessionDesc RoomSessionDesc = {};
    float Angle = 0.0f;
};

struct PAK_BLADEINSERT
{
    PlayingRoomSessionDesc RoomSessionDesc = {};
    int Index = 0;
};

struct PAK_INSERTFOLLOW
{
    int PlayerIndex = 0;
    int HoldIndex = 0;
};

struct ClientSession
{
    WSAOVERLAPPED Overlapped_Send;
    WSAOVERLAPPED Overlapped_Recv;
    SOCKET soc = INVALID_SOCKET;
    WSABUF wsaBuf_Send = {};
    WSABUF wsaBuf_Recv = {};
    __int32 eType = 0;
    char recvBuffer[BUFSIZE] = {};
    __int32 recvSize = 0;
    __int32 sendSize = 0;
    __int32 ByteTransferred = 0;
    __int32 ByteToSent = 0;
    __int32 ByteToRead = 0;
    CMyCQ* CQPtr = nullptr;
    DWORD Respones = {};
    int LateCount = 0;
    bool bHeaderTransferred = false;
    PREDATA pLatestHead;

    void* PlayingRoomPtr = nullptr;

    enum class ClientState
    {
        WAITING,
        SCENECHANGE_PLAY,
        PLAYING,
        NOMESSAGE,
        TURNON,
        TURNOFF,
        END,
    };

    ClientState eClientState = ClientState::END;

    ~ClientSession()
    {
        if (CQPtr != nullptr)
        {
            delete CQPtr;
            CQPtr = nullptr;
        }
    }

    mutex WSAMutex_Send = {};
    mutex WSAMutex_Recv = {};
};







template<typename T>
bool MySend(ClientSession* pSession, T& Instance, PREDATA::OrderType Type)
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
        sizeof(T),
        Type
    ));
    pSession->CQPtr->Enqueqe_Instance<T>(Instance);

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
                closesocket(pSession->soc);
            }
        }
    }

    return Return;
}

bool MySend_Ptr(ClientSession* pSession, void* Ptr, int Size, PREDATA::OrderType Type);



void WorkerEntry(HANDLE hHandle, WSABUF* pOut);