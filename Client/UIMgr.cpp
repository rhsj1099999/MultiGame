#include "stdafx.h"
#include "UIMgr.h"

#include "SceneMgr.h"

CUIMgr* CUIMgr::m_pInstance = nullptr;

void CUIMgr::Initialize()
{
	m_iMoney = 500;
}

void CUIMgr::Render(HDC hDC)
{
	if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_WORLDMAP)
	{
		TCHAR szBuff[32];
		wsprintf(szBuff, L"������ : %d�޷�", m_iMoney);
		TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
	}
	else if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_STAGE2)
	{
		TCHAR szBuff[32];
		wsprintf(szBuff, L"������ : %d�޷�", m_iMoney);
		TextOut(hDC, 450, 15, szBuff, lstrlen(szBuff));
		wsprintf(szBuff, L"�����̽��� : ���� ����");
		TextOut(hDC, 440, 540, szBuff, lstrlen(szBuff));
		wsprintf(szBuff, L"���� 0~9 : 5��, Ȧ/¦ : 2��");
		TextOut(hDC, 200, 560, szBuff, lstrlen(szBuff));

		wsprintf(szBuff, L"���� : �����");
		TextOut(hDC, 488, 520, szBuff, lstrlen(szBuff));
		wsprintf(szBuff, L"��Ŭ�� : +100�޷�");
		TextOut(hDC, 472, 560, szBuff, lstrlen(szBuff));
		wsprintf(szBuff, L"��Ŭ�� : -100�޷�");
		TextOut(hDC, 472, 580, szBuff, lstrlen(szBuff));

		if (m_bRoulletWaiting)
		{

		}
		else
		{
			if (m_iRoulletMoney >= 0)
			{
				wsprintf(szBuff, L"%d(��)�� ���Խ��ϴ�. +%d�޷�", m_iRoulletNumber, m_iRoulletMoney);
				TextOut(hDC, 185, 580, szBuff, lstrlen(szBuff));
			}
			else if (m_iRoulletMoney < 0)
			{
				wsprintf(szBuff, L"%d(��)�� ���Խ��ϴ�.  %d�޷�", m_iRoulletNumber, m_iRoulletMoney);
				TextOut(hDC, 185, 580, szBuff, lstrlen(szBuff));
			}
		}
	}
	else if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_STAGE1)
	{
		TCHAR szBuff[32];
		wsprintf(szBuff, L"������ : %d�޷�", m_iMoney);
		TextOut(hDC, 650, 25, szBuff, lstrlen(szBuff));

		wsprintf(szBuff, L"���� �δ��� : %d����", m_iMoleCnt);
		TextOut(hDC, 20, 25, szBuff, lstrlen(szBuff));
	}
	else if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_STAGE4)
	{
		TCHAR szBuff[32];
		wsprintf(szBuff, L"������ : %d�޷�", m_iMoney);
		TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
	}
	else if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_STAGE3)
	{
		TCHAR szBuff[32];
		wsprintf(szBuff, L"������ : %d�޷�", m_iMoney);
		TextOut(hDC, 350, 25, szBuff, lstrlen(szBuff));
	}


}

void CUIMgr::Update()
{
}