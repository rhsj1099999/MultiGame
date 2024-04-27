#include "stdafx.h"
#include "Player.h"
#include "KeyMgr.h"
#include "UIMgr.h"


CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	Release();
}

void CPlayer::Initialize(void)
{
	m_tInfo.vLook = { 1.f, 0.f, 0.f };
	m_tInfo.vScale.x = 100;
	m_tInfo.vScale.y = 100;
	m_fSpeed = 5.f;
}

int CPlayer::Update(void)
{
	Key_Input();
	return OBJ_NOEVENT;
}

void CPlayer::Render(HDC hDC)
{
	Rectangle(hDC,
		int(m_tInfo.vPos.x - m_tInfo.vScale.x * 0.5f),
		int(m_tInfo.vPos.y + m_tInfo.vScale.y * 0.5f),
		int(m_tInfo.vPos.x + m_tInfo.vScale.x * 0.5f),
		int(m_tInfo.vPos.y - m_tInfo.vScale.y * 0.5f)
	);
}

void CPlayer::Release(void)
{
	
}

void CPlayer::Key_Input(void)
{
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT))
	{
		m_tInfo.vPos.x -= m_fSpeed;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
	{
		m_tInfo.vPos.x += m_fSpeed;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP))
	{
		m_tInfo.vPos.y -= m_fSpeed;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN))
	{
		m_tInfo.vPos.y += m_fSpeed;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing('F'))
	{
		if (CUIMgr::Get_Instance()->Get_Money() < 300)
			CUIMgr::Get_Instance()->Set_Money(300);
	}

	if (CKeyMgr::Get_Instance()->Key_Down('G'))
	{
		CUIMgr::Get_Instance()->Add_Money(5000);
	}
}

void CPlayer::Late_Update(void)
{
}

void CPlayer::OnTrigger()
{
}

void CPlayer::Cal_WorldMatrix()
{
}


