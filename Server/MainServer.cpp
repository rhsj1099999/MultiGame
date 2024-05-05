#include "stdafx.h"
#include "MainServer.h"


#include "Timer.h"
#include "PlayingRoom.h"

CMainServer* CMainServer::m_pInstance = nullptr;

mutex TempLock;

static int ChangedCount = 0;

CMainServer::CMainServer()
{
}

CMainServer::~CMainServer()
{
}

void CMainServer::Release()
{
    if (m_IOCPHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_IOCPHandle);
        m_IOCPHandle = INVALID_HANDLE_VALUE;
    }

    for (ClientSession* pCS : m_liClientSockets)
    {
        closesocket(pCS->soc);
    }

    closesocket(m_Socket);

    WSACleanup();

    for (vector<thread>::iterator Itr = m_vecWorkerThreads.begin(); Itr != m_vecWorkerThreads.end(); ++Itr)
    {
        Itr->join();
    }

    for (ClientSession* pCS : m_liClientSockets)
    {
        delete pCS;
    }
}

void CMainServer::Init()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa))
    {
        //Break;
    }

    m_Socket = socket
    (
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (m_Socket == INVALID_SOCKET)
    {
        //Break;
    }

    unsigned long On = 1;
    ioctlsocket(m_Socket, FIONBIO, &On);
    bool bEnable = true;
    setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bEnable, sizeof(bEnable));
    setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&bEnable, sizeof(bEnable));

    SOCKADDR_IN m_ServerAddr;
    ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
    m_ServerAddr.sin_family = AF_INET;
    m_ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_ServerAddr.sin_port = htons(7777);

    if (bind(m_Socket, (SOCKADDR*)&m_ServerAddr, sizeof(m_ServerAddr)) == SOCKET_ERROR)
    {
        //Break;
    }

    if (m_IOCPHandle == INVALID_HANDLE_VALUE)
        m_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    for (int i = 0; i < 2; i++)
    {
        m_vecWorkerThreads.push_back(thread([=]()
            {
                WorkerEntry_D(m_IOCPHandle);
            }));
    }

    cout << "Now Listen Try" << endl;

    if (listen(m_Socket, 10) == SOCKET_ERROR)
    {
        //Break;
    }
}

void CMainServer::Tick()
{
    ConnectTry();

    LiveCheck();

    MatchingRoom();

    //for (CPlayingRoom* pRoom : m_liPlayingRooms)
    //{
    //    pRoom->Tick();
    //}
}

bool CMainServer::SettingNextOrder(ClientSession* pSession)
{
    switch (pSession->eClientState)
    {
    case ClientSession::ClientState::WAITING:
    {
        pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
        (
            sizeof(int),
            PREDATA::OrderType::USERCOUNT
        ));
        pSession->CQPtr->Enqueqe_Instance<__int32>(m_iCurrUser);
    }
        break;

    case ClientSession::ClientState::SCENECHANGE_PLAY:
    {
        pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
        (
            sizeof(int),
            PREDATA::OrderType::SCENECHANGE_TOPLAY
        ));
        pSession->CQPtr->Enqueqe_Instance<__int32>(m_iCurrUser);
    }
        break;

    case ClientSession::ClientState::PLAYING:
        break;

    case ClientSession::ClientState::NOMESSAGE:
        break;

    case ClientSession::ClientState::TURNON:
        pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
        (
            sizeof(int),
            PREDATA::OrderType::TURNON
        ));
        pSession->CQPtr->Enqueqe_Instance<__int32>(m_iCurrUser);
        break;

    case ClientSession::ClientState::TURNOFF:
        pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
        (
            sizeof(int),
            PREDATA::OrderType::TURNON
        ));
        pSession->CQPtr->Enqueqe_Instance<__int32>(m_iCurrUser);
        break;

    case ClientSession::ClientState::END:
        break;

    default:
        break;
    }

    return false;
}



CMainServer* CMainServer::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new CMainServer;
	}
	return m_pInstance;
}

void CMainServer::Destroy_Instance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}


