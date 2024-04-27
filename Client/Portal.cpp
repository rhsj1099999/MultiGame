#include "stdafx.h"
#include "Portal.h"
#include "SceneMgr.h"
#include "UIMgr.h"

void CPortal::Initialize(void)
{
	m_tInfo.vScale.x = 200;
	m_tInfo.vScale.y = 100;
}

int CPortal::Update(void)
{
	return 0;
}

void CPortal::Late_Update(void)
{
}

void CPortal::Render(HDC hDC)
{
	Rectangle(hDC,
		int(m_tInfo.vPos.x - m_tInfo.vScale.x * 0.5f),
		int(m_tInfo.vPos.y - m_tInfo.vScale.y * 0.5f),
		int(m_tInfo.vPos.x + m_tInfo.vScale.x * 0.5f),
		int(m_tInfo.vPos.y + m_tInfo.vScale.y * 0.5f));

	wchar_t PortalName[255]{};
	swprintf_s(PortalName, this->m_PortalName.c_str());
	TextOut(hDC, (m_tInfo.vPos.x - lstrlen(PortalName) / 2 - 10) - 22, m_tInfo.vPos.y - 7, PortalName, lstrlen(PortalName));
}

void CPortal::Release(void)
{
}

void CPortal::OnTrigger()
{
	Change_Scene();
}

void CPortal::Cal_WorldMatrix()
{
}

void CPortal::Set_PortalNumber(int _iNumber)
{
	m_iPortalNumber = _iNumber;
	switch (m_iPortalNumber)
	{
	case 1:
		this->m_PortalName = L"µÎ´õÁö Àâ±â";
		break;
	case 2:
		this->m_PortalName = L"·ê·¿";
		break;
	case 3:
		this->m_PortalName = L"³¬½Ã";
		break;
	case 4:
		this->m_PortalName = L"ÇØÀû ·ê·¿";
		break;
	case 5:
		this->m_PortalName = L"Å»Ãâ±¸";
		break;
	}
}

void CPortal::Change_Scene()
{
	switch (m_iPortalNumber)
	{
	case 1:
		if (CUIMgr::Get_Instance()->Get_Money() >= 500)
		{
			CUIMgr::Get_Instance()->Add_Money(-500);
			CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE1);
		}

		break;
	case 2:
		CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE2);
		break;
	case 3:
		if (CUIMgr::Get_Instance()->Get_Money() >= 100)
		{
			CUIMgr::Get_Instance()->Add_Money(-100);
			CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE3);
		}
		break;
	case 4:
		CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE4);
		break;
	case 5:
		if (CUIMgr::Get_Instance()->Get_Money() >= 5000)
		{
			CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE_EXIT);
		}
		break;
	}
}
