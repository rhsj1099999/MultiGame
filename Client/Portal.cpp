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
	m_ePortalType = static_cast<PortalType>(_iNumber);
	m_PortalName = m_PortalNames[_iNumber];
}

void CPortal::Change_Scene()
{
	switch (m_ePortalType)
	{
	case CPortal::PortalType::SINGLE:
		CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE4);
		break;
	case CPortal::PortalType::MULTI:
		CSceneMgr::Get_Instance()->Scene_Change(SC_MATCHINGSTAGE);
		//MultiScene
		break;
	case CPortal::PortalType::EXIT:
		CSceneMgr::Get_Instance()->Scene_Change(SC_STAGE_EXIT);
		break;
	case CPortal::PortalType::END:
		break;
	default:
		break;
	}
}
