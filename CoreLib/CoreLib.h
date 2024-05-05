#pragma once
#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <mutex>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define BUF64 64
#define BUF128 128
#define BUF256 256
#define BUF512 512
#define BUF1024 1024

#define BUFSIZE 100
#define BUFSIZE_100 100
#define BUFSIZE_1000 1000



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
        TEST2,
        MESSAGECHANGE,
        SCENECHANGE_TOPLAY,
        SCENECHANGE_TOWORLD,
        TURNOFF,
        TURNON,
        ROTATEANGLE,
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
    private:
        mutex* m = nullptr;
    };

private:
    char* queue;
    int capacity;
    int front, rear;
    int Size = 0;
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
    void Enqueqe_Ptr(void* Ptr, int memSize);
    int dequeue_Int();
    void Dequeqe_Size(int memSize);
    void display();
    void* GetFrontPtr();
    void* GetRearPtr();
    void* GetFrontOffsetPtr(int Offset);
    void Dequq_N(int iSize);
    char* GetBuffer();
    int GetSize();

    template<typename T>
    void Enqueqe_Instance(T& Instance)
    {
        
        if (isFull() || isFull_Add(sizeof(T)))
        {
            return;
        }
        else if (isEmpty())
        {
            front = rear = 0;
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
        
        if (isFull() || isFull_Add(sizeof(T)))
        {
            return;
        }
        else if (isEmpty())
        {
            front = rear = 0;
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
    T* GetBufferT() {  return (T*)queue; }

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
        }
    }

    mutex WSAMutex_Send = {};
    mutex WSAMutex_Recv = {};

public:
    int Lock_WSASend(_In_reads_(dwBufferCount) LPWSABUF lpBuffers,
        _In_ DWORD dwBufferCount,
        _Out_opt_ LPDWORD lpNumberOfBytesSent,
        _In_ DWORD dwFlags,
        _Inout_opt_ LPWSAOVERLAPPED lpOverlapped,
        _In_opt_ LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
    {
        WSAMutex_Send.lock();
        int Ret = WSASend(soc, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
        WSAMutex_Send.unlock();
        return Ret;
    }
    int Lock_WSARecv(_In_reads_(dwBufferCount) __out_data_source(NETWORK) LPWSABUF lpBuffers,
        _In_ DWORD dwBufferCount,
        _Out_opt_ LPDWORD lpNumberOfBytesRecvd,
        _Inout_ LPDWORD lpFlags,
        _Inout_opt_ LPWSAOVERLAPPED lpOverlapped,
        _In_opt_ LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
    {
        
        WSAMutex_Recv.lock();
        int Ret = WSARecv(soc, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
        WSAMutex_Recv.unlock();
        return Ret;
    }
};

void WorkerEntry(HANDLE hHandle, WSABUF* pOut);