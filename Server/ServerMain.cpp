#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

#include <CoreLib.h>
#pragma comment(lib, "CoreLib.lib")

using namespace std;

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        //Break;
    }

    SOCKET socReciver(socket
    (
        AF_INET,
        SOCK_STREAM,
        0
    ));

    if (socReciver == INVALID_SOCKET)
    {
        //Break;
    }

    unsigned long On = 1;
    ioctlsocket(socReciver, FIONBIO, &On);
    bool bEnable = true;
    setsockopt(socReciver, SOL_SOCKET, SO_REUSEADDR, (const char*)&bEnable, sizeof(bEnable));
    setsockopt(socReciver, IPPROTO_TCP, TCP_NODELAY, (const char*)&bEnable, sizeof(bEnable));

    SOCKADDR_IN socadServerAddr;
    ZeroMemory(&socadServerAddr, sizeof(socadServerAddr));
    socadServerAddr.sin_family = AF_INET;
    socadServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    socadServerAddr.sin_port = htons(7777);

    if (bind(socReciver, (SOCKADDR*)&socadServerAddr, sizeof(socadServerAddr)) == SOCKET_ERROR)
    {
        //Break;
    }

    cout << "Now Listen Try" << endl;

    if (listen(socReciver, 10) == SOCKET_ERROR)
    {
        //Break;
    }

    vector<Session*> vecSessions;
    vector<thread> vecThreads;

    HANDLE hCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); //만들때는 이렇게
    for (int i = 0; i < 5; i++)
    {
        vecThreads.push_back(thread([=]()
            {
                WorkerEntry(hCPHandle, nullptr);
            }));
    }


    while (true)
    {
        SOCKET ClientSocket;
        SOCKADDR_IN ClientAddress;
        __int32 AddrLen = sizeof(ClientAddress);

        while (true)
        {
            ClientSocket = accept(socReciver, (SOCKADDR*)&ClientAddress, &AddrLen);

            if (ClientSocket == INVALID_SOCKET)
            {
                //Break;
            }

            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                continue;
            }
            else
            {
                break;
            }
        }


        Session* pSession = new Session{ ClientSocket };
        vecSessions.push_back(pSession);
        CreateIoCompletionPort((HANDLE)ClientSocket, hCPHandle, /*Key*/(ULONG_PTR)pSession, 0); //등록할때는
        cout << "Client Connected!" << endl;

        OverlappedExtend* OverlapEX = new OverlappedExtend();
        OverlapEX->eType = QUEUEWATING;

        WSABUF wsaBuf;

        int TempCurrUser = static_cast<int>(vecSessions.size());
        memset(pSession->recvBuffer, 0, sizeof(pSession->recvBuffer));
        memcpy(pSession->recvBuffer, &TempCurrUser, sizeof(TempCurrUser));

        wsaBuf.buf = pSession->recvBuffer;
        wsaBuf.len = BUFSIZE;

        DWORD recvLen = 0;
        DWORD flag = 0;

        WSASend(ClientSocket, &wsaBuf, 1, &recvLen, flag, &OverlapEX->OverlappedEvent, nullptr);



     /*   OverlappedExtend* OverlapEX = new OverlappedExtend();
        OverlapEX->eType = READ;

        WSABUF wsaBuf;
        wsaBuf.buf = pSession->recvBuffer;
        wsaBuf.len = BUFSIZE;
        DWORD recvLen = 0;
        DWORD flag = 0;
        WSARecv(ClientSocket, &wsaBuf, 1, &recvLen, &flag, &OverlapEX->OverlappedEvent, NULL);*/
    }


    for (vector<thread>::iterator Itr = vecThreads.begin(); Itr != vecThreads.end(); ++Itr)
    {
        Itr->join();
    }
    for (Session* Ss : vecSessions)
    {
        delete Ss;
    }
    WSACleanup();
    return 0;
}