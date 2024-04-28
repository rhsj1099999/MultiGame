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

void Stage_Matching::Update()
{
    if (m_bClientConnected == true)
        return;

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
                {
                    HANDLE hCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
                    m_bClientConnected = true;
                    for (int i = 0; i < 2; i++)
                    {
                        m_vecWorkerThreads.push_back(thread([=]()
                            {
                                WorkerEntry(hCPHandle, &m_wsaBuffer);
                            }));
                    }

                    if (m_pOverlapEX != nullptr)
                        delete m_pOverlapEX;

                    m_pOverlapEX = new OverlappedExtend();
                    m_pOverlapEX->eType = READ;

                    m_wsaBuffer.buf = m_Buffer;
                    m_wsaBuffer.len = BUFSIZE;
                    DWORD recvLen = 0;
                    DWORD flag = 0;
                    WSARecv(m_Socket, &m_wsaBuffer, 1, &recvLen, &flag, &m_pOverlapEX->OverlappedEvent, NULL);
                }

                break;
            }
        }
    }
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
        int CurrUsers = 1;
        int MaxUsers = 4;
        
        int User = 0;
        memcpy(&User, m_wsaBuffer.buf, sizeof(int));

        if (User >= MaxUsers)
        {
            m_bSceneChangeeTrigger = true;
        }

        wsprintf(szBuff, L"매칭중... 인원수 = %d / %d", User, MaxUsers);
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



    if (m_Socket)
    {
        closesocket(m_Socket);
    }
    
    WSACleanup();

    CObjMgr::Get_Instance()->Release();

    CObjMgr::Get_Instance()->Destroy_Instance();
}
