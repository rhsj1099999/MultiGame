#pragma once
#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 100
#define BUFSIZE_100 100
#define BUFSIZE_1000 1000

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