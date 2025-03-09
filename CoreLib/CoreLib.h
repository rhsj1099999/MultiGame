#pragma once
#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <list>
#include <mutex>
#include <map>
#include <queue>


#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <mutex>
#include <iostream>
#include <fstream>


#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define HOLE_HORIZON 8
#define HOLE_VERTICAL 3

#define MAXCLIENTS 3
#define ALLOWMAXCLIENTS 7

#define CLIENT3 3
#define CLIENT1 1
#define WINEFFECTMAXSPEED 4
#define WINEFFECTMINSPEED 1

#define WINEFFECTMAXROTSPEED 30
#define WINEFFECTMINROTSPEED 5


#define BUF64 64
#define BUF128 128
#define BUF256 256
#define BUF384 384
#define BUF512 512
#define BUF1024 1024

#define BUFSIZE 100
#define BUFSIZE_100 100
#define BUFSIZE_1000 1000


#define MAXCHATLEN 32
#define MAXCHATLEN_TOC (MAXCHATLEN * 2)
#define CMPCHAT 150
#define NULLSIZE 1
#define MAXCHATTINGSHOW 4000
#define MESSAGEYDIFF 30

#define SENDHEARTBEATCYCLE 2000
#define MAXCONNETTIME 10000
#define MAXTIMEOUT 3000
#define MAXLATECOUNT 2


#define SR1_MSGBOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Fail_"), MB_OK)
#define SR1_MSGBOX2(MESSAGE) MessageBox(0, MESSAGE, TEXT("Fail_"), MB_OK)


class LockGuard
{
    public:     LockGuard(mutex& mutex)
        : _mutex(&mutex)
    {
        _mutex->lock();
    }
    public:     ~LockGuard()
    {
        _mutex->unlock();
    }

    private:    mutex* _mutex = nullptr;
};


#pragma region Circular Queue

class CircularQueue
{
    private:    char* _data;
    private:    int _capacity;
    private:    int _front;
    private:    int _rear;
    private:    int _size = 0;
    private:    int _messageCount = 0;

    private:    mutex _mutex;
    public:     mutex& GetMutex() { return _mutex; }

    public:     CircularQueue() = delete;
    public:     CircularQueue(int ByteSize);
    public:     ~CircularQueue();
    public:     bool isEmpty();
    public:     bool isFull();
    public:     bool isFull_Add(int iSize);
    public:     void enqueue_Int(int item);
    public:     int dequeue_Int();
    public:     void Dequeqe_Size(int memSize);
    public:     void display();
    public:     void* GetFrontPtr() { return &_data[_front]; }
    public:     void* GetRearPtr() { return &_data[_rear]; }
    public:     void* GetFrontOffsetPtr(int Offset);
    public:     void Dequq_N(int iSize);
    public:     char* GetBuffer();
    public:     int GetSize();
    public:     void Enqueqe_Ptr(void* Ptr, int memSize);

      template<typename T>
      void Enqueqe_Instance(T& Instance, int variableSize = -1)
      {
          int size = (variableSize < 0)
              ? sizeof(T)
              : variableSize;

          bool bMessageReady = false;

          if (isFull() || isFull_Add(size))
          {
              return;
          }
          else if (isEmpty())
          {
              _front = _rear = 0;
              bMessageReady = true;
          }

          int spaceToEnd = _capacity - _rear - 1;
          if (_front > _rear && size > spaceToEnd)
          {
              memcpy(&_data + _rear + 1, &Instance, spaceToEnd);
              memcpy(&_data, &Instance + spaceToEnd, size - spaceToEnd);
          }
          else
          {
              memcpy(&_data[_rear], &Instance, size);
          }

          _rear = (_rear + size) % _capacity;
          _size += size;
      }

      template<typename T>
      T* GetBufferT() { return (T*)_data; }

      template<typename T>
      void DisplayAll()
      {
          int iTypeSize = sizeof(T);

          if (isEmpty())
          {
              cout << "Queue is empty.\n";
              return;
          }
          else if (_front <= _rear)
          {
              T* ReadPtr = nullptr;

              cout << "Queue: ";
              for (int i = _front; i <= _rear / iTypeSize; i++)
              {
                  ReadPtr = (T*)(&_data[i * iTypeSize]);
                  cout << *ReadPtr << " ";
              }
          }
          else
          {
              T* ReadPtr = nullptr;

              cout << "Queue: ";
              for (int i = _front; i <= _rear / iTypeSize; i++)
              {
                  ReadPtr = (T*)(&_data[i * iTypeSize]);
                  cout << *ReadPtr << " ";
              }

              for (int i = _front; i <= _rear / iTypeSize; i++)
              {
                  ReadPtr = (T*)(&_data[i * iTypeSize]);
                  cout << *ReadPtr << " ";
              }
          }
          cout << endl;
      }
};
#pragma endregion Circular Queue

