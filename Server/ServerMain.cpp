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

#define SR1_MSGBOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Fail_"), MB_OK)
#define SR1_MSGBOX2(MESSAGE) MessageBox(0, MESSAGE, TEXT("Fail_"), MB_OK)

__int32 iCurrentUser = 0;


list<ClientSession*> liClientSessions;
list<CMyCQ*> liClietSendQueue;

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
                break;
            }
            continue;
        }

        //pSession->ByteTransferred += Bytes;
        //pSession->ByteToSent -= Bytes;

        //if (pSession->ByteToSent < 0)
        //{
        //    SR1_MSGBOX("Error : ByteToSent Under 0");
        //}

        pSession->CQPtr->Dequq_N(Bytes);

        if (pSession->CQPtr->GetSize() <= 0)
        {
            //다 보냈다
            pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
            (
                sizeof(int),
                PREDATA::OrderType::TEST1
            ));
            pSession->CQPtr->Enqueqe_Instance<__int32>(iCurrentUser);
        }
        else
        {
            //다 보내지 못했다.
            SR1_MSGBOX("Error : ByteToSent Sent Not Yet");
            int a = 10;
        }

        pSession->wsaBuf.len = pSession->CQPtr->GetSize();
        pSession->wsaBuf.buf = (char*)pSession->CQPtr->GetFrontPtr();
        

        DWORD recvLen = 0;
        DWORD flag = 0;

        if (WSASend((pSession)->soc, &pSession->wsaBuf, 1, &recvLen, flag, &(pSession)->OverlappedEvent, NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSA_IO_PENDING)
            {

            }
        }
    }
}

struct MainPacket
{
    MainPacket() = delete;
    MainPacket(int DataSize, PREDATA::OrderType eOrderType, void* pData)
        : Head(PREDATA(DataSize, eOrderType))
    {
        Data = new char[DataSize + sizeof(PREDATA)];
        memset(Data, 0, DataSize);
        memcpy(Data, (char*)&Head, sizeof(PREDATA));
        memcpy(&Data[sizeof(PREDATA)], (char*)pData, DataSize);
    }
    ~MainPacket()
    {
        if (Data != nullptr)
        {
            delete Data;
        }
    }
    PREDATA Head;
    char* Data = nullptr;
    int GetSize() { return (Head.iSizeStandby + sizeof(PREDATA)); }
    char* GetBuffer() { return Data; }
};


enum class MainPacketType
{
    UserCount = 0,
    END,
};


int main()
{
    Timer* pTimer = Timer::GetInstance();
    pTimer->Init();

    vector<MainPacket*> vecPacketPool;
    vecPacketPool.resize(static_cast<int>(MainPacketType::END));
    vecPacketPool[static_cast<int>(MainPacketType::UserCount)] = new MainPacket(sizeof(iCurrentUser), PREDATA::OrderType::TEST1, &iCurrentUser);



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
        pSession->ByteTransferred = 0;
        liClientSessions.push_back(pSession);
        pSession->CQPtr = new CMyCQ(64);
        CreateIoCompletionPort((HANDLE)ClientSocket, hCPHandle, /*Key*/(ULONG_PTR)pSession, 0); //등록할때는

        iCurrentUser = static_cast<__int32>(liClientSessions.size());
        cout << "Client Connected!" << endl;

        memcpy(pSession->recvBuffer, &iCurrentUser, sizeof(iCurrentUser));
        cout << iCurrentUser << endl;

        DWORD recvLen = 0;
        DWORD flag = 0;




        pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
        (
            sizeof(int),
            PREDATA::OrderType::TEST1
        ));
        pSession->CQPtr->Enqueqe_Instance<__int32>(iCurrentUser);

        int Size = pSession->CQPtr->GetSize();
        
        //pSession->ByteToSent = sizeof(iCurrentUser);
        //pSession->wsaBuf.buf = pSession->recvBuffer;
        //pSession->wsaBuf.len = pSession->ByteToSent;
        //WSASend(pSession->soc, &pSession->wsaBuf, 1, &recvLen, flag, &pSession->OverlappedEvent, NULL);

        //After Circular Queue
        pSession->ByteToSent = pSession->CQPtr->GetSize();
        pSession->wsaBuf.buf = pSession->CQPtr->GetBuffer();
        pSession->wsaBuf.len = pSession->CQPtr->GetSize();
        WSASend(pSession->soc, &pSession->wsaBuf, 1, &recvLen, flag, &pSession->OverlappedEvent, NULL);
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
    for (MainPacket* pMP : vecPacketPool)
    {
        delete pMP;
    }
    vecPacketPool.clear();
    return 0;
}