void CMainServer::ConnectTry()
{
    SOCKET ClientSocket;
    SOCKADDR_IN ClientAddress;
    __int32 AddrLen = sizeof(ClientAddress);

    ClientSocket = accept(m_Socket, (SOCKADDR*)&ClientAddress, &AddrLen);

    if (ClientSocket == INVALID_SOCKET)
        return;

    if (WSAGetLastError() == WSAEWOULDBLOCK)
        return;

    ClientSession* pSession = new ClientSession;
    pSession->soc = ClientSocket;
    pSession->Overlapped_Send = {};
    pSession->Overlapped_Recv = {};
    pSession->eType = QUEUEWATING;
    pSession->Respones = CTimer::GetInstance()->GetCurrTime();
    pSession->LateCount = 0;
    pSession->ByteTransferred = 0;
    pSession->eClientState = ClientSession::ClientState::WAITING;
    m_liClientSockets.push_back(pSession);
    pSession->CQPtr = new CMyCQ(64);
    m_queWaitingQueue.push(pSession);
    CreateIoCompletionPort((HANDLE)ClientSocket, m_IOCPHandle, /*Key*/(ULONG_PTR)pSession, 0);

    m_iCurrUser = static_cast<__int32>(m_liClientSockets.size());

    memcpy(pSession->recvBuffer, &m_iCurrUser, sizeof(m_iCurrUser));
    cout << "Client Connected Users : " << m_iCurrUser << endl;

    DWORD recvLen = 0;
    DWORD flag = 0;

    

    CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());
    pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
    (
        sizeof(int),
        PREDATA::OrderType::USERCOUNT
    ));
    pSession->CQPtr->Enqueqe_Instance<__int32>(m_iCurrUser);
    pSession->pLatestHead = PREDATA(sizeof(int),PREDATA::OrderType::USERCOUNT);
    pSession->ByteToSent = pSession->CQPtr->GetSize();
    pSession->wsaBuf_Send.buf = pSession->CQPtr->GetBuffer();
    pSession->wsaBuf_Send.len = pSession->CQPtr->GetSize();
    
    WSASend(pSession->soc, &pSession->wsaBuf_Send, 1, &recvLen, flag, &pSession->Overlapped_Send, NULL);
    WSARecv(pSession->soc, &pSession->wsaBuf_Recv, 1, &recvLen, &flag, &pSession->Overlapped_Recv, NULL);
}

void CMainServer::TickWatingClients()
{

}


void CMainServer::LiveCheck()
{
    DWORD dwCurrTime = CTimer::GetInstance()->GetCurrTime();

    for (list<ClientSession*>::iterator Itr = m_liClientSockets.begin(); Itr != m_liClientSockets.end(); ++Itr)
    {
        if (abs(static_cast<int>((*Itr)->Respones - dwCurrTime)) > MAXTIMEOUT)
        {
            ++(*Itr)->LateCount;

            if ((*Itr)->LateCount > MAXLATECOUNT)
            {
                closesocket((*Itr)->soc);

                delete (*Itr);

                Itr = m_liClientSockets.erase(Itr);

                m_iCurrUser = static_cast<int>(m_liClientSockets.size());

                cout << "Client TimeOut. Users : " << m_iCurrUser << endl;

                if (Itr == m_liClientSockets.end())
                    break;
            }
        }
    }
}

void CMainServer::MatchingRoom()
{
    if (m_queWaitingQueue.size() >= 1)
    {
        CPlayingRoom* pNewRoom = new CPlayingRoom();

        ClientSession* pArr[1] = { nullptr, };

        for (int i = 0; i < 1; i++)
        {
            ClientSession* pSession = m_queWaitingQueue.front();

            pArr[i] = m_queWaitingQueue.front();

            pSession->PlayingRoomPtr = pNewRoom;

            CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());

            m_queWaitingQueue.front()->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
            (
                sizeof(int),
                PREDATA::OrderType::SCENECHANGE_TOPLAY
            ));
            m_queWaitingQueue.front()->CQPtr->Enqueqe_Instance<int>(m_iCurrUser);

            DWORD Sendlen = {};
            DWORD Flag = {};
            m_queWaitingQueue.front()->Lock_WSASend
            (
                &pSession->wsaBuf_Send, 1, &Sendlen, Flag, &pSession->Overlapped_Send, NULL
            );

            m_queWaitingQueue.front()->eClientState = ClientSession::ClientState::SCENECHANGE_PLAY;
            m_queWaitingQueue.pop();
        }

        pNewRoom->Init(pArr, m_liClientSockets);
    }
}

