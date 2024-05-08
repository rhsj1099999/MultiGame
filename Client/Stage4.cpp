#include "stdafx.h"
#include "Stage4.h"
#include "AbstractFactory.h"
#include "ObjMgr.h"
#include "UIMgr.h"
#include "SceneMgr.h"
#include "KeyMgr.h"
#include "CServerManager.h"
#include "PirateHead.h"

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

	CUpCase* pUpCasePtr = static_cast<CUpCase*>(CAbstractFactory<CUpCase>::Create(m_bIsServerMode));
	CObjMgr::Get_Instance()->Add_Object(OBJ_UPCASE, pUpCasePtr);

	CObjMgr::Get_Instance()->Add_Object(OBJ_MIDCASE, CAbstractFactory<CMidCase>::Create(m_bIsServerMode));

	CServerManager::Get_Instance()->SetPirateHeadPtr(static_cast<CPirateHead*>(pUpCasePtr->GetPirateHeadPtr()));
}

void CStage4::Update()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_ESCAPE))
	{
		if (m_bIsServerMode)
		{
			CServerManager::Get_Instance()->ClearDatas();
			CServerManager::Get_Instance()->Release();
			CServerManager::Get_Instance()->Destroy_Instance();
		}
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

	TCHAR szBuff[64];

	if (m_bIsServerMode == true)
	{
		if (CServerManager::Get_Instance()->GetGameEnd() == true)
		{
			if (CServerManager::Get_Instance()->GetWin() == true)
			{
				wsprintf(szBuff, L"%s", L"...You Win!...");
				TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
			}
			else
			{
				wsprintf(szBuff, L"Player [ %d ] Win", CServerManager::Get_Instance()->GetWhoWin());
				TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
			}
		}
		else
		{
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
		

	}
	else
	{
		wsprintf(szBuff, L"소지금 : %d달러", CUIMgr::Get_Instance()->Get_Money());
		TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
	}
}

void CStage4::Release()
{
	CObjMgr::Get_Instance()->Release();
	CObjMgr::Get_Instance()->Destroy_Instance();
}
