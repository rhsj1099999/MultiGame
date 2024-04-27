#include "stdafx.h"
#include "Tunnel.h"
#include "ObjMgr.h"
#include "AbstractFactory.h"
#include "Mole.h"

CTunnel::CTunnel()
{
}

CTunnel::~CTunnel()
{
}

void CTunnel::Initialize(void)
{
	//두더지 소환여부
	m_bSummon = false;

	//두더지 소환주기
	m_dwSummonTime = GetTickCount64();

	m_tInfo.vPos = { 0.f, 0.f, 0.f };
	m_tInfo.vScale = { 1.f , 1.f, 1.f };

	m_vPoint[0] = { -50.f, 50.f, 0.f };
	m_vPoint[1] = { -70.f, 0.f, 0.f };
	m_vPoint[2] = { -50.f, -50.f, 0.f };
	m_vPoint[3] = { 50.f, -50.f, 0.f };
	m_vPoint[4] = { 70.f, 0.f, 0.f };
	m_vPoint[5] = { 50.f, 50.f, 0.f };

}

int CTunnel::Update(void)
{
	Cal_WorldMatrix();

	if (!m_bSummon)
	{
		if (m_dwSummonTime + 3000 > GetTickCount64())
		{
			Summon_Mole();
			m_dwSummonTime = GetTickCount64();
		}

	}

	return OBJ_NOEVENT;
}

void CTunnel::Late_Update(void)
{
}

void CTunnel::Render(HDC hDC)
{
	MoveToEx(hDC, m_PointVec[5].x, m_PointVec[5].y, nullptr);
	
	for (int i = 0; i < 6; ++i)
		LineTo(hDC, m_PointVec[i].x, m_PointVec[i].y);
}

void CTunnel::Release(void)
{
}

void CTunnel::Cal_WorldMatrix()
{
	D3DXMATRIX Trans;
	D3DXMatrixTranslation(&Trans, m_tInfo.vPos.x, m_tInfo.vPos.y, m_tInfo.vPos.z);

	//월드행렬은 매 프레임
	m_tInfo.matWorld = Trans;

	for (int i = 0; i < 6; i++)
		D3DXVec3TransformCoord(&m_PointVec[i], &m_vPoint[i], &m_tInfo.matWorld);

}

void CTunnel::Summon_Mole()
{
	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLE, CAbstractFactory<CMole>::Create(m_tInfo.vPos.x, m_tInfo.vPos.y));
	m_bSummon = true;
}
