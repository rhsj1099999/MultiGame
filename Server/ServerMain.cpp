#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>
#include <map>

#include <CoreLib.h>
#pragma comment(lib, "CoreLib.lib")

using namespace std;

int main()
{
#pragma region InitServer
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
#pragma endregion InitServer
    vector<Session*> vecSessions;

    vector<Session*> vecWatingSessions;
    vector<Session*> vecGamingSessions;

    WSABUF WatingBuf;
    char Wating[BUFSIZE] = {};


    vector<thread> vecThreads;

    HANDLE hCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); //만들때는 이렇게
    for (int i = 0; i < 5; i++)
    {
        vecThreads.push_back(thread([=]()
            {
                WorkerEntry(hCPHandle, nullptr);
            }));
    }

    map<wstring, OverlappedExtend*> mapOverlaped;


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

        int TempCurrUser = static_cast<int>(vecSessions.size());
        memset(Wating, 0, sizeof(Wating));
        memcpy(Wating, &TempCurrUser, sizeof(TempCurrUser));

        WatingBuf.buf = Wating;
        WatingBuf.len = BUFSIZE;

        DWORD recvLen = 0;
        DWORD flag = 0;

        for (Session* pSS : vecSessions)
        {
            memset(pSS->recvBuffer, 0, sizeof(pSS->recvBuffer));
            memcpy(pSS->recvBuffer, &TempCurrUser, sizeof(TempCurrUser));

            WSASend(pSS->soc, &WatingBuf, 1, &recvLen, flag, &OverlapEX->OverlappedEvent, nullptr);
        }
        
    }


    for (vector<thread>::iterator Itr = vecThreads.begin(); Itr != vecThreads.end(); ++Itr)
    {
        Itr->join();
    }
    for (Session* Ss : vecSessions)
    {
        closesocket(Ss->soc);
        delete Ss;
    }
    WSACleanup();
    return 0;
}