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

        if (pOverlap == &pSession->_overlapped_Recv)
        {
#pragma region Recv
            pSession->_byteTransferred += Bytes;
            pSession->_byteToRead -= Bytes;


            if (pSession->_byteToRead < 0)
            {
                ServerDamaged();
                return;
            }
                

            while (true)
            {
                if (pSession->_isHeaderTransferred == false)
                {
                    if (pSession->_byteTransferred < sizeof(PacketHeader))
                    {
                        pSession->_wsaBuffer_Recv.buf = &pSession->_buffer[pSession->_byteTransferred];
                        pSession->_wsaBuffer_Recv.len = pSession->_byteToRead;
                        break;
                    }
                    else
                    {
                        pSession->_isHeaderTransferred = true;
                        pSession->_latestSendedHeader = *((PacketHeader*)pSession->_buffer);
                        pSession->_byteTransferred = 0;
                        pSession->_byteToRead = pSession->_latestSendedHeader._packetSize;
                        pSession->_wsaBuffer_Recv.buf = pSession->_buffer;
                        pSession->_wsaBuffer_Recv.len = pSession->_latestSendedHeader._packetSize;
                        break;
                    }
                }
                else
                {
                    if (pSession->_byteTransferred < pSession->_byteToRead)
                    {
                        pSession->_wsaBuffer_Recv.buf = &pSession->_buffer[pSession->_byteTransferred];
                        pSession->_wsaBuffer_Recv.len = pSession->_byteToRead;
                        break;
                    }
                    else
                    {
                        ExecuetionMessage
                        (
                            pSession->_latestSendedHeader._packetType,
                            pSession->_buffer,
                            pSession->_latestSendedHeader._packetSize
                        );

                        pSession->_isHeaderTransferred = false;
                        pSession->_byteTransferred = 0;
                        pSession->_byteToRead = sizeof(PacketHeader);
                        pSession->_wsaBuffer_Recv.buf = pSession->_buffer;
                        pSession->_wsaBuffer_Recv.len = sizeof(PacketHeader);
                        break;
                    }
                }
            }
            DWORD recvLen = 0;
            DWORD flag = 0;

            if (pSession->_byteToRead <= 0)
            {
                ServerDamaged();
                return;
            }

            if (WSARecv(pSession->_socket, &pSession->_wsaBuffer_Recv, 1, &recvLen, &flag, &pSession->_overlapped_Recv, NULL) == SOCKET_ERROR)
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
            LockGuard Temp(pSession->_circularQueue->GetMutex());

            pSession->_circularQueue->Dequq_N(Bytes);

            if (pSession->_circularQueue->GetSize() == 0)
                continue; //메세지 없으면 자러갈꺼임

            pSession->_wsaBuffer_Send.len = pSession->_circularQueue->GetSize();
            pSession->_wsaBuffer_Send.buf = (char*)pSession->_circularQueue->GetFrontPtr();

            DWORD recvLen = 0;
            DWORD flag = 0;

            if (WSASend((pSession)->_socket, &pSession->_wsaBuffer_Send, 1, &recvLen, flag, &(pSession)->_overlapped_Send, NULL) == SOCKET_ERROR)
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

void CServerManager::SetCanChat(bool bCanChat) 
{ 
    m_bCanChat = bCanChat;

    m_Chattings.clear();

    if (g_hWndEdit != INVALID_HANDLE_VALUE)
    {
        SetWindowText(g_hWndEdit, L"");
        SendMessage(g_hWndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);

        m_bIsChatReady = false;
        ShowWindow(g_hWndEdit, SW_HIDE);
        SetFocus(g_hWnd);
    }
}

void CServerManager::ChattingUpdate()
{

    if (m_bClientConnected == false || m_bCanChat == false)
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
                PAK_ChattingMessageToRoom chattingPacketToRoom = PAK_ChattingMessageToRoom
                (
                    m_tRoomDesc._myNumber,
                    m_tRoomDesc._myRoomPtr,
                    PAK_ChattingMessage::MSGType::User,
                    0,
                    Buffer
                );

                MySend<PAK_ChattingMessageToRoom>(m_pSession, chattingPacketToRoom, PacketHeader::PacketType::CLIENTCHATSHOOT);
            }

            SetWindowText(g_hWndEdit, L"");
            SendMessage(g_hWndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);

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
    if (m_Chattings.size() == 0 || m_bClientConnected == false || m_bCanChat == false)
        return;

    DWORD CurrTime = CMainGame::Get_Instance()->GetCurrTime();

    int Index = 0;

    EnterCriticalSection_Chat();
    for (deque<PAK_ChattingMessage>::iterator Itr = m_Chattings.begin(); Itr != m_Chattings.end(); ++Itr)
    {
        if ((CurrTime - (*Itr)._time) > MAXCHATTINGSHOW)
        {
            Itr = m_Chattings.erase(Itr);

            if (Itr == m_Chattings.end())
                break;
        }
        ++Index;

        switch ((*Itr)._messageType)
        {
        case PAK_ChattingMessage::MSGType::Sys:
            SetTextColor(hDC, RGB(255, 0, 0));
            break;
        case PAK_ChattingMessage::MSGType::User:
            SetTextColor(hDC, RGB(0, 0, 0));
            break;
        default:
            break;
        }

        TextOut(hDC, 15, (WINCY - 15) - (MESSAGEYDIFF * Index), (*Itr)._message.c_str(), (*Itr)._message.length());
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
        PAK_HeartBeat packetHeartBeat = PAK_HeartBeat();
        MySend<PAK_HeartBeat>(m_pSession, packetHeartBeat, PacketHeader::PacketType::HEARTBEAT);
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
            pSession->_socket = m_Socket;
            pSession->_overlapped_Send = {};
            pSession->_overlapped_Recv = {};
            pSession->_ioType = ClientSession::IOType::Read;


            pSession->_byteTransferred = 0;
            pSession->_byteToRead = sizeof(PacketHeader);
            pSession->_wsaBuffer_Recv.buf = pSession->_buffer;
            pSession->_wsaBuffer_Recv.len = sizeof(PacketHeader);
            CreateIoCompletionPort((HANDLE)m_Socket, m_IOCPHandle, /*Key*/(ULONG_PTR)pSession, 0); //등록할때는

            pSession->_circularQueue = new CircularQueue(BUF384);

            DWORD recvLen = 0;
            DWORD flag = 0;


            if (WSARecv(pSession->_socket, &pSession->_wsaBuffer_Recv, 1, &recvLen, &flag, &pSession->_overlapped_Recv, NULL) == SOCKET_ERROR)
            {
                int ERR = WSAGetLastError();
                if (ERR != WSAEWOULDBLOCK && ERR != WSA_IO_PENDING)
                {
                    SR1_MSGBOX("WOD Client RECV / Error_MySend. Not EWB, PENDING");
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
    if (m_tRoomDesc._myNumber == -1)
        return nullptr;

    return &m_tRoomDesc;

}

bool CServerManager::ExecuetionMessage(PacketHeader::PacketType packetType, void* pPacketData, int DataSize)
{
    bool Ret = false;

	switch (packetType)
	{
	case PacketHeader::PacketType::USERCOUNT:
    {
        PAK_UserCount* pCasted = static_cast<PAK_UserCount*>(pPacketData);
        m_iCurrUser = pCasted->_usercount;
    }
		break;
    case PacketHeader::PacketType::SCENECHANGE_TOPLAY:
    {
        PAK_SceneChange* pCasted = static_cast<PAK_SceneChange*>(pPacketData);

        CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE4, true);

        PlayingRoomSessionDesc roomDesc;
        roomDesc._myNumber = pCasted->_myNumber;
        roomDesc._myRoomPtr = pCasted->_myRoomPtr;

        CServerManager::Get_Instance()->SetRoomDesc(roomDesc);

        m_HoleVector.clear();
        m_HoleVector.resize(HOLE_HORIZON * HOLE_VERTICAL, true);
        SetCanChat(true);
    }
        break;

    case PacketHeader::PacketType::TURNCHANGED:
    {
        PAK_TurnChanged* pCasted = static_cast<PAK_TurnChanged*>(pPacketData);
        m_iCurrentPlayer = pCasted->_nextPlayerIndex;
        m_bCanMove = (m_tRoomDesc._myNumber == m_iCurrentPlayer) ? true : false;
    }

        break;

    case PacketHeader::PacketType::FOLLOWANGLE:
    {
        PAK_ROTATEANGLE_Follow* pCasted = static_cast<PAK_ROTATEANGLE_Follow*>(pPacketData);
        m_fCurrAngle = pCasted->_angle;
    }
        break;

    case PacketHeader::PacketType::FOLLOWINDEX:
    {
        PAK_INSERTFOLLOW* pCasted = static_cast<PAK_INSERTFOLLOW*>(pPacketData);

        m_HoleVector[pCasted->_insertedIndex] = false;
        (*m_vecCaseHoles)[pCasted->_insertedIndex]->SetIsInserted(true, pCasted->_inertPlayerIndex);
    }
        break;
    case PacketHeader::PacketType::SERVERCHATSHOOT:
    {
        PAK_ChattingToSingle* pCasted = static_cast<PAK_ChattingToSingle*>(pPacketData);

        PAK_ChattingMessage::MSGType TempType = pCasted->_messageType;

        wchar_t CompleteString[CMPCHAT];

        switch (TempType)
        {
        case PAK_ChattingMessage::MSGType::Sys:
            wsprintf(CompleteString, L"%s", pCasted->_message);
            break;
        case PAK_ChattingMessage::MSGType::User:
            wsprintf(CompleteString, L"%s", pCasted->_message);
            break;
        }

        EnterCriticalSection_Chat();
        m_Chattings.push_front(PAK_ChattingMessage
        (
            TempType,
            CMainGame::Get_Instance()->GetCurrTime(),
            CompleteString
        ));
        LeaveCriticalSection_Chat();
    }
        break;

    case PacketHeader::PacketType::GAMEEND:
    {
        PAK_GameEnd* pCasted = static_cast<PAK_GameEnd*>(pPacketData);

        m_bGameIsEnd = true;

        if (m_pPirateHeadPtr != nullptr)
        {
            m_pPirateHeadPtr->SetForcedGoUp(true);
        }

        m_iWhoWins = pCasted->_winPlayer;
        m_bCanMove = false;
        if (m_tRoomDesc._myNumber == m_iWhoWins)
        {
            m_bIMWIN = true;
        }

        wchar_t TempVictoryMessage[MAX_PATH] = {};
        wsprintf(TempVictoryMessage, L"System : Player [ %d ] Win.", m_iWhoWins);

        EnterCriticalSection_Chat();
        m_Chattings.push_front(PAK_ChattingMessage
        (
            PAK_ChattingMessage::MSGType::Sys,
            CMainGame::Get_Instance()->GetCurrTime(),
            TempVictoryMessage
        ));
        LeaveCriticalSection_Chat();
    }
        break;
    case PacketHeader::PacketType::FORCEDGAMEEND:
    {
        PAK_GameEnd* pCasted = static_cast<PAK_GameEnd*>(pPacketData);

        m_bGameIsEnd = true;
        m_bCanMove = false;
        m_bIMWIN = true;

        wchar_t TempVictoryMessage[MAX_PATH] = {};
        wsprintf(TempVictoryMessage, L"System : Player [ %d ] Win.", m_iWhoWins);

        EnterCriticalSection_Chat();
        m_Chattings.push_front(PAK_ChattingMessage
        (
            PAK_ChattingMessage::MSGType::Sys,
            CMainGame::Get_Instance()->GetCurrTime(),
            TempVictoryMessage
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
