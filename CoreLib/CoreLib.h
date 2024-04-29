#pragma once
#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF64 64
#define BUF128 128
#define BUF256 256
#define BUF512 512
#define BUF1024 1024

#define BUFSIZE 100
#define BUFSIZE_100 100
#define BUFSIZE_1000 1000

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
    DWORD Respones = {};
    int LateCount = 0;
};

struct Session
{
    SOCKET soc = INVALID_SOCKET;
    char recvBuffer[BUFSIZE_100] = {};
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


void WorkerEntry(HANDLE hHandle, WSABUF* pOut);