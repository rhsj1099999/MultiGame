#include "stdafx.h"
#include "CServerManager.h"
#include "SceneMgr.h"
#include "CaseHoles.h"
#include "KeyMgr.h"
#include "MainGame.h"
#include "PirateHead.h"
#include <fstream>

CServerManager* CServerManager::m_pInstance = nullptr;

static bool SceneChanged = false;

void CServerManager::ClearDatas()
{
    m_iCurrUser = 1; //At Least Me
    m_bCanMove = false;
    m_fCurrAngle = 0.0;
    m_fPrevAngle = 0.0;
    m_iCurrentPlayer = -1;
    m_bIsChatReady = false;

    m_Chattings.clear();
    DeleteCriticalSection(&m_ChatQueueCS);

    m_bGameIsEnd = false;
    m_bIMWIN = false;
    m_iWhoWins = -1;
    m_dwConnectTrying = {};

    if (m_pSession != nullptr)
    {
        delete m_pSession;
        m_pSession = nullptr;
    }

    m_pPirateHeadPtr = nullptr;
    m_vecCaseHoles = nullptr;
    m_dwHeartBitCounter = {};
}

void CServerManager::WorkerEntry_D(HANDLE hHandle, char* pOut, int size)
{
    while (true)
    {
        DWORD Bytes = 0;
        ClientSession* pSession = nullptr;
        LPOVERLAPPED pOverlap = nullptr;

        bool bRet = GetQueuedCompletionStatus(hHandle, &Bytes, (ULONG_PTR*)&pSession, (LPOVERLAPPED*)&pOverlap, INFINITE);

        if (bRet == FALSE || Bytes == 0)
        {
            ServerDamaged();
            return;
        }

        if (pOverlap == &pSession->Overlapped_Recv)
        {
#pragma region Recv
            pSession->ByteTransferred += Bytes;
            pSession->ByteToRead -= Bytes;


            if (pSession->ByteToRead < 0)
            {
                ServerDamaged();
                return;
            }
                

            while (true)
            {
                if (pSession->bHeaderTransferred == false)
                {
                    if (pSession->ByteTransferred < sizeof(PREDATA))
                    {
                        pSession->wsaBuf_Recv.buf = &pSession->recvBuffer[pSession->ByteTransferred];
                        pSession->wsaBuf_Recv.len = pSession->ByteToRead;
                        break;
                    }
                    else
                    {
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
                    if (pSession->ByteTransferred < pSession->ByteToRead)
                    {
                        pSession->wsaBuf_Recv.buf = &pSession->recvBuffer[pSession->ByteTransferred];
                        pSession->wsaBuf_Recv.len = pSession->ByteToRead;
                        break;
                    }
                    else
                    {
                        ExecuetionMessage
                        (
                            pSession->pLatestHead.eOrderType,
                            pSession->recvBuffer,
                            pSession->pLatestHead.iSizeStandby
                        );
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
                ServerDamaged();
                return;
            }

            if (WSARecv(pSession->soc, &pSession->wsaBuf_Recv, 1, &recvLen, &flag, &pSession->Overlapped_Recv, NULL) == SOCKET_ERROR)
            {
                int ERR = WSAGetLastError();
                if (ERR != WSAEWOULDBLOCK && ERR != WSA_IO_PENDING)
                {
                    ServerDamaged();
                    return;
                }
            }
#pragma endregion Recv
        }
        else
        {
#pragma region Send
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
                    ServerDamaged();
                    return;
                }
            }
#pragma endregion Send
        }
    }
}


CServerManager::CServerManager()
{
}

CServerManager::~CServerManager()
{
    //Release();
}

int CServerManager::Update()
{
    if (m_bClientConnected == true && m_Socket == INVALID_SOCKET)
    {
        CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP, false);
        SR1_MSGBOX("Server Closed");
        Release();
        return 0;
    }

    ConnectTry();

    SendHeartBeat();

    ChattingUpdate();
 
	return 0;
}

void CServerManager::Late_Update()
{
}

void CServerManager::Release()
{
    ClearDatas();

    m_bClientConnected = false;

    shutdown(m_Socket, SD_BOTH);

    closesocket(m_Socket);

    CloseHandle(m_IOCPHandle);

    m_IOCPHandle = INVALID_HANDLE_VALUE;

    for (vector<thread>::iterator itr = m_vecWorkerThreads.begin(); itr != m_vecWorkerThreads.end(); ++itr)
    {
        if (itr->joinable())
        {
            itr->join();
        }
    }

    WSACleanup();

    if (m_pSession != nullptr)
    {
        delete m_pSession;
    }
}

