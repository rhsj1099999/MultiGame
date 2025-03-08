#include "stdafx.h"
#include "MainServer.h"

#include "Timer.h"
#include "PlayingRoom.h"
#include <fstream>
#include <string>


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

    for (vector<thread>::iterator Itr = m_vecWorkerThreads.begin(); Itr != m_vecWorkerThreads.end(); ++Itr)
    {
        Itr->join();
    }


    closesocket(m_Socket);

    WSACleanup();

    for (list<CPlayingRoom*>::iterator Itr = m_liPlayingRooms.begin(); Itr != m_liPlayingRooms.end(); ++Itr)
    {
        delete (*Itr);
    }
    m_liPlayingRooms.clear();


    {
        LockGuard Temp(m_ListLock);

        for (ClientSession* pCS : m_liClientSockets)
        {
            closesocket(pCS->_socket);

            if (pCS != nullptr)
            {
                delete pCS;
                pCS = nullptr;
            }
        }
        m_liClientSockets.clear();
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
    
    SOCKADDR_IN m_ServerAddr;
    ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
    m_ServerAddr.sin_family = AF_INET;
    inet_pton(AF_INET, line.c_str(), &m_ServerAddr.sin_addr);
    m_ServerAddr.sin_port = htons(7777);
    file.close();


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

    char serverIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &m_ServerAddr.sin_addr, serverIP, INET_ADDRSTRLEN);
    std::cout << "Server IP: " << serverIP << std::endl;

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

