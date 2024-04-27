#include "stdafx.h"
#include "Obj.h"


CObj::CObj()
	: m_fSpeed(0.f)
	, m_bDead(false)
{
	ZeroMemory(&m_tInfo, sizeof(INFO));
}


CObj::~CObj()
{
}

void CObj::Set_Pos(float _fX, float _fY)
{
	m_tInfo.vPos = { _fX,_fY, 0 };
}

RECT CObj::Get_Rect()
{
	RECT temp =
	{
		m_tInfo.vPos.x - m_tInfo.vScale.x * 0.5f,
		m_tInfo.vPos.y - m_tInfo.vScale.y * 0.5f,
		m_tInfo.vPos.x + m_tInfo.vScale.x * 0.5f,
		m_tInfo.vPos.y + m_tInfo.vScale.y * 0.5f
	};
	return temp;
}

void CObj::OnTrigger()
{
}

/// <summary>
/// �δ�����⿡�� ����ϴ� �浹Ʈ����
/// </summary>
/// <param name="_other"></param>
void CObj::OnCollision(CObj* _other)
{
}
