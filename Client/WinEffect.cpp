#include "stdafx.h"
#include "WinEffect.h"
#include "KeyMgr.h"
#include "CServerManager.h"

CWinEffect::CWinEffect()
{
}

void CWinEffect::Initialize()
{
	int RandBrush = rand() % 3;
	
	switch (RandBrush)
	{
	case 0:
		m_ColRef = RGB(255, 0, 0);
		break;
	case 1:
		m_ColRef = RGB(0, 255, 0);
		break;
	case 2:
		m_ColRef = RGB(0, 0, 255);
		break;
	default:
		break;
	}

	m_eObjID = OBJ_WINEFFECT;
	m_fAngle = 0.0f;
	m_tInfo.vPos = { static_cast<float>(rand() % WINCX),0,0.0f}; //·£´ý

	m_fAngleSpeed = static_cast<float>(WINEFFECTMINROTSPEED + (rand() % WINEFFECTMAXROTSPEED));

	m_fSpeed = static_cast<float>(WINEFFECTMINSPEED + (rand() % WINEFFECTMAXSPEED));

	float TempAngle = static_cast<float>((360 / 4));

	for (int i = 0; i < 360 / (360 / 4); i++)
	{
		float TempRadian = TempAngle * i * M_PI / 180.f;
		D3DXVECTOR3 TempVecor = { m_tInfo.vPos.x + (m_fCaseDiameter / 2 * cosf(TempRadian)),m_tInfo.vPos.y + (m_fCaseDiameter / 2 * sinf(TempRadian)),0.0f };
		m_LocalPointVector.push_back(TempVecor);
	}
	for (int i = 0; i < 360 / (360 / 4); i++)
	{
		float TempRadian = TempAngle * i * M_PI / 180.f;
		D3DXVECTOR3 TempVecor = { m_tInfo.vPos.x + (m_fCaseDiameter / 2 * cosf(TempRadian)),m_tInfo.vPos.y + (m_fCaseDiameter / 2 * sinf(TempRadian)),0.0f };
		m_WorldPointVector.push_back(TempVecor);
	}
}

int CWinEffect::Update()
{
	m_fAngle += m_fAngleSpeed;
	m_tInfo.vPos.y += m_fSpeed;

	Cal_WorldMatrix();

	return 0;
}

void CWinEffect::Late_Update()
{
}

void CWinEffect::Render(HDC hDC)
{
	HPEN TempRedPen = CreatePen(PS_SOLID, 10, m_ColRef);
	HPEN TempHoldPen = (HPEN)SelectObject(hDC, TempRedPen);

	MoveToEx
	(
		hDC,
		m_WorldPointVector[0].x,
		m_WorldPointVector[0].y,
		nullptr
	);
	for (int i = 0; i < static_cast<int>(m_WorldPointVector.size()); ++i)
	{
		LineTo
		(
			hDC,
			m_WorldPointVector[i].x,
			m_WorldPointVector[i].y
		);
	}
	LineTo
	(
		hDC,
		m_WorldPointVector[0].x,
		m_WorldPointVector[0].y
	);


	DeleteObject(SelectObject(hDC, TempHoldPen));
}

void CWinEffect::Release()
{
	
}

void CWinEffect::Cal_WorldMatrix()
{
	D3DXMATRIX TempMatrixToWorld_Size = {};
	D3DXMATRIX TempMatrixToWorld_Rotation = {};
	D3DXMATRIX TempMatrixToWorld_Location = {};

	D3DXMatrixScaling(&TempMatrixToWorld_Size, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationX(&TempMatrixToWorld_Rotation, D3DXToRadian(m_fAngle));
	D3DXMatrixTranslation(&TempMatrixToWorld_Location, 0.0f, m_tInfo.vPos.y, 0.0f);

	D3DXMATRIX TempMatrixToWorld = {};
	TempMatrixToWorld = TempMatrixToWorld_Size * TempMatrixToWorld_Rotation * TempMatrixToWorld_Location;
	for (int i = 0; i < static_cast<int>(m_LocalPointVector.size()); ++i)
	{
		D3DXVec3TransformCoord(&m_WorldPointVector[i], &m_LocalPointVector[i], &TempMatrixToWorld);
	}
}