void CMainServer::DeleteRoom(CPlayingRoom* RoomPtr)
{
    for (list<CPlayingRoom*>::iterator Itr = m_liPlayingRooms.begin(); Itr != m_liPlayingRooms.end(); ++Itr)
    {
        if ((*Itr) == RoomPtr)
        {
            delete (*Itr);

            Itr = m_liPlayingRooms.erase(Itr);

            if (Itr == m_liPlayingRooms.end())
                break;
        }
    }
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
    pSession->_socket = ClientSocket;
    pSession->_overlapped_Send = {};
    pSession->_overlapped_Recv = {};
    pSession->_ioType = ClientSession::IOType::Read;
    pSession->_responedTime = CTimer::GetInstance()->GetCurrTime();
    pSession->_lateCount = 0;
    pSession->_clientState = ClientSession::ClientState::WAITING;
    pSession->_circularQueue = new CircularQueue(BUF384);

    CreateIoCompletionPort((HANDLE)ClientSocket, m_IOCPHandle, /*Key*/(ULONG_PTR)pSession, 0);

    {
        LockGuard TempListLock(m_ListLock);
        m_liClientSockets.push_back(pSession);
        m_iCurrUser = static_cast<__int32>(m_liClientSockets.size());

        LockGuard TempQueueLock(m_WatiingLock);
        m_liWatingClients.push_back(pSession);
    }

    cout << "Client Connected Users : " << m_iCurrUser << endl;

    pSession->_byteToSend = pSession->_circularQueue->GetSize();
    pSession->_wsaBuffer_Send.buf = pSession->_circularQueue->GetBuffer();
    pSession->_wsaBuffer_Send.len = pSession->_circularQueue->GetSize();

    pSession->_byteTransferred = 0;
    pSession->_byteToRead = sizeof(PacketHeader);
    pSession->_wsaBuffer_Recv.buf = pSession->_buffer;
    pSession->_wsaBuffer_Recv.len = sizeof(PacketHeader);
    pSession->_byteTransferred = 0;

    DWORD recvLen = 0;
    DWORD flag = 0;

    for (ClientSession* pCS : m_liClientSockets)
    {
        if (pCS->PlayingRoomPtr != nullptr)
            continue;

        MySend<int>(pCS, m_iCurrUser, PacketHeader::PacketType::USERCOUNT);
    }
    
    WSARecv(pSession->_socket, &pSession->_wsaBuffer_Recv, 1, &recvLen, &flag, &pSession->_overlapped_Recv, NULL);
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
        bTempDead = false;

        if ((*Itr)->_socket == INVALID_SOCKET)
        {
            bTempDead = true;
        }
        else
        {
            if (abs(static_cast<int>((*Itr)->_responedTime - dwCurrTime)) > MAXTIMEOUT)
            {
                ++(*Itr)->_lateCount;

                if ((*Itr)->_lateCount > MAXLATECOUNT)
                    bTempDead = true;
            }
        }

        if (bTempDead)
        {
            {
                LockGuard TempWating(m_WatiingLock);
                for (list<ClientSession*>::iterator Itr_W = m_liWatingClients.begin(); Itr_W != m_liWatingClients.end(); ++Itr_W)
                {
                    Itr_W = m_liWatingClients.erase(Itr_W);

                    if (Itr_W == m_liWatingClients.end())
                        break;
                }
            }


            LockGuard Temp(m_ListLock);

            if ((*Itr)->PlayingRoomPtr != nullptr)
                static_cast<CPlayingRoom*>((*Itr)->PlayingRoomPtr)->ClientDead((*Itr));

            closesocket((*Itr)->_socket);

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
    if (m_liWatingClients.size() >= MAXCLIENTS)
    {
        LockGuard Temp(m_WatiingLock);

        CPlayingRoom* pNewRoom = new CPlayingRoom();

        ClientSession* pArr[MAXCLIENTS] = { nullptr, };

        PlayingRoomSessionDesc Desc
        {
            -1,
            pNewRoom
        };

        for (int i = 0; i < MAXCLIENTS; i++)
        {
            ClientSession* pSession = m_liWatingClients.front();

            pArr[i] = pSession;

            pSession->PlayingRoomPtr = pNewRoom;

            Desc.MyNumber = i;

            MySend<PlayingRoomSessionDesc>(pSession, Desc, PacketHeader::PacketType::SCENECHANGE_TOPLAY);


            m_liWatingClients.erase(m_liWatingClients.begin());
        }

        pNewRoom->Init(pArr, m_liClientSockets);
        m_liPlayingRooms.push_back(pNewRoom);
    }
}

bool CMainServer::ExecuetionMessage(PacketHeader::PacketType eType, void* pData, int DataSize)
{
    bool Ret = false;

    switch (eType)
    {
    case PacketHeader::PacketType::USERCOUNT:
        break;
    case PacketHeader::PacketType::MESSAGECHANGE:
        break;
    case PacketHeader::PacketType::SCENECHANGE_TOPLAY:
        break;
    case PacketHeader::PacketType::SCENECHANGE_TOWORLD:
        break;
    case PacketHeader::PacketType::TURNON:
        break;
    case PacketHeader::PacketType::TURNOFF:
        break;
    case PacketHeader::PacketType::ROTATEANGLE:
    {
        char* pCharCasted = static_cast<char*>(pData);

        PAK_ROTATEANGLE Data;

        memcpy(&Data, pCharCasted, sizeof(PAK_ROTATEANGLE));

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(Data.RoomSessionDesc.MyRoomPtr);

        pRoom->ExecutionMessage_InRoom(eType, &Data, sizeof(PAK_ROTATEANGLE));

    }
        break;
    case PacketHeader::PacketType::PLAYERBLADEINSERTED:
    {
        char* pCharCasted = static_cast<char*>(pData);

        PAK_BLADEINSERT Data;

        memcpy(&Data, pCharCasted, sizeof(PAK_BLADEINSERT));

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(Data.RoomSessionDesc.MyRoomPtr);

        pRoom->ExecutionMessage_InRoom(eType, &Data, sizeof(PAK_BLADEINSERT));
    }
        break;
    case PacketHeader::PacketType::HEARTBEAT:
        break;
    case PacketHeader::PacketType::CLIENTCHATSHOOT:
    {
        char* pCharCasted = static_cast<char*>(pData);

        PAK_ChattingMessage::MSGType TempType = {};
        PlayingRoomSessionDesc TempRoomDesc = {};
        char PacData[MAX_PATH] = {};
        int CharSize = DataSize - (sizeof(TempType) + sizeof(TempRoomDesc));
        
        memcpy(&TempRoomDesc, &pCharCasted[sizeof(TempType)], sizeof(TempRoomDesc));
        memcpy(PacData, pCharCasted, sizeof(PAK_ChattingMessage::MSGType));
        memcpy(&PacData[sizeof(PAK_ChattingMessage::MSGType)], &TempRoomDesc.MyNumber, sizeof(int));
        memcpy(&PacData[sizeof(TempType) + sizeof(int)], &pCharCasted[(sizeof(TempType) + sizeof(TempRoomDesc))], CharSize);

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(TempRoomDesc.MyRoomPtr);

        pRoom->ExecutionMessage_InRoom(eType, PacData, CharSize + sizeof(int) + sizeof(TempType));
    }
        break;
    case PacketHeader::PacketType::SERVERCHATSHOOT:
        break;
    case PacketHeader::PacketType::END:
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
    m_liWatingClients.push_back((*Casted));
}

void CMainServer::Lock_Queue_ChangingRoom(void* Ptr)
{
    if (m_liWatingClients.size() >= 3)
    {
        for (int i = 0; i < 3; i++)
        {
            m_liWatingClients.front()->_clientState = ClientSession::ClientState::PLAYING;
            m_liWatingClients.erase(m_liWatingClients.begin());
        }
    }
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

            closesocket(pSession->_socket);

            continue;
        }

        if (pOverlap == &pSession->_overlapped_Send)
        {
#pragma region Send

            pSession->_responedTime = CTimer::GetInstance()->GetCurrTime();

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
                    //MSGBOX("WOD Server Send / Error_MySend. Not EWB, PENDING");
                    closesocket(pSession->_socket);
                }
            }
#pragma endregion Send
        }
        else
        {
#pragma region Recv
            pSession->_byteTransferred += Bytes;
            pSession->_byteToRead -= Bytes;

            if (pSession->_byteToRead < 0)
            {
                closesocket(pSession->_socket);
                continue;
            }

            pSession->_responedTime = CTimer::GetInstance()->GetCurrTime();

            while (true)
            {
                if (pSession->_isHeaderTransferred == false)
                {
                    //헤더가 캐싱된적이 없다
                    if (pSession->_byteTransferred < sizeof(PacketHeader))
                    {
                        //근데 헤더를 완성할 수 없다
                        pSession->_wsaBuffer_Recv.buf = &pSession->_buffer[pSession->_byteTransferred];
                        pSession->_wsaBuffer_Recv.len = pSession->_byteToRead;
                        break;
                    }
                    else
                    {
                        //헤더를 캐싱할 수 있다.
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
                    //헤더를 캐싱했다
                    if (pSession->_byteTransferred < pSession->_byteToRead)
                    {
                        //데이터를 다 못읽었다.
                        pSession->_wsaBuffer_Recv.buf = &pSession->_buffer[pSession->_byteTransferred];
                        pSession->_wsaBuffer_Recv.len = pSession->_byteToRead;
                        break;
                    }
                    else
                    {
                        //데이터를 다 읽었다.
                        if (pSession->_latestSendedHeader._packetType == PacketHeader::PacketType::HEARTBEAT)
                        {
                            pSession->_responedTime = CTimer::GetInstance()->GetCurrTime();
                        }
                        else
                        {
                            ExecuetionMessage
                            (
                                pSession->_latestSendedHeader._packetType,
                                pSession->_buffer,
                                pSession->_latestSendedHeader._packetSize
                            );
                        }

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
                closesocket(pSession->_socket);
                continue;
            }

            if (WSARecv(pSession->_socket, &pSession->_wsaBuffer_Recv, 1, &recvLen, &flag, &pSession->_overlapped_Recv, NULL) == SOCKET_ERROR)
            {
                int ERR = WSAGetLastError();
                if (ERR != WSAEWOULDBLOCK && ERR != WSA_IO_PENDING)
                {
                    closesocket(pSession->_socket);
                }
            }
#pragma endregion Recv
        }
    }
}

