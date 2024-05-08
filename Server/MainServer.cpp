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
    //setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bEnable, sizeof(bEnable));
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

    for (int i = 0; i < 10; i++)
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
    pSession->eClientState = ClientSession::ClientState::WAITING;
    m_liClientSockets.push_back(pSession);
    pSession->CQPtr = new CMyCQ(BUF384);
    m_queWaitingQueue.push(pSession);
    CreateIoCompletionPort((HANDLE)ClientSocket, m_IOCPHandle, /*Key*/(ULONG_PTR)pSession, 0);

    {
        CMyCQ::LockGuard Temp(m_ClassDataLock);
        m_iCurrUser = static_cast<__int32>(m_liClientSockets.size());
    }

    cout << "Client Connected Users : " << m_iCurrUser << endl;

    pSession->ByteToSent = pSession->CQPtr->GetSize();
    pSession->wsaBuf_Send.buf = pSession->CQPtr->GetBuffer();
    pSession->wsaBuf_Send.len = pSession->CQPtr->GetSize();

    pSession->ByteTransferred = 0;
    pSession->ByteToRead = sizeof(PREDATA);
    pSession->wsaBuf_Recv.buf = pSession->recvBuffer;
    pSession->wsaBuf_Recv.len = sizeof(PREDATA);
    pSession->ByteTransferred = 0;

    DWORD recvLen = 0;
    DWORD flag = 0;

    for (ClientSession* pCS : m_liClientSockets)
    {
        if (pCS->PlayingRoomPtr != nullptr)
            continue;

        MySend<int>(pCS, m_iCurrUser, PREDATA::OrderType::USERCOUNT);
    }
    
    WSARecv(pSession->soc, &pSession->wsaBuf_Recv, 1, &recvLen, &flag, &pSession->Overlapped_Recv, NULL);
}

void CMainServer::TickWatingClients()
{

}