void CServerManager::InitServer()
{
	if (m_bClientConnected == true)
		return;

    InitializeCriticalSection(&m_ChatQueueCS);

	if (WSAStartup(MAKEWORD(2, 2), &m_wsa))
	{
		//Break;
	}

	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	unsigned long On = 1;
	ioctlsocket(m_Socket, FIONBIO, &On);
	bool bEnable = true;
	//setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bEnable, sizeof(bEnable));
	setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&bEnable, sizeof(bEnable));

	if (m_Socket == INVALID_SOCKET)
	{
		//Break;
	}

    std::string projectDirectory = "../OutPut/IPAddress.txt";

    //exe -> ../../IPAddress.txt
    //VC -> ../IPAddress.txt

    bool bFileRead = false;

    ifstream file(projectDirectory.c_str()); //VC Try

    if (file.is_open())
    {
        bFileRead = true;
    }
    if (bFileRead == false) //EXE Try
    {
        file.close();
        projectDirectory = "../../IPAddress.txt";
        file = ifstream(projectDirectory.c_str());

        if (file.is_open())
        {
            bFileRead = true;
        }
    }

    if (bFileRead == false)
    {
        SR1_MSGBOX("Failed to open the file");
        return;
    }


    string line;
    if (!(getline(file, line)))
    {
        SR1_MSGBOX("Failed to Read");
        return;
    }

	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
	m_ServerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, line.c_str(), &m_ServerAddr.sin_addr);
	m_ServerAddr.sin_port = htons(7777);
    file.close();

    if (g_hWndEdit == NULL)
    {
        g_hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            50, 50, 200, 30, g_hWnd, NULL, g_hInstance, NULL);

        ShowWindow(g_hWndEdit, SW_HIDE);
    }
}

bool CServerManager::ChattingLengthCheck()
{
    return true;
}

void CServerManager::ChattingShowOn()
{
    if (g_hWndEdit != NULL)
    {
        ShowWindow(g_hWndEdit, SW_SHOW);
    }
}

void CServerManager::ChattingUpdate()
{
    if (m_bClientConnected == false)
        return;

    if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN) && GetForegroundWindow() == g_hWnd)
    {
        if (m_bIsChatReady == false)
        {
            m_bIsChatReady = true;
            ShowWindow(g_hWndEdit, SW_SHOW);
            SetFocus(g_hWndEdit);
        }
        else
        {
            wchar_t Buffer[MAX_PATH] = {};
            GetWindowText(g_hWndEdit, Buffer, MAXCHATLEN + 1);
            char Buffer_Char[MAXCHATLEN_TOC] = {};

            int Debug_WideLen = lstrlenW(Buffer);
            int Byte = (Debug_WideLen * 2) + NULLSIZE;
            
            bool bIsOnlyBlank = true;
            for (int i = 0; i < Byte; i++)
            {
                if (Buffer_Char[i] != ' ')
                {
                    bIsOnlyBlank = false;
                    break;
                }
            }

            if (Buffer[0] != '\0' && bIsOnlyBlank == false && m_bClientConnected == true)
            {
                char TempPacketPtr[MAX_PATH] = {};
                MSGType TempMsgType = MSGType::User;
                int Debug = sizeof(TempMsgType);
                memcpy(TempPacketPtr, &TempMsgType, sizeof(TempMsgType));
                Debug = sizeof(m_tRoomDesc);
                memcpy(&TempPacketPtr[sizeof(TempMsgType)], &m_tRoomDesc, sizeof(m_tRoomDesc));
                memcpy(&TempPacketPtr[sizeof(TempMsgType) + sizeof(m_tRoomDesc)], &Buffer, Byte);
                MySend_Ptr(m_pSession, TempPacketPtr, Byte + sizeof(TempMsgType) + sizeof(m_tRoomDesc), PREDATA::OrderType::CLIENTCHATSHOOT);
            }

            SetWindowText(g_hWndEdit, L"");
            SendMessage(g_hWndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
            GetWindowText(g_hWndEdit, Buffer, MAXCHATLEN + 1);

            m_bIsChatReady = false;
            ShowWindow(g_hWndEdit, SW_HIDE);
            SetFocus(g_hWnd);
        }
    }

    if (m_bIsChatReady != true)
        return;

    int len = GetWindowTextLength(g_hWndEdit);
    if (len > MAXCHATLEN)
    {
        wchar_t Buffer[MAX_PATH] = {};
        GetWindowText(g_hWndEdit, Buffer, MAXCHATLEN + 1);
        SetWindowText(g_hWndEdit, Buffer);
        SendMessage(g_hWndEdit, EM_SETSEL, (WPARAM)MAXCHATLEN, (LPARAM)MAXCHATLEN);
    }
}

