#include "stdafx.h"
#include "DownCase.h"
#include "KeyMgr.h"

CDownCase::CDownCase()
{
}

void CDownCase::Initialize()
{
	m_eObjID = OBJ_DOWNCASE;
	m_fAngleSpeed = 3.0f;
	m_fAngle = 0.0f;
	m_tInfo.vPos = { 0.0f,0.0f,0.0f };
	float TempAngle = static_cast<float>((360 / Points));
	for (int i = 0; i < 360 / (360 / Points); i++)
	{
		float TempRadian = TempAngle * i * M_PI / 180.f;
		D3DXVECTOR3 TempVecor = { m_tInfo.vPos.x + (m_fCaseDiameter / 2 * cosf(TempRadian)),m_tInfo.vPos.y + (m_fCaseDiameter / 2 * sinf(TempRadian)),0.0f };
		m_LocalPointVector.push_back(TempVecor);
	}
	for (int i = 0; i < 360 / (360 / Points); i++)
	{
		float TempRadian = TempAngle * i * M_PI / 180.f;
		D3DXVECTOR3 TempVecor = { m_tInfo.vPos.x + (m_fCaseDiameter / 2 * cosf(TempRadian)),m_tInfo.vPos.y + (m_fCaseDiameter / 2 * sinf(TempRadian)),0.0f };
		m_WorldPointVector.push_back(TempVecor);
	}
	m_tInfo.vPos = { 400,900,0.0f };
}

int CDownCase::Update()
{
	if (CKeyMgr::Get_Instance()->Key_Pressing('Q'))
	{
		m_fAngle += m_fAngleSpeed;

		if (m_fAngle > 360.0f)
		{
			m_fAngle = 0.0f;
		}
		float TempRadian = m_fAngle * 3.14f / 180.0f;
		m_tInfo.vLook.x = cos(TempRadian);
		m_tInfo.vLook.y = sin(TempRadian);
	}
	else
	{
		if (CKeyMgr::Get_Instance()->Key_Pressing('E'))
		{
			m_fAngle -= m_fAngleSpeed;

			if (m_fAngle < 0.0f)
			{
				m_fAngle = 360.0f;
			}
			float TempRadian = m_fAngle * 3.14f / 180.0f;
			m_tInfo.vLook.x = cos(TempRadian);
			m_tInfo.vLook.y = sin(TempRadian);
		}
	}

	Cal_WorldMatrix();

	return 0;
}

void CDownCase::Late_Update()
{
}

void CDownCase::Render(HDC hDC)
{
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
}

void CDownCase::Release()
{
}

void CDownCase::Cal_WorldMatrix()
{
	D3DXMATRIX TempMatrixToWorld_Size = {};
	D3DXMATRIX TempMatrixToWorld_Rotation = {};
	D3DXMATRIX TempMatrixToWorld_Location = {};
	D3DXMatrixScaling(&TempMatrixToWorld_Size, 1.0f, 0.5f, 1.0f);
	D3DXMatrixTranslation(&TempMatrixToWorld_Location, m_tInfo.vPos.x, m_tInfo.vPos.y, m_tInfo.vPos.z);
	D3DXMatrixRotationZ(&TempMatrixToWorld_Rotation, D3DXToRadian(m_fAngle));
	D3DXMATRIX TempMatrixToWorld = {};
	TempMatrixToWorld = TempMatrixToWorld_Rotation * TempMatrixToWorld_Location;
	for (int i = 0; i < static_cast<int>(m_LocalPointVector.size()); ++i)
	{
		D3DXVec3TransformCoord(&m_WorldPointVector[i], &m_LocalPointVector[i], &TempMatrixToWorld);
	}
	for (int i = 0; i < static_cast<int>(m_LocalPointVector.size()); ++i)
	{
		D3DXVec3TransformCoord(&m_WorldPointVector[i], &m_WorldPointVector[i], &TempMatrixToWorld_Size);
	}
}
