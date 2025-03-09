#include "stdafx.h"
#include "MainServer.h"
#include "Timer.h"
#include "PlayingRoom.h"


CMainServer* CMainServer::_instance = nullptr;


CMainServer::CMainServer()
{
}

CMainServer::~CMainServer()
{
}


CMainServer* CMainServer::GetInstance()
{
    if (!_instance)
    {
        _instance = new CMainServer;
    }
    return _instance;
}

void CMainServer::Destroy_Instance()
{
    if (_instance)
    {
        delete _instance;
        _instance = nullptr;
    }
}





void CMainServer::Release()
{
    //핸들 닫기
    {
        CloseHandle(_iocpHandle);
        _iocpHandle = INVALID_HANDLE_VALUE;
    }

    //스레드 닫기
    {
        for (vector<thread>::iterator Itr = _workerThreads.begin(); Itr != _workerThreads.end(); ++Itr)
        {
            Itr->join();
        }
    }

    //클라 전부 닫기
    {
        LockGuard Temp(_mutex_CurrClientSession);

        for (ClientSession* pCS : _currClientSessions)
        {
            closesocket(pCS->_socket);

            if (pCS != nullptr)
            {
                delete pCS;
                pCS = nullptr;
            }
        }
        _currClientSessions.clear();
    }

    //방 닫기
    {
        for (list<CPlayingRoom*>::iterator Itr = _currPlayingRooms.begin(); Itr != _currPlayingRooms.end(); ++Itr)
        {
            delete (*Itr);
        }
        _currPlayingRooms.clear();
    }

    //서버 닫기
    {
        closesocket(_serverSocket);
        WSACleanup();
    }
}



void CMainServer::Init()
{
    //WSAStartUp
    WSADATA wsa;
    {
        if (WSAGetLastError_MessageReport(WSAStartup(MAKEWORD(2, 2), &wsa)) == false)
        {
            return;
        }
    }
    
    //소켓생성
    {
        _serverSocket = socket
        (
            AF_INET,
            SOCK_STREAM,
            0
        );

        unsigned long On = 1;
        ioctlsocket(_serverSocket, FIONBIO, &On);
        bool bEnable = true;
        setsockopt(_serverSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&bEnable, sizeof(bEnable));
    }

    //IP읽기 준비 (메모장에 있어요)
    string serverIPAddress;
    string serverIPAddressDirectory = "../OutPut/IPAddress.txt";
    {
        bool readSuccess = ReadStringFromFile(&serverIPAddressDirectory, &serverIPAddress);

        if (readSuccess == false)
        {
            serverIPAddressDirectory = "../../IPAddress.txt";
            readSuccess = ReadStringFromFile(&serverIPAddressDirectory, &serverIPAddress);
        }

        if (readSuccess == false)
        {
            SR1_MSGBOX("Fail To read Open AddressFile");
        }
    }

    SOCKADDR_IN serverADDR;
    //serverADDR 준비
    {
        ZeroMemory(&serverADDR, sizeof(serverADDR));
        serverADDR.sin_family = AF_INET;
        inet_pton(AF_INET, serverIPAddress.c_str(), &serverADDR.sin_addr);
        serverADDR.sin_port = htons(7777);
    }

    //소켓에 ADDR 바인드
    {
        WSAGetLastError_MessageReport(bind(_serverSocket, (SOCKADDR*)&serverADDR, sizeof(serverADDR)));
    }


    //IOCP 핸들 준비
    {
        _iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (_iocpHandle == INVALID_HANDLE_VALUE)
        {
            SR1_MSGBOX("Fail To Ready IOCPHandle");
        }
    }


    //워커 스레드 밀어넣기
    {
        for (int i = 0; i < 10; i++)
        {
            _workerThreads.push_back(thread([=]()
            {
                WorkerEntry_D(_iocpHandle);
            }));
        }
    }

    //리슨 시작
    {
        char serverIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &serverADDR.sin_addr, serverIP, INET_ADDRSTRLEN);
        if (WSAGetLastError_MessageReport(listen(_serverSocket, 10)) == false)
        {
            return;
        }

        std::cout << "Server IP: " << serverIP << std::endl;
        std::cout << "---Listen---" << std::endl;
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
    for (list<CPlayingRoom*>::iterator Itr = _currPlayingRooms.begin(); Itr != _currPlayingRooms.end(); ++Itr)
    {
        if ((*Itr) == RoomPtr)
        {
            delete (*Itr);

            Itr = _currPlayingRooms.erase(Itr);

            if (Itr == _currPlayingRooms.end())
                break;
        }
    }
}




