#include "stdafx.h"
#include "CServerManager.h"
#include "SceneMgr.h"


CServerManager* CServerManager::m_pInstance = nullptr;

static bool SceneChanged = false;

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
            return;

            if (m_IOCPHandle == INVALID_HANDLE_VALUE)
                return;

            if (Bytes == 0)
                SR1_MSGBOX("ERROR : GQCS Read 0 Bytes");

            SR1_MSGBOX("GQCS Fail : Recieve At Client");
            //아마도 서버가 죽거나 서버컴 랜선이 뽑혔을때

            

            continue;
        }

        if (pOverlap == &pSession->Overlapped_Recv)
        {
#pragma region Read

            if (SceneChanged == true)
            {
                int a = 10;
            }

            pSession->ByteTransferred += Bytes;
            pSession->ByteToRead -= Bytes;


            if (pSession->ByteToRead < 0)
                SR1_MSGBOX("ERROR : ByteToRead Under 0");

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
                wchar_t Log[32] = {};
                wsprintf(Log, L"ByteToRead Error, %d", pSession->ByteToRead);
                MessageBox(0, Log, TEXT("Fail_"), MB_OK);
            }

            if (WSARecv(pSession->soc, &pSession->wsaBuf_Recv, 1, &recvLen, &flag, &pSession->Overlapped_Recv, NULL) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSA_IO_PENDING)
                {
                    closesocket(m_Socket);
                }
            }
#pragma endregion Read
        }
        else
        {
#pragma region Send
            DWORD recvLen = 0;
            DWORD flag = 0;
            if (WSASend(pSession->soc, &pSession->wsaBuf_Recv, 1, &recvLen, flag, &pSession->Overlapped_Send, NULL) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSA_IO_PENDING)
                {
                    closesocket(m_Socket);
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
	return 0;
}

void CServerManager::Late_Update()
{
}

void CServerManager::Release()
{
    if (m_bClientConnected == false)
        return;

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

	if (WSAStartup(MAKEWORD(2, 2), &m_wsa))
	{
		//Break;
	}

	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	unsigned long On = 1;
	ioctlsocket(m_Socket, FIONBIO, &On);
	bool bEnable = true;
	setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bEnable, sizeof(bEnable));
	setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&bEnable, sizeof(bEnable));

	if (m_Socket == INVALID_SOCKET)
	{
		//Break;
	}

	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
	m_ServerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &m_ServerAddr.sin_addr);
	m_ServerAddr.sin_port = htons(7777);

    while (true)
    {
        if (connect(m_Socket, (SOCKADDR*)&m_ServerAddr, sizeof(m_ServerAddr)) == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                continue;
            }

            if (WSAGetLastError() == WSAEISCONN)
            {
                m_bClientConnected = true;
                if (m_IOCPHandle == m_IOCPHandle)
                {
                    m_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
                }
                m_bClientConnected = true;
                for (int i = 0; i < 1; i++)
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
                pSession->CQPtr = new CMyCQ(64);
                pSession->wsaBuf_Recv.buf = pSession->recvBuffer;
                pSession->wsaBuf_Recv.len = sizeof(PREDATA);
                CreateIoCompletionPort((HANDLE)m_Socket, m_IOCPHandle, /*Key*/(ULONG_PTR)pSession, 0); //등록할때는


                DWORD recvLen = 0;
                DWORD flag = 0;


                pSession->Lock_WSARecv(&pSession->wsaBuf_Recv, 1, &recvLen, &flag, &pSession->Overlapped_Recv, NULL);
               
                //if (WSARecv(pSession->soc, &pSession->wsaBuf, 1, &recvLen, &flag, &pSession->Overlapped_Recv, NULL) == SOCKET_ERROR)
                //{
                //    if (WSAGetLastError() != WSA_IO_PENDING)
                //    {
                //        //SR1_MSGBOX("WSARecv ERROR But Not Pending");
                //        closesocket(m_Socket);
                //    }
                //}
                break;
            }
        }
    }
}

bool CServerManager::ExecuetionMessage(PREDATA::OrderType eType, void* Data, int DataSize)
{
    bool Ret = false;

	switch (eType)
	{
	case PREDATA::OrderType::USERCOUNT:
		memcpy(&m_iCurrUser, (int*)Data, sizeof(int));
		break;
	case PREDATA::OrderType::TEST2:
		memcpy(&m_iCurrUser, (int*)Data, DataSize);
		break;

    case PREDATA::OrderType::MESSAGECHANGE:
        memcpy(&m_pSession->wsaBuf_Recv.len, (int*)Data, DataSize);
        break;

    case PREDATA::OrderType::SCENECHANGE_TOPLAY:
    {
        m_iCurrUser = 1;
        CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE4, true);
        int a = 10;
        SceneChanged = true;
    }

        break;

    case PREDATA::OrderType::SCENECHANGE_TOWORLD:
        CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);
        break;

    case PREDATA::OrderType::TURNON:
        if (SceneChanged == false)
        {
            SR1_MSGBOX("TurnOnFirst");
        }
        m_bCanMove = true;
        break;

    case PREDATA::OrderType::TURNOFF:
        m_bCanMove = false;
        break;

	case PREDATA::OrderType::END:
		memcpy(&m_iCurrUser, (int*)Data, DataSize);
		break;
	default:
        SR1_MSGBOX("Wrong Message");
        Ret = true;
        int a = 10;
		break;
	}
    return Ret;
}
