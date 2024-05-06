#include "stdafx.h"
#include "Stage4.h"
#include "AbstractFactory.h"
#include "ObjMgr.h"
#include "UIMgr.h"
#include "SceneMgr.h"
#include "KeyMgr.h"
#include "CServerManager.h"

CStage4::CStage4()
{

}

CStage4::~CStage4()
{
	Release();
}

void CStage4::SetServerMode(bool bMode)
{
	__super::SetServerMode(bMode);
}

void CStage4::Initialize()
{
	CObjMgr::Get_Instance()->Add_Object(OBJ_DOWNCASE, CAbstractFactory<CDownCase>::Create(m_bIsServerMode));
	CObjMgr::Get_Instance()->Add_Object(OBJ_UPCASE, CAbstractFactory<CUpCase>::Create(m_bIsServerMode));
	CObjMgr::Get_Instance()->Add_Object(OBJ_MIDCASE, CAbstractFactory<CMidCase>::Create(m_bIsServerMode));
}

void CStage4::Update()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_ESCAPE))
	{
		CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);
	}
	else
	{
		CObjMgr::Get_Instance()->Update();
	}

}

void CStage4::Late_Update()
{
}

void CStage4::Render(HDC hDC)
{
	CObjMgr::Get_Instance()->Render(hDC);
	CUIMgr::Get_Instance()->Render(hDC);

	//if (CServerManager::Get_Instance()->GetMyTurn() == true)
	//{
	//	TCHAR szBuff[64];
	//	wsprintf(szBuff, L"%s", L"Your Turn.");
	//	TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
	//}
	//else
	//{

	//}
	TCHAR szBuff[64];
	
	if (CServerManager::Get_Instance()->GetMyTurn() == true)
	{
		wsprintf(szBuff, L"%s", L"Your Turn");
		TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
	}
	else
	{
		int CurrPlayer = CServerManager::Get_Instance()->GetCurrentPlayer();

		if (CurrPlayer == -1)
		{
			wsprintf(szBuff, L"%s", L"...Waiting...");
			TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
		}
		else
		{
			wsprintf(szBuff, L"Player[ %d ]'s Turn", CurrPlayer);
			TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
		}
	}

}

void CStage4::Release()
{
	CObjMgr::Get_Instance()->Release();
	CObjMgr::Get_Instance()->Destroy_Instance();
}