bool ReadStringFromFile(string* directory, string* ret);

bool WSAGetLastError_MessageReport(int ret);




class PacketHeader
{
    public:     enum class PacketType
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
        FORCEDGAMEEND,
        END, //ERROR
    };

    public:     PacketHeader() {}
    public:     PacketHeader
    (
        int Size,
        PacketHeader::PacketType eOrder
    )
        : _packetSize(Size)
        , _packetType(eOrder)
    {}

    public:     int _packetSize = 0;
    public:     PacketType _packetType = PacketType::END;
};


struct PacketBase 
{
    PacketBase() {}
    PacketBase(PacketHeader::PacketType eOrder) {}
    virtual int GetPacketSize() = 0;
};


struct PAK_HeartBeat : PacketBase
{
    PAK_HeartBeat() {}
    int GetPacketSize() override { return sizeof(PAK_HeartBeat); }
};



struct PAK_UserCount : PacketBase
{
    PAK_UserCount
    (
        int usercount
    )
        : _usercount(usercount){}

    int GetPacketSize() override { return sizeof(PAK_UserCount); }

    int _usercount = 0;
};




struct PAK_ROTATEANGLE_Follow : PacketBase
{
    PAK_ROTATEANGLE_Follow
    (
        float angle
    )
        : _angle(angle) {}

    int GetPacketSize() override { return sizeof(PAK_ROTATEANGLE_Follow); }

    float _angle = 0.0f;
};



struct PAK_INSERTFOLLOW : PacketBase
{
    PAK_INSERTFOLLOW
    (
        int inertPlayerIndex,
        int insertedIndex
    )
        : _inertPlayerIndex(inertPlayerIndex)
        , _insertedIndex(insertedIndex) {}

    int GetPacketSize() override { return sizeof(PAK_INSERTFOLLOW); }

    int _inertPlayerIndex = 0;
    int _insertedIndex = 0;
};


struct PAK_TurnChanged : PacketBase
{
    PAK_TurnChanged
    (
        int nextPlayerIndex
    )
        : _nextPlayerIndex(nextPlayerIndex) {}

    int GetPacketSize() override { return sizeof(PAK_TurnChanged); }

    int _nextPlayerIndex = 0;
};


struct PAK_GameEnd : PacketBase
{
    PAK_GameEnd
    (
        int winPlayer
    )
        : _winPlayer(winPlayer) {}

    int GetPacketSize() override { return sizeof(PAK_GameEnd); }

    int _winPlayer = 0;
};


struct PAK_ChattingMessage
{
    enum class MSGType
    {
        Sys,
        User,
        END,
    };

    PAK_ChattingMessage
    (
        MSGType Type,
        DWORD SendTime,
        const wchar_t* Buffer
    )
        : _messageType(Type)
        , _time(SendTime) 
    {
        _message.assign(Buffer);
    }

    MSGType _messageType = MSGType::END;
    wstring _message;
    DWORD _time;

    int GetStrSize() { return (_message.length() * sizeof(wchar_t)); }
};


struct PAK_ChattingToSingle : PacketBase, PAK_ChattingMessage
{
    PAK_ChattingToSingle
    (
        MSGType Type,
        DWORD SendTime,
        const wchar_t* Buffer
    )
        : PAK_ChattingMessage(Type, SendTime, Buffer) {}

    int GetPacketSize() override { return sizeof(PAK_ChattingToSingle) + GetStrSize(); }
};





struct PlayingRoomSessionDesc
{
    int _myNumber = -1;
    void* _myRoomPtr = nullptr;
};




struct PacketBaseRoomBroadCast : PacketBase
{
    PacketBaseRoomBroadCast
    (
        int playerIndex,
        void* roomPtr
    )
        : _myNumber(playerIndex)
        , _myRoomPtr(roomPtr) {}

    int _myNumber = -1;
    void* _myRoomPtr = nullptr;

    int GetPacketSize() override { return sizeof(PacketBaseRoomBroadCast); }
};



struct PAK_SceneChange : PacketBaseRoomBroadCast
{
    PAK_SceneChange
    (
        int playerIndex,
        void* roomPtr
    )
        : PacketBaseRoomBroadCast(playerIndex, roomPtr) {}