void CServerManager::ShowChattings(HDC hDC)
{
    if (m_Chattings.size() == 0)
        return;

    DWORD CurrTime = CMainGame::Get_Instance()->GetCurrTime();

    int Index = 0;

    EnterCriticalSection_Chat();
    for (deque<ChattingMessageDesc>::iterator Itr = m_Chattings.begin(); Itr != m_Chattings.end(); ++Itr)
    {
        if ((CurrTime - (*Itr).SendedTime) > MAXCHATTINGSHOW)
        {
            Itr = m_Chattings.erase(Itr);

            if (Itr == m_Chattings.end())
                break;
        }
        ++Index;

        switch ((*Itr).eType)
        {
        case MSGType::Sys:
            SetTextColor(hDC, RGB(255, 0, 0));
            break;
        case MSGType::User:
            SetTextColor(hDC, RGB(0, 0, 0));
            break;
        default:
            break;
        }

        TextOut(hDC, 15, (WINCY - 15) - (MESSAGEYDIFF * Index), (*Itr).Message.c_str(), (*Itr).Message.size());
    }
    LeaveCriticalSection_Chat();

    SetTextColor(hDC, RGB(0, 0, 0));
}

void CServerManager::Render(HDC hDC)
{
    if (m_bClientConnected == true)
    {
        ShowChattings(hDC);
    }
}

void CServerManager::SendHeartBeat()
{
    if (m_bClientConnected == false)
        return;

    m_dwHeartBitCounter += CMainGame::Get_Instance()->GetDeltaTime();

    if (m_dwHeartBitCounter >= SENDHEARTBEATCYCLE)
    {
        bool bTempIMALIVE = true;
        MySend<bool>(m_pSession, bTempIMALIVE, PREDATA::OrderType::HEARTBEAT);
    }
}

void CServerManager::ConnectTry()
{
    if (m_bClientConnected == true)
        return;

    if (connect(m_Socket, (SOCKADDR*)&m_ServerAddr, sizeof(m_ServerAddr)) == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
        {
            return;
        }

        if (WSAGetLastError() == WSAEISCONN)
        {
            m_bClientConnected = true;
            if (m_IOCPHandle == m_IOCPHandle)
            {
                m_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
            }
            m_bClientConnected = true;

            for (int i = 0; i < 10; i++)
            {
                m_vecWorkerThreads.push_back(thread([=]()
                    {
                        WorkerEntry_D(m_IOCPHandle, nullptr);
                    }));
            }


            ClientSession* pSession = new ClientSession;
            m_pSession = pSession;
            pSession->soc = m_Socket;
            pSession->Overlapped_Send = {};
            pSession->Overlapped_Recv = {};
            pSession->eType = READ;


            pSession->ByteTransferred = 0;
            pSession->ByteToRead = sizeof(PREDATA);
            pSession->wsaBuf_Recv.buf = pSession->recvBuffer;
            pSession->wsaBuf_Recv.len = sizeof(PREDATA);
            CreateIoCompletionPort((HANDLE)m_Socket, m_IOCPHandle, /*Key*/(ULONG_PTR)pSession, 0); //등록할때는

            pSession->CQPtr = new CMyCQ(BUF384);

            DWORD recvLen = 0;
            DWORD flag = 0;


            if (WSARecv(pSession->soc, &pSession->wsaBuf_Recv, 1, &recvLen, &flag, &pSession->Overlapped_Recv, NULL) == SOCKET_ERROR)
            {
                int ERR = WSAGetLastError();
                if (ERR != WSAEWOULDBLOCK && ERR != WSA_IO_PENDING)
                {
                    MSGBOX("WOD Client RECV / Error_MySend. Not EWB, PENDING");
                }
            }
            return;
        }
    }
}

void CServerManager::ServerDamaged()
{
    closesocket(m_Socket);
    m_Socket = INVALID_SOCKET;
}

PlayingRoomSessionDesc* CServerManager::GetRoomDescPtr()
{
    if (m_tRoomDesc.MyNumber == -1)
        return nullptr;

    return &m_tRoomDesc;

}