void CMainServer::ConnectTry()
{
    SOCKET ClientSocket;
    SOCKADDR_IN ClientAddress;
    __int32 AddrLen = sizeof(ClientAddress);

    ClientSocket = accept(_serverSocket, (SOCKADDR*)&ClientAddress, &AddrLen);

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

    CreateIoCompletionPort((HANDLE)ClientSocket, _iocpHandle, /*Key*/(ULONG_PTR)pSession, 0);

    {
        LockGuard TempListLock(_mutex_CurrClientSession);
        _currClientSessions.push_back(pSession);
        _currUserCount = static_cast<__int32>(_currClientSessions.size());

        LockGuard TempQueueLock(_mutex_CurrWaitingClientSession);
        _currWaitingClientSessions.push_back(pSession);
    }

    cout << "Client Connected Users : " << _currUserCount << endl;

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

    for (ClientSession* pCS : _currClientSessions)
    {
        if (pCS->PlayingRoomPtr != nullptr)
            continue;

        PAK_UserCount packetUserCount = PAK_UserCount(_currUserCount);

        MySend<PAK_UserCount>(pCS, packetUserCount, PacketHeader::PacketType::USERCOUNT);
    }
    
    WSARecv(pSession->_socket, &pSession->_wsaBuffer_Recv, 1, &recvLen, &flag, &pSession->_overlapped_Recv, NULL);
}


void CMainServer::LiveCheck()
{
    DWORD dwCurrTime = CTimer::GetInstance()->GetCurrTime();
    DWORD recvLen = 0;
    DWORD flag = 0;

    bool bTempDead = false;

    for (list<ClientSession*>::iterator Itr = _currClientSessions.begin(); Itr != _currClientSessions.end(); ++Itr)
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
                LockGuard TempWating(_mutex_CurrWaitingClientSession);
                for (list<ClientSession*>::iterator Itr_W = _currWaitingClientSessions.begin(); Itr_W != _currWaitingClientSessions.end(); ++Itr_W)
                {
                    Itr_W = _currWaitingClientSessions.erase(Itr_W);

                    if (Itr_W == _currWaitingClientSessions.end())
                        break;
                }
            }


            LockGuard Temp(_mutex_CurrClientSession);

            if ((*Itr)->PlayingRoomPtr != nullptr)
                static_cast<CPlayingRoom*>((*Itr)->PlayingRoomPtr)->ClientDead((*Itr));

            closesocket((*Itr)->_socket);

            delete (*Itr);

            Itr = _currClientSessions.erase(Itr);

            _currUserCount = static_cast<int>(_currClientSessions.size());

            cout << "Client TimeOut. Users : " << _currUserCount << endl;
            

            if (Itr == _currClientSessions.end())
                break;
        }
    }
}

void CMainServer::MatchingRoom()
{
    if (_currWaitingClientSessions.size() >= MAXCLIENTS)
    {
        LockGuard Temp(_mutex_CurrWaitingClientSession);

        CPlayingRoom* pNewRoom = new CPlayingRoom();

        ClientSession* pArr[MAXCLIENTS] = { nullptr, };

        PAK_SceneChange pakcetGameStart
        {
            0,
            pNewRoom
        };

        PAK_TurnChanged packetTurnChanged
        {
            0,
        };

        for (int i = 0; i < MAXCLIENTS; i++)
        {
            ClientSession* pSession = _currWaitingClientSessions.front();

            pArr[i] = pSession;

            pSession->PlayingRoomPtr = pNewRoom;

            pakcetGameStart._myNumber = i;

            MySend<PAK_SceneChange>(pSession, pakcetGameStart, PacketHeader::PacketType::SCENECHANGE_TOPLAY);
            MySend<PAK_TurnChanged>(pSession, packetTurnChanged, PacketHeader::PacketType::TURNCHANGED);
            
            _currWaitingClientSessions.erase(_currWaitingClientSessions.begin());
        }

        pNewRoom->Init(pArr, _currClientSessions);
        _currPlayingRooms.push_back(pNewRoom);
    }
}

bool CMainServer::ExecuetionMessage(PacketHeader::PacketType eType, void* pData, int DataSize)
{
    bool Ret = false;

    switch (eType)
    {
    case PacketHeader::PacketType::ROTATEANGLE:
    {
        //클라가 로테이트 앵글을 했다
        PAK_ROTATEANGLE* pCasted = static_cast<PAK_ROTATEANGLE*>(pData);

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(pCasted->_myRoomPtr);

        pRoom->BroadCast_RotateAngle(pCasted);
    }
        break;
    case PacketHeader::PacketType::PLAYERBLADEINSERTED:
    {
        //클라가 칼을 꼽았다
        PAK_BLADEINSERT* pCasted = static_cast<PAK_BLADEINSERT*>(pData);

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(pCasted->_myRoomPtr);

        pRoom->BroadCast_BladeInserted(pCasted);
    }
        break;
    case PacketHeader::PacketType::HEARTBEAT:
        break;
    case PacketHeader::PacketType::CLIENTCHATSHOOT:
    {
        PAK_ChattingMessageToRoom* pCasted = static_cast<PAK_ChattingMessageToRoom*>(pData);

        CPlayingRoom* pRoom = static_cast<CPlayingRoom*>(pCasted->_myRoomPtr);

        pRoom->BroadCast_ClientChattingShoot(pCasted);
    }
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
    _currWaitingClientSessions.push_back((*Casted));
}

void CMainServer::Lock_Queue_ChangingRoom(void* Ptr)
{
    if (_currWaitingClientSessions.size() >= 3)
    {
        for (int i = 0; i < 3; i++)
        {
            _currWaitingClientSessions.front()->_clientState = ClientSession::ClientState::PLAYING;
            _currWaitingClientSessions.erase(_currWaitingClientSessions.begin());
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
            if (_iocpHandle == INVALID_HANDLE_VALUE)
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