    int GetPacketSize() override { return sizeof(PAK_SceneChange); }
};



struct PAK_ROTATEANGLE : PacketBaseRoomBroadCast
{
    PAK_ROTATEANGLE
    (
        int playerIndex,
        void* roomPtr,
        float angle
    )
        : PacketBaseRoomBroadCast(playerIndex, roomPtr)
        , _angle(angle) {}

    float _angle = 0.0f;

    int GetPacketSize() override { return sizeof(PAK_ROTATEANGLE); }
};




struct PAK_BLADEINSERT : PacketBaseRoomBroadCast
{
    PAK_BLADEINSERT
    (
        int playerIndex,
        void* roomPtr,
        int insertedIndex
    )
        : PacketBaseRoomBroadCast(playerIndex, roomPtr)
        , _insertedIndex(insertedIndex) {}

    int _insertedIndex = 0;

    int GetPacketSize() override { return sizeof(PAK_ROTATEANGLE); }
};



struct PAK_ChattingMessageToRoom : PacketBaseRoomBroadCast, PAK_ChattingMessage
{
    PAK_ChattingMessageToRoom
    (
        int playerIndex,
        void* roomPtr,

        MSGType Type,
        DWORD SendTime,
        wchar_t* Buffer
    )
        : PacketBaseRoomBroadCast(playerIndex, roomPtr)
        , PAK_ChattingMessage(Type, SendTime, Buffer) {}

    int GetPacketSize() override { return sizeof(PAK_ChattingMessageToRoom) + GetStrSize(); }
};













struct ClientSession
{
    enum class IOType
    {
        Read,
        Write,
    };

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

    ~ClientSession()
    {
        if (_circularQueue != nullptr)
        {
            delete _circularQueue;
            _circularQueue = nullptr;
        }
    }

    ClientState _clientState = ClientState::END;
    IOType _ioType = IOType::Read;


    WSAOVERLAPPED _overlapped_Send;
    WSAOVERLAPPED _overlapped_Recv;

    SOCKET _socket = INVALID_SOCKET;

    WSABUF _wsaBuffer_Send = {};
    mutex _mutex_wsaBuffer_Send = {};
    int _recvSize = 0;
    int _byteToSend = 0;

    WSABUF _wsaBuffer_Recv = {};
    mutex _mutex_wsaBuffer_Recv = {};
    int _sendSize = 0;
    int _byteToRead = 0;

    char _buffer[BUFSIZE] = {};

    int _byteTransferred = 0;

    CircularQueue* _circularQueue = nullptr;
    PacketHeader _latestSendedHeader;
    DWORD _responedTime = {};

    int _lateCount = 0;
    bool _isHeaderTransferred = false;

    void* PlayingRoomPtr = nullptr;
};




template<typename T>
bool MySend(ClientSession* pSession, T& Instance, PacketHeader::PacketType packetType)
{
    bool Return = true;

    if (pSession->_socket == INVALID_SOCKET)
        return false;

    LockGuard Temp(pSession->_circularQueue->GetMutex());

    bool bTempMessageSend = (pSession->_circularQueue->GetSize() == 0);

    PacketHeader header = PacketHeader();
    header._packetType = packetType;
    header._packetSize = Instance.GetPacketSize();

    int variableSize = (header._packetSize == sizeof(T))
        ? -1 //고정길이 패킷입니다
        : header._packetSize;

    pSession->_circularQueue->Enqueqe_Instance<PacketHeader>(header);

    pSession->_circularQueue->Enqueqe_Instance<T>(Instance, variableSize);

    if (bTempMessageSend == true)
    {
        DWORD recvLen = 0;
        DWORD flag = 0;

        pSession->_wsaBuffer_Send.buf = (char*)pSession->_circularQueue->GetFrontPtr();
        pSession->_wsaBuffer_Send.len = pSession->_circularQueue->GetSize();

        if (WSASend((pSession)->_socket, &pSession->_wsaBuffer_Send, 1, &recvLen, flag, &(pSession)->_overlapped_Send, NULL) == SOCKET_ERROR)
        {
            int ERR = WSAGetLastError();
            if (ERR != WSAEWOULDBLOCK && ERR != WSA_IO_PENDING)
            {
                Return = false;
                closesocket(pSession->_socket);
            }
        }
    }

    return Return;
}


void WorkerEntry(HANDLE hHandle, WSABUF* pOut);

class CBase
{
    public:     virtual void Release() = 0;
};