bool CServerManager::ExecuetionMessage(PREDATA::OrderType eType, void* Data, int DataSize)
{
    bool Ret = false;

	switch (eType)
	{
	case PREDATA::OrderType::USERCOUNT:
		memcpy(&m_iCurrUser, (int*)Data, sizeof(int));
		break;
    case PREDATA::OrderType::MESSAGECHANGE:
        memcpy(&m_pSession->wsaBuf_Recv.len, (int*)Data, DataSize);
        break;

    case PREDATA::OrderType::SCENECHANGE_TOPLAY:
    {
        CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE4, true);
        PlayingRoomSessionDesc* pCast = static_cast<PlayingRoomSessionDesc*>(Data);
        CServerManager::Get_Instance()->SetRoomDesc(pCast);

        m_HoleVector.clear();
        m_HoleVector.resize(HOLE_HORIZON * HOLE_VERTICAL, true);
    }

        break;

    case PREDATA::OrderType::SCENECHANGE_TOWORLD:
        CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);
        break;

    case PREDATA::OrderType::TURNOFF:
        m_bCanMove = false;
        break;

    case PREDATA::OrderType::TURNON:
        m_bCanMove = true;
        break;

    case PREDATA::OrderType::TURNCHANGED:
    {
        memcpy(&m_iCurrentPlayer, (int*)Data, sizeof(int));
        m_bCanMove = (m_tRoomDesc.MyNumber == m_iCurrentPlayer) ? true : false;
    }

        break;

	case PREDATA::OrderType::END:
		memcpy(&m_iCurrUser, (int*)Data, DataSize);
		break;

    case PREDATA::OrderType::FOLLOWANGLE:
        memcpy(&m_fCurrAngle, (float*)Data, sizeof(float));
        break;

    case PREDATA::OrderType::FOLLOWINDEX:
    {
        PAK_INSERTFOLLOW TempData = {};

        memcpy(&TempData, (int*)Data, sizeof(PAK_INSERTFOLLOW));
        m_HoleVector[TempData.HoldIndex] = false;
        (*m_vecCaseHoles)[TempData.HoldIndex]->SetIsInserted(true, TempData.PlayerIndex);
    }

        break;
    case PREDATA::OrderType::HEARTBEAT:
        break;
    case PREDATA::OrderType::CLIENTCHATSHOOT:
        break;
    case PREDATA::OrderType::SERVERCHATSHOOT:
    {
        char* Casted = static_cast<char*>(Data);

        MSGType TempType = {};
        int UserIndex = -1;
        wchar_t TempWCharBuffer[MAXCHATLEN_TOC + NULLSIZE] = {};

        memcpy(&TempType, Casted, sizeof(TempType));
        memcpy(&UserIndex, &Casted[sizeof(TempType)], sizeof(UserIndex));
        memcpy(&TempWCharBuffer, &Casted[sizeof(TempType) + sizeof(TempType)], DataSize - (sizeof(TempType) + sizeof(TempType)));

        wchar_t CompleteString[CMPCHAT] = {};

        switch (TempType)
        {
        case MSGType::Sys:
            wsprintf(CompleteString, L"System : %s", TempWCharBuffer);
            break;
        case MSGType::User:
            wsprintf(CompleteString, L"Player [%d] : %s", UserIndex, TempWCharBuffer);
            break;
        case MSGType::END:
            break;
        default:
            break;
        }
        EnterCriticalSection_Chat();
        m_Chattings.push_front(ChattingMessageDesc
        (
            TempType,
            CompleteString,
            CMainGame::Get_Instance()->GetCurrTime()
        ));
        LeaveCriticalSection_Chat();
    }
        break;

    case PREDATA::OrderType::GAMEEND:
    {
        m_bGameIsEnd = true;

        if (m_pPirateHeadPtr != nullptr)
        {
            m_pPirateHeadPtr->SetForcedGoUp(true);
        }

        char* Casted = static_cast<char*>(Data);

        int Inserted = 0;

        memcpy(&Inserted, Casted, sizeof(int));
        memcpy(&m_iWhoWins, &Casted[sizeof(int)], sizeof(int));

        m_HoleVector[Inserted] = false;
        (*m_vecCaseHoles)[Inserted]->SetIsInserted(true, m_iWhoWins);

        m_bCanMove = false;

        if (m_tRoomDesc.MyNumber == m_iWhoWins)
        {
            //Effect
            m_bIMWIN = true;
        }

        wchar_t TempVictoryMessage[MAX_PATH] = {};
        wsprintf(TempVictoryMessage, L"System : Player [ %d ] Win.", m_iWhoWins);
        EnterCriticalSection_Chat();
        m_Chattings.push_front(ChattingMessageDesc
        (
            MSGType::Sys,
            TempVictoryMessage,
            CMainGame::Get_Instance()->GetCurrTime()
        ));
        LeaveCriticalSection_Chat();

    }
        break;
    case PREDATA::OrderType::FORCEDGAMEEND:
    {
        m_bGameIsEnd = true;
        memcpy(&m_iWhoWins, Data, sizeof(int));
        m_bCanMove = false;
        if (m_tRoomDesc.MyNumber == m_iWhoWins)
        {
            //Effect
            m_bIMWIN = true;
        }

        wchar_t TempVictoryMessage[MAX_PATH] = {};
        wsprintf(TempVictoryMessage, L"System : Player [ %d ] Win.", m_iWhoWins);
        EnterCriticalSection_Chat();
        m_Chattings.push_front(ChattingMessageDesc
        (
            MSGType::Sys,
            TempVictoryMessage,
            CMainGame::Get_Instance()->GetCurrTime()
        ));
        LeaveCriticalSection_Chat();
    }
        break;

	default:
    {
        SR1_MSGBOX("Wrong Message");
        Ret = true;
    }
		break;
	}
    return Ret;
}
