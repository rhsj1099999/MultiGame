#include "stdafx.h"
#include "Stage_Matching.h"
#include "AbstractFactory.h"
#include "ObjMgr.h"
#include "UIMgr.h"
#include "SceneMgr.h"
#include "KeyMgr.h"

Stage_Matching::Stage_Matching()
{
}

Stage_Matching::~Stage_Matching()
{
    Release();
}

void Stage_Matching::Initialize()
{
#pragma region CreateSocket_ToServer
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
#pragma endregion CreateSocket_ToServer
}

void WorkerEntry_D(HANDLE hHandle, char* pOut, int size = 100)
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
            continue;
        }

        switch (pSession->eType)
        {
        case READ:
        {
            if (pOut != nullptr)
            {
                memcpy(pOut, pSession->wsaBuf.buf, size);
            }
            pSession->wsaBuf.buf = pSession->recvBuffer;
            pSession->wsaBuf.len = sizeof(pSession->recvBuffer);

            DWORD recvLen = 0;
            DWORD flag = 0;
            WSARecv(pSession->soc, &pSession->wsaBuf, 1, &recvLen, &flag, &pSession->OverlappedEvent, NULL);
            //WSARecv(pSession->soc, &pSession->wsaBuf, 1, &Bytes, &flag, (LPOVERLAPPED)&pOverlap, NULL);
            //아래껀 되고, 위에거는 안됨
        }
        break;

        case WRITE:
        {
        }
        break;

        case QUEUEWATING:
        {

            //int TempCurrUser = static_cast<int>(liClientSessions.size());
            //memset(pSession->recvBuffer, 0, sizeof(pSession->recvBuffer));
            //memcpy(pSession->recvBuffer, &TempCurrUser, sizeof(TempCurrUser));

            //pSession->wsaBuf.buf = pSession->recvBuffer;
            //pSession->wsaBuf.len = sizeof(pSession->recvBuffer);

            //DWORD recvLen = 0;
            //DWORD flag = 0;

            ////WSABUF DataBuf;
            ////DataBuf.buf = pSession->recvBuffer;
            ////DataBuf.len = BUFSIZE;

            ////DWORD recvLen = 0;
            ////DWORD flag = 0;
            //WSASend((pSession)->soc, &pSession->wsaBuf, 1, &recvLen, flag, &(pSession)->OverlappedEvent, NULL);
        }
        break;


        default:
            break;
        }
    }
}


void Stage_Matching::Update()
{
    while (true && m_bClientConnected == false)
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
//#pragma region ClientIOCP
                HANDLE hCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
                m_bClientConnected = true;
                for (int i = 0; i < 2; i++)
                {
                    m_vecWorkerThreads.push_back(thread([=]()
                        {
                            WorkerEntry_D(hCPHandle, m_Buffer);
                        }));
                }

                


                ClientSession* pSession = new ClientSession;
                pSession->soc = m_Socket;
                pSession->OverlappedEvent = {};
                pSession->OverlappedEvent.hEvent = WSACreateEvent();
                pSession->eType = READ;
                pSession->wsaBuf.buf = pSession->recvBuffer;
                pSession->wsaBuf.len = sizeof(pSession->recvBuffer);
                CreateIoCompletionPort((HANDLE)m_Socket, hCPHandle, /*Key*/(ULONG_PTR)pSession, 0); //등록할때는


                //WSABUF DataBuf;
                //DataBuf.buf = pSession->recvBuffer;
                //DataBuf.len = BUFSIZE;

                DWORD recvLen = 0;
                DWORD flag = 0;
                WSARecv(pSession->soc, &pSession->wsaBuf, 1, &recvLen, &flag, &pSession->OverlappedEvent, NULL);
//#pragma endregion ClientIOCP
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

void Stage_Matching::Late_Update()
{
   //SceneChange
    if (m_bSceneChangeeTrigger)
    {
        CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);
    }
}

void Stage_Matching::Render(HDC hDC)
{
    TCHAR szBuff[64];


    if (m_bClientConnected == false)
    {
        wsprintf(szBuff, L"연결중...");
        TextOut(hDC, WINCX / 2, WINCY / 2, szBuff, lstrlen(szBuff));
    }
    else
    {
        int MaxUsers = 4;

        memcpy(&m_iCurrUser, m_Buffer, sizeof(int));
        

        wsprintf(szBuff, L"매칭중... 인원수 = %d / %d", m_iCurrUser, MaxUsers);
        TextOut(hDC, WINCX / 2, WINCY / 2, szBuff, lstrlen(szBuff));
    }

    CObjMgr::Get_Instance()->Render(hDC);
    CUIMgr::Get_Instance()->Render(hDC);
}

void Stage_Matching::Release()
{
    for (vector<thread>::iterator itr = m_vecWorkerThreads.begin(); itr != m_vecWorkerThreads.end(); ++itr)
    {
        if (itr->joinable())
        {
            itr->join();
        }
    }



    shutdown(m_Socket, SD_BOTH);

    closesocket(m_Socket);
    
    WSACleanup();

    CObjMgr::Get_Instance()->Release();

    CObjMgr::Get_Instance()->Destroy_Instance();
}
