#include "stdafx.h"
#include "Mole.h"
#include "UIMgr.h"
#include "MolePlayer.h"

CMole::CMole()
{
}

CMole::~CMole()
{
}

void CMole::Initialize(void)
{
	m_fAngle = 0.f;
	m_fYscale = 1.f;
	m_tInfo.vPos = { 0.f, 0.f, 0.f };
	m_tInfo.vScale = { 1.f , 1.f, 1.f };

	m_vPoint[0] = { -40.f, -90.f, 0.f };
	m_vPoint[1] = { -40.f, 0.f, 0.f };
	m_vPoint[2] = { 40.f, 0.f, 0.f };
	m_vPoint[3] = { 40.f, -90.f, 0.f };
	m_vPoint[4] = { 30.f, -100.f, 0.f };
	m_vPoint[5] = { -30.f, -100.f, 0.f };

	m_vColliderPoint[0] = { -40.f, 15.f, 0.f };
	m_vColliderPoint[1] = { -40.f, -15.f, 0.f };
	m_vColliderPoint[2] = { 40.f, -15.f, 0.f };
	m_vColliderPoint[3] = { 40.f, 15.f, 0.f };

	m_dwReviveInterval = GetTickCount64();
	m_iReviveDelay = 0;

}

int CMole::Update(void)
{
	m_tColliderInfo.vPos = { 0.f, -85.f, 0.f };

	if (m_fYscale == 0.2f)
	{
		if (m_dwReviveInterval + (m_iReviveDelay * 1000) < GetTickCount64())
		{
			m_fYscale = 1.f;
		}
	}

	Cal_WorldMatrix();

    return OBJ_NOEVENT;
}

void CMole::Late_Update(void)
{
}

void CMole::Render(HDC hDC)
{
	MoveToEx(hDC, m_PointVec[5].x, m_PointVec[5].y, nullptr);

	for (int i = 0; i < 6; ++i)
		LineTo(hDC, m_PointVec[i].x, m_PointVec[i].y);

	/*MoveToEx(hDC, m_ColliderPointVec[3].x, m_ColliderPointVec[3].y, nullptr);
	for (int i = 0; i < 4; ++i)
		LineTo(hDC, m_ColliderPointVec[i].x, m_ColliderPointVec[i].y);*/
}

void CMole::Release(void)
{
}

void CMole::Cal_WorldMatrix()
{
	D3DXMATRIX Scale;
	D3DXMatrixScaling(&Scale, 1.f, m_fYscale,1.f);

	D3DXMATRIX RotZ;
	D3DXMatrixRotationZ(&RotZ, m_fAngle);

	D3DXMATRIX Trans;
	D3DXMatrixTranslation(&Trans, m_tInfo.vPos.x, m_tInfo.vPos.y, m_tInfo.vPos.z);

	//월드행렬은 매 프레임
	m_tInfo.matWorld = Scale * RotZ * Trans;

	for (int i = 0; i < 6; i++)
		D3DXVec3TransformCoord(&m_PointVec[i], &m_vPoint[i], &m_tInfo.matWorld);

	D3DXMATRIX ColliderTrans;
	D3DXMatrixTranslation(&ColliderTrans, m_tColliderInfo.vPos.x, m_tColliderInfo.vPos.y, m_tColliderInfo.vPos.z);
	m_tColliderInfo.matWorld = ColliderTrans * m_tInfo.matWorld;

	for (int i = 0; i < 4; i++)
		D3DXVec3TransformCoord(&m_ColliderPointVec[i], &m_vColliderPoint[i], &m_tColliderInfo.matWorld);
}

void CMole::OnTrigger()
{
}

void CMole::OnCollision(CObj* _other)
{
	if (m_fYscale == 1.f)
	{
		m_fYscale = 0.2f;

		int iCombo = 0.f;
		float fMoneyScale = 0.f;
		iCombo = dynamic_cast<CMolePlayer*>(_other)->Get_Combo();
		fMoneyScale = dynamic_cast<CMolePlayer*>(_other)->Get_MoneyScale();

		srand(unsigned(time(nullptr)));
		
		m_iReviveDelay = rand() % 2 + 1;
		m_dwReviveInterval = GetTickCount64();
		
		//한마리당 달러 x 배율
		CUIMgr::Get_Instance()->Set_Money(CUIMgr::Get_Instance()->Get_Money() + (20 * fMoneyScale));
		CUIMgr::Get_Instance()->Set_MoleCnt(CUIMgr::Get_Instance()->Get_MoleCnt() + 1);
	}

}


/// <summary>
/// 충돌체 Get함수
/// </summary>
/// <returns></returns>
RECT CMole::Get_ColliderRect()
{
	RECT temp = { m_ColliderPointVec[1].x, m_ColliderPointVec[1].y, m_ColliderPointVec[3].x ,m_ColliderPointVec[3].y };

	return temp;
}