void CMainServer::LiveCheck()
{
    DWORD dwCurrTime = CTimer::GetInstance()->GetCurrTime();
    DWORD recvLen = 0;
    DWORD flag = 0;

    bool bTempDead = false;

    for (list<ClientSession*>::iterator Itr = m_liClientSockets.begin(); Itr != m_liClientSockets.end(); ++Itr)
    {
        if ((*Itr)->soc == INVALID_SOCKET)
        {
            bTempDead = true;
        }
        else
        {
            if (abs(static_cast<int>((*Itr)->Respones - dwCurrTime)) > MAXTIMEOUT)
            {
                ++(*Itr)->LateCount;

                if ((*Itr)->LateCount > MAXLATECOUNT)
                    bTempDead = true;
            }
        }

        if (bTempDead)
        {
            CMyCQ::LockGuard Temp(m_ClassDataLock);

            if ((*Itr)->PlayingRoomPtr != nullptr)
                static_cast<CPlayingRoom*>((*Itr)->PlayingRoomPtr)->ClientDead((*Itr));

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

void CMainServer::MatchingRoom()
{
    if (m_queWaitingQueue.size() >= CLIENT3)
    {
        CPlayingRoom* pNewRoom = new CPlayingRoom();

        ClientSession* pArr[CLIENT3] = { nullptr, };

        PlayingRoomSessionDesc Desc
        {
            -1,
            pNewRoom
        };

        for (int i = 0; i < CLIENT3; i++)
        {
            ClientSession* pSession = m_queWaitingQueue.front();

            pArr[i] = pSession;

            pSession->PlayingRoomPtr = pNewRoom;

            Desc.MyNumber = i;

            MySend<PlayingRoomSessionDesc>(pSession, Desc, PREDATA::OrderType::SCENECHANGE_TOPLAY);

            m_queWaitingQueue.pop();
        }

        pNewRoom->Init(pArr, m_liClientSockets);
    }
}

bool CMainServer::ExecuetionMessage(PREDATA::OrderType eType, void* pData, int DataSize)
{
    bool Ret = false;

    switch (eType)
    {
    case PREDATA::OrderType::USERCOUNT:
        break;
    case PREDATA::OrderType::MESSAGECHANGE:
        break;
    case PREDATA::OrderType::SCENECHANGE_TOPLAY:
        break;
    case PREDATA::OrderType::SCENECHANGE_TOWORLD:
        break;
    case PREDATA::OrderType::TURNON:
        break;
    case PREDATA::OrderType::TURNOFF:
        break;
    case PREDATA::OrderType::ROTATEANGLE:
    {
        char* pCharCasted = static_cast<char*>(pData);

        PAK_ROTATEANGLE Data;

        memcpy(&Data, pCharCasted, sizeof(PAK_ROTATEANGLE));

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(Data.RoomSessionDesc.MyRoomPtr);

        pRoom->ExecutionMessage_InRoom(eType, &Data, sizeof(PAK_ROTATEANGLE));

    }
        break;
    case PREDATA::OrderType::PLAYERBLADEINSERTED:
    {
        char* pCharCasted = static_cast<char*>(pData);

        PAK_BLADEINSERT Data;

        memcpy(&Data, pCharCasted, sizeof(PAK_BLADEINSERT));

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(Data.RoomSessionDesc.MyRoomPtr);

        pRoom->ExecutionMessage_InRoom(eType, &Data, sizeof(PAK_BLADEINSERT));
    }
        break;
    case PREDATA::OrderType::HEARTBEAT:
        break;
    case PREDATA::OrderType::CLIENTCHATSHOOT:
    {
        char* pCharCasted = static_cast<char*>(pData);

        MSGType TempType = {};
        PlayingRoomSessionDesc TempRoomDesc = {};
        char PacData[MAX_PATH] = {};
        int CharSize = DataSize - (sizeof(TempType) + sizeof(TempRoomDesc));
        
        memcpy(&TempRoomDesc, &pCharCasted[sizeof(TempType)], sizeof(TempRoomDesc));
        memcpy(PacData, pCharCasted, sizeof(MSGType));
        memcpy(&PacData[sizeof(MSGType)], &TempRoomDesc.MyNumber, sizeof(int));
        memcpy(&PacData[sizeof(TempType) + sizeof(int)], &pCharCasted[(sizeof(TempType) + sizeof(TempRoomDesc))], CharSize);

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(TempRoomDesc.MyRoomPtr);

        pRoom->ExecutionMessage_InRoom(eType, PacData, CharSize + sizeof(int) + sizeof(TempType));
    }
        break;
    case PREDATA::OrderType::SERVERCHATSHOOT:
        break;
    case PREDATA::OrderType::END:
        break;
    default:
        break;
    }

    return Ret;
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

        if (pOverlap == &pSession->Overlapped_Send)
        {
#pragma region Send

            pSession->Respones = CTimer::GetInstance()->GetCurrTime();

            CMyCQ::LockGuard Temp(pSession->CQPtr->GetMutex());

            pSession->CQPtr->Dequq_N(Bytes);

            if (pSession->CQPtr->GetSize() == 0)
                continue; //메세지 없으면 자러갈꺼임

            pSession->wsaBuf_Send.len = pSession->CQPtr->GetSize();
            pSession->wsaBuf_Send.buf = (char*)pSession->CQPtr->GetFrontPtr();

            DWORD recvLen = 0;
            DWORD flag = 0;

            if (WSASend((pSession)->soc, &pSession->wsaBuf_Send, 1, &recvLen, flag, &(pSession)->Overlapped_Send, NULL) == SOCKET_ERROR)
            {
                int ERR = WSAGetLastError();
                if (ERR != WSAEWOULDBLOCK && ERR != WSA_IO_PENDING)
                {
                    MSGBOX("WOD Server Send / Error_MySend. Not EWB, PENDING");
                }
            }
#pragma endregion Send
        }
        else
        {
#pragma region Recv
            pSession->ByteTransferred += Bytes;
            pSession->ByteToRead -= Bytes;

            if (pSession->ByteToRead < 0)
                SR1_MSGBOX("ERROR : ByteToRead Under 0 / Server");

            pSession->Respones = CTimer::GetInstance()->GetCurrTime();

            while (true)
            {
                if (pSession->bHeaderTransferred == false)
                {
                    //헤더가 캐싱된적이 없다
                    if (pSession->ByteTransferred < sizeof(PREDATA))
                    {
                        //근데 헤더를 완성할 수 없다
                        pSession->wsaBuf_Recv.buf = &pSession->recvBuffer[pSession->ByteTransferred];
                        pSession->wsaBuf_Recv.len = pSession->ByteToRead;
                        break;
                    }
                    else
                    {
                        //헤더를 캐싱할 수 있다.
                        pSession->bHeaderTransferred = true;
                        pSession->pLatestHead = *((PREDATA*)pSession->recvBuffer);
                        pSession->ByteTransferred = 0;
                        pSession->ByteToRead = pSession->pLatestHead.iSizeStandby;
                        pSession->wsaBuf_Recv.buf = pSession->recvBuffer;
                        pSession->wsaBuf_Recv.len = pSession->pLatestHead.iSizeStandby;
                        break;
                    }
                }
                else
                {
                    //헤더를 캐싱했다
                    if (pSession->ByteTransferred < pSession->ByteToRead)
                    {
                        //데이터를 다 못읽었다.
                        pSession->wsaBuf_Recv.buf = &pSession->recvBuffer[pSession->ByteTransferred];
                        pSession->wsaBuf_Recv.len = pSession->ByteToRead;
                        break;
                    }
                    else
                    {
                        //데이터를 다 읽었다.
                        if (pSession->pLatestHead.eOrderType == PREDATA::OrderType::HEARTBEAT)
                        {
                            pSession->Respones = CTimer::GetInstance()->GetCurrTime();
                        }
                        else
                        {
                            ExecuetionMessage
                            (
                                pSession->pLatestHead.eOrderType,
                                pSession->recvBuffer,
                                pSession->pLatestHead.iSizeStandby
                            );
                        }

                        pSession->bHeaderTransferred = false;
                        pSession->ByteTransferred = 0;
                        pSession->ByteToRead = sizeof(PREDATA);
                        pSession->wsaBuf_Recv.buf = pSession->recvBuffer;
                        pSession->wsaBuf_Recv.len = sizeof(PREDATA);
                        break;
                    }
                }
            }
            DWORD recvLen = 0;
            DWORD flag = 0;

            if (pSession->ByteToRead <= 0)
            {
                wchar_t Log[32] = {};
                wsprintf(Log, L"ByteToRead Error Server, %d", pSession->ByteToRead);
                MessageBox(0, Log, TEXT("Fail_"), MB_OK);
            }

            if (WSARecv(pSession->soc, &pSession->wsaBuf_Recv, 1, &recvLen, &flag, &pSession->Overlapped_Recv, NULL) == SOCKET_ERROR)
            {
                int ERR = WSAGetLastError();
                if (ERR != WSAEWOULDBLOCK && ERR != WSA_IO_PENDING)
                {
                    MSGBOX("WOD Server RECV / Error_MySend. Not EWB, PENDING");
                }
            }
#pragma endregion Recv
        }
    }
}

