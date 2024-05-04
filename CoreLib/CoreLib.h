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
    //class LockGuard
    //{
    //public:
    //    LockGuard(mutex& _m)
    //        : m(&_m) 
    //    {
    //        m->lock();
    //    }
    //    ~LockGuard() 
    //    {
    //        m->unlock();
    //    }
    //private:
    //    mutex* m = nullptr;
    //};

    class LockGuard
    {
    public:
        LockGuard(mutex& _m)
            : m(&_m) {
            while (true)
            {
                if (m->try_lock() == true)
                {
                    break;
                }

            }
        }
        ~LockGuard() { m->unlock(); }
    private:
        mutex* m;
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
        LockGuard G(m);
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
        LockGuard G(m);
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
    T* GetBufferT() { LockGuard G(m); return (T*)queue; }

    template<typename T>
    void DisplayAll()
    {
        LockGuard G(m);
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
    WSAOVERLAPPED OverlappedEvent;
    SOCKET soc = INVALID_SOCKET;
    WSABUF wsaBuf = {};
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
};

void WorkerEntry(HANDLE hHandle, WSABUF* pOut);