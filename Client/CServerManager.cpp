#include "stdafx.h"
#include "CServerManager.h"

CServerManager* CServerManager::m_pInstance = nullptr;

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
            if (m_IOCPHandle == INVALID_HANDLE_VALUE)
                return;

            SR1_MSGBOX("GQCS Fail");

            continue;
        }

        pSession->CQPtr->Enqueqe_Ptr(pSession->wsaBuf.buf, Bytes); //���� �������� len�� ��ǥ������, ������� �ƴ�.

        pSession->ByteTransferred += Bytes;
        pSession->ByteToRead -= Bytes;

        if (pSession->ByteToRead < 0)
        {
            SR1_MSGBOX("ERROR : ByteToRead Under 0");
        }

        static int WhileCount = 0;

        while (true)
        {
            ++WhileCount;

            if (WhileCount > 3)
            {
                SR1_MSGBOX("ERROR : ByteToRead Under 0");
            }

            int iCurrSize = pSession->CQPtr->GetSize();

            if (iCurrSize == 0)
            {
                WhileCount = 0;
                break;
            }


            if (pSession->bHeaderTransferred == false)
            {
                if (iCurrSize < sizeof(PREDATA))
                {
                    WhileCount = 0;
                    break;
                }
                else
                {
                    pSession->pLatestHead = *(PREDATA*)pSession->CQPtr->GetFrontPtr();
                    pSession->CQPtr->Dequq_N(sizeof(PREDATA));
                    pSession->bHeaderTransferred = true;
                }
            }

            int DataSize = pSession->pLatestHead.iSizeStandby;

            iCurrSize = pSession->CQPtr->GetSize();; //���� ����� �޾���

            if (iCurrSize < DataSize)
            {
                break; //����� �ϼ��ƴµ� ���޾��� ���� �޾ƿ� ��
            }
            else
            {
                //�˳��ϰ� ����
                pSession->bHeaderTransferred = false;

                int* UserCount = (int*)pSession->CQPtr->GetFrontPtr();
                ExecuetionMessage(pSession->pLatestHead.eOrderType, pSession->CQPtr->GetFrontPtr(), sizeof(int));
                pSession->CQPtr->Dequq_N(iCurrSize);

                if (pSession->CQPtr->GetSize() == 0)
                {
                    //���� �ƹ��͵� ������ Ż��. ������� �ٽ� �����·� ����
                    WhileCount = 0;
                    break;
                }
            }
        }

        DWORD recvLen = 0;
        DWORD flag = 0;

        if (pSession->ByteToRead == 0)
        {
            pSession->ByteTransferred = 0;
            pSession->ByteToRead = sizeof(PREDATA) + sizeof(int);
        }
        pSession->wsaBuf.len = pSession->ByteToRead;

        if (WSARecv(pSession->soc, &pSession->wsaBuf, 1, &recvLen, &flag, &pSession->OverlappedEvent, NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                //SR1_MSGBOX("WSARecv ERROR But Not Pending");
                closesocket(m_Socket);
            }
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
                pSession->OverlappedEvent = {};
                pSession->OverlappedEvent.hEvent = WSACreateEvent();
                pSession->eType = READ;
                pSession->wsaBuf.buf = pSession->recvBuffer;

                pSession->ByteTransferred = 0;
                pSession->ByteToRead = sizeof(PREDATA) + sizeof(int);
                pSession->CQPtr = new CMyCQ(64);
                //Ŭ���̾�Ʈ �������� pSession->wsaBuf.len�� �ڽ��� ���ǿ� ���� �ٲ����
                pSession->wsaBuf.len = sizeof(PREDATA) + sizeof(int);
                //Ŭ���̾�Ʈ �������� pSession->wsaBuf.len�� �ڽ��� ���ǿ� ���� �ٲ����
                CreateIoCompletionPort((HANDLE)m_Socket, m_IOCPHandle, /*Key*/(ULONG_PTR)pSession, 0); //����Ҷ���


                DWORD recvLen = 0;
                DWORD flag = 0;
                if (WSARecv(pSession->soc, &pSession->wsaBuf, 1, &recvLen, &flag, &pSession->OverlappedEvent, NULL) == SOCKET_ERROR)
                {
                    if (WSAGetLastError() != WSA_IO_PENDING)
                    {
                        //SR1_MSGBOX("WSARecv ERROR But Not Pending");
                        closesocket(m_Socket);
                    }
                }
                break;
            }
        }
    }

    //if (recv(m_Socket, m_Buffer, sizeof(m_Buffer), 0) == SOCKET_ERROR)
    //{
    //    if (WSAGetLastError() != WSAEWOULDBLOCK)
    //    {
    //        //Game Break;
    //    }
    //}
    //else
    //{
    //    memcpy(&m_iCurrUser, m_Buffer, sizeof(int));
    //}
}

void CServerManager::ExecuetionMessage(PREDATA::OrderType eType, void* Data, int DataSize)
{
	switch (eType)
	{
	case PREDATA::OrderType::TEST1:
		memcpy(&m_iCurrUser, (int*)Data, DataSize);
		break;
	case PREDATA::OrderType::TEST2:
		memcpy(&m_iCurrUser, (int*)Data, DataSize);
		break;
	case PREDATA::OrderType::END:
		memcpy(&m_iCurrUser, (int*)Data, DataSize);
		break;
	default:
		break;
	}
}