void CMainServer::Lock_Session(VFPtr pFArr[], int ArrSize, void* Args[])
{
    for (int i = 0; i < ArrSize; ++i)
    {
        (this->*pFArr[i])(Args[i]);
    }
}

void CMainServer::Lock_Queue(VFPtr pFArr[], int ArrSize, void* Args[])
{

    for (int i = 0; i < ArrSize; ++i)
    {
        (this->*pFArr[i])(Args[i]);
    }

}

void CMainServer::Lock_Queue_Push(void* Ptr)
{
    ClientSession** Casted = static_cast<ClientSession**>(Ptr);
    m_queWaitingQueue.push((*Casted));
}

void CMainServer::Lock_Queue_ChangingRoom(void* Ptr)
{
    if (m_queWaitingQueue.size() >= 3)
    {
        for (int i = 0; i < 3; i++)
        {
            m_queWaitingQueue.front()->eClientState = ClientSession::ClientState::PLAYING;
            m_queWaitingQueue.pop();
        }
    }
}

void CMainServer::Lock_Session_ChangingState(void* Ptr)
{
}










void CMainServer::WorkerEntry_D(HANDLE hHandle)
{
    while (true)
    {
        DWORD Bytes = 0;
        ClientSession* pSession = nullptr;
        LPOVERLAPPED pOverlap = nullptr;

        bool bRet = GetQueuedCompletionStatus(hHandle, &Bytes, (ULONG_PTR*)&pSession, (LPOVERLAPPED*)&pOverlap, INFINITE);

        if (bRet == FALSE || Bytes == 0)
        {
            if (m_IOCPHandle == INVALID_HANDLE_VALUE)
                return;

            closesocket(pSession->soc);

            continue;
        }

        CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());

        if (pOverlap == &pSession->Overlapped_Send)
        {
#pragma region Send
            pSession->CQPtr->Dequq_N(Bytes);

            pSession->Respones = CTimer::GetInstance()->GetCurrTime();

            //if (pSession->CQPtr->GetSize() == 0)
            //{
            //    if (pSession->eClientState == ClientSession::ClientState::WAITING)
            //    {
            //        pSession->CQPtr->Enqueqe_InstanceRVal<PREDATA>(PREDATA
            //        (
            //            sizeof(int),
            //            PREDATA::OrderType::USERCOUNT
            //        ));
            //        pSession->CQPtr->Enqueqe_Instance<__int32>(m_iCurrUser);
            //    }
            //    else
            //    {
            //        continue; //메세지 없으면 자러갈꺼임
            //    }
            //    //SettingNextOrder(pSession);
            //}

            if (pSession->CQPtr->GetSize() == 0)
            {
                continue; //메세지 없으면 자러갈꺼임
            }

            pSession->wsaBuf_Send.len = pSession->CQPtr->GetSize();
            pSession->wsaBuf_Send.buf = (char*)pSession->CQPtr->GetFrontPtr();

            DWORD recvLen = 0;
            DWORD flag = 0;

            pSession->Lock_WSASend(&pSession->wsaBuf_Send, 1, &recvLen, flag, &(pSession)->Overlapped_Send, NULL);
            //if (WSASend((pSession)->soc, &pSession->wsaBuf, 1, &recvLen, flag, &(pSession)->Overlapped_Send, NULL) == SOCKET_ERROR)
            //{
            //    if (WSAGetLastError() != WSA_IO_PENDING)
            //    {
            //        closesocket(pSession->soc);
            //    }
            //}
#pragma endregion Send
        }
        else
        {
#pragma region Recv
            pSession->CQPtr->Dequq_N(Bytes);

            pSession->Respones = CTimer::GetInstance()->GetCurrTime();

            if (pSession->CQPtr->GetSize() == 0)
            {
                SettingNextOrder(pSession);
            }

            pSession->wsaBuf_Recv.len = sizeof(float);
            pSession->wsaBuf_Recv.buf = pSession->recvBuffer;

            DWORD recvLen = 0;
            DWORD flag = 0;

            if (WSARecv((pSession)->soc, &pSession->wsaBuf_Recv, 1, &recvLen, &flag, &(pSession)->Overlapped_Recv, NULL) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSA_IO_PENDING)
                {
                    closesocket(pSession->soc);
                }
            }
#pragma endregion Recv
        }
    }
}

