#include "stdafx.h"
#include "Stage_Matching.h"
#include "AbstractFactory.h"
#include "ObjMgr.h"
#include "UIMgr.h"
#include "SceneMgr.h"
#include "KeyMgr.h"
#include "CServerManager.h"

Stage_Matching::Stage_Matching()
{
}

Stage_Matching::~Stage_Matching()
{
    Release();
}

void Stage_Matching::Initialize()
{
    CServerManager::Get_Instance()->InitServer();
}


void Stage_Matching::Update()
{
}

void Stage_Matching::Late_Update()
{
}

void Stage_Matching::Render(HDC hDC)
{
    TCHAR szBuff[64];

    if (CServerManager::Get_Instance()->GetClientConnected() == false)
    {
        wsprintf(szBuff, L"연결중...");
        TextOut(hDC, WINCX / 2, WINCY / 2, szBuff, lstrlen(szBuff));
    }
    else
    {
        wsprintf(szBuff, L"매칭중... 인원수 = %d / %d", CServerManager::Get_Instance()->GetServerUser(), MAXCLIENTS);
        TextOut(hDC, WINCX / 2, WINCY / 2, szBuff, lstrlen(szBuff));
    }

    CObjMgr::Get_Instance()->Render(hDC);
    CUIMgr::Get_Instance()->Render(hDC);
}

void Stage_Matching::Release()
{
    CObjMgr::Get_Instance()->Release();
    CObjMgr::Get_Instance()->Destroy_Instance();
}
