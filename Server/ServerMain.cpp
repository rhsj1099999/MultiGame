#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <list>
#include <mutex>
#include <map>


#include <CoreLib.h>
#pragma comment(lib, "CoreLib.lib")

#include "Timer.h"

using namespace std;

list<ClientSession*> liClientSessions;

void CheckResponse()
{

}

void WorkerEntry_D(HANDLE hHandle, WSABUF* pOut)
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
            if (WSAGetLastError() == WAIT_TIMEOUT)
            {
                int a = 10;
                break;
            }
            continue;
        }

        DWORD ResponeTime = pSession->Respones;

        if ((Timer::GetInstance()->GetCurrTime() - pSession->Respones) > 1000)
        {
            ++pSession->LateCount;
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


            pSession->ByteTransferred += Bytes;
            if (pSession->ByteTransferred < pSession->ByteToSent)
            {
                pSession->wsaBuf.buf = &(pSession->recvBuffer[pSession->ByteTransferred]);
                pSession->wsaBuf.len = sizeof(pSession->recvBuffer - pSession->ByteTransferred);
            }
            else
            {
                int TempCurrUser = static_cast<int>(liClientSessions.size());
                //memset(pSession->recvBuffer, 0, sizeof(pSession->recvBuffer));
                memcpy(pSession->recvBuffer, &TempCurrUser, sizeof(TempCurrUser));

                pSession->wsaBuf.buf = pSession->recvBuffer;
                pSession->wsaBuf.len = 4;
                pSession->ByteTransferred = 0;
                pSession->ByteToSent = sizeof(int);
            }


            //pSession->wsaBuf.buf = pSession->recvBuffer;
            //pSession->wsaBuf.len = sizeof(pSession->recvBuffer);

            DWORD recvLen = 0;
            DWORD flag = 0;
            WSASend((pSession)->soc, &pSession->wsaBuf, 1, &recvLen, flag, &(pSession)->OverlappedEvent, NULL);
        }
        break;


        default:
            break;
        }
    }
}



int main()
{
    Timer* pTimer = Timer::GetInstance();
    pTimer->Init();


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



    char Wating[BUFSIZE] = {};

    vector<thread> vecThreads;

    HANDLE hCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); //만들때는 이렇게
    for (int i = 0; i < 10; i++)
    {
        vecThreads.push_back(thread([=]()
            {
                WorkerEntry_D(hCPHandle, nullptr);
            }));
    }

    map<wstring, OverlappedExtend*> mapOverlaped;

    SOCKET ClientSocket;
    SOCKADDR_IN ClientAddress;
    __int32 AddrLen = sizeof(ClientAddress);


    while (true)
    {
        pTimer->Tick();


#pragma region ConnectTry
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
#pragma endregion ConnectTry

        ClientSession* pSession = new ClientSession;
        pSession->soc = ClientSocket;
        pSession->OverlappedEvent = {};
        pSession->OverlappedEvent.hEvent = WSACreateEvent();
        pSession->eType = QUEUEWATING;
        pSession->Respones = Timer::GetInstance()->GetCurrTime();
        pSession->LateCount = 0;
        CreateIoCompletionPort((HANDLE)ClientSocket, hCPHandle, /*Key*/(ULONG_PTR)pSession, 0); //등록할때는
        liClientSessions.push_back(pSession);
        cout << "Client Connected!" << endl;

        int TempCurrUser = static_cast<int>(liClientSessions.size());
        memset(pSession->recvBuffer, 0, sizeof(pSession->recvBuffer));
        memcpy(pSession->recvBuffer, &TempCurrUser, sizeof(TempCurrUser));

        DWORD recvLen = 0;
        DWORD flag = 0;

        pSession->wsaBuf.buf = pSession->recvBuffer;
        pSession->wsaBuf.len = 4;
        pSession->ByteTransferred = 0;
        pSession->ByteToSent = sizeof(int);
        if (WSASend((pSession)->soc, &pSession->wsaBuf, 1, &recvLen, flag, &(pSession)->OverlappedEvent, NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                delete pSession;
            }
        }
    }


    for (vector<thread>::iterator Itr = vecThreads.begin(); Itr != vecThreads.end(); ++Itr)
    {
        Itr->join();
    }
    //for (Session* Ss : vecSessions)
    //{
    //    closesocket(Ss->soc);
    //    delete Ss;
    //}
    closesocket(socReciver);
    WSACleanup();
    return 0;
}