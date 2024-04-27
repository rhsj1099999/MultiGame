#include "stdafx.h"
#include "UpCase.h"
#include "KeyMgr.h"
#include "ObjMgr.h"
#include "CaseHoles.h"
#include "AbstractFactory.h"
#include "PirateHead.h"
#include "SceneMgr.h"

CUpCase::CUpCase()
{

}

void CUpCase::Initialize()
{
	m_eObjID = OBJ_UPCASE;
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
	for (int i = 0; i < 360 / (360 / Points); i++)
	{
		float TempRadian = TempAngle * i * M_PI / 180.f;
		D3DXVECTOR3 TempVecor = { m_tInfo.vPos.x + (m_fCaseDiameter * 0.65f / 2 * cosf(TempRadian)),m_tInfo.vPos.y + (m_fCaseDiameter * 0.65f / 2 * sinf(TempRadian)),0.0f };
		m_LocalShadeVector.push_back(TempVecor);
	}
	for (int i = 0; i < 360 / (360 / Points); i++)
	{
		float TempRadian = TempAngle * i * M_PI / 180.f;
		D3DXVECTOR3 TempVecor = { m_tInfo.vPos.x + (m_fCaseDiameter * 0.65f / 2 * cosf(TempRadian)),m_tInfo.vPos.y + (m_fCaseDiameter * 0.65f / 2 * sinf(TempRadian)),0.0f };
		m_WorldShadeVector.push_back(TempVecor);
	}
	m_tInfo.vPos = { 400,300,0.0f };

	
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			CObj* TempPtr = new CCaseHoles();
			CCaseHoles* TempCastPtr = dynamic_cast<CCaseHoles*>(TempPtr);
			TempCastPtr->Initialize();
			TempCastPtr->AftInit(this, 0.0f + static_cast<float>(i) * 24.0f, 40.0f + j * 200);
			CObjMgr::Get_Instance()->Add_Object(OBJ_CASEHOLES, TempPtr);
			this->m_HolesPtrVector.push_back(TempPtr);
		}

	}
	CObj* TempHeadPtr = new CPirateHead();
	CPirateHead* TempHeadCastPtr = dynamic_cast<CPirateHead*>(TempHeadPtr);
	TempHeadCastPtr->Initialize();
	TempHeadCastPtr->AftInit(this);
	CObjMgr::Get_Instance()->Add_Object(OBJ_CASEHOLES, TempHeadCastPtr);


	int TempSizeInt = static_cast<int>(this->m_HolesPtrVector.size());
	int GetMoneyInt = rand() % TempSizeInt;
	int LoseMoneyInt = 0;
	while (1)
	{
		LoseMoneyInt = rand() % TempSizeInt;
		if (GetMoneyInt != LoseMoneyInt)
		{
			break;
		}
	}
	CCaseHoles* TempCastPtr2 = dynamic_cast<CCaseHoles*>(this->m_HolesPtrVector[GetMoneyInt]);
	TempCastPtr2->SetReward(RWD_GETMONEY);
	TempCastPtr2 = dynamic_cast<CCaseHoles*>(this->m_HolesPtrVector[LoseMoneyInt]);
	TempCastPtr2->SetReward(RWD_LOSEALLMONEY);
}

int CUpCase::Update()
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

void CUpCase::Late_Update()
{

}

void CUpCase::Render(HDC hDC)
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



	MoveToEx
	(
		hDC,
		m_WorldShadeVector[0].x,
		m_WorldShadeVector[0].y,
		nullptr
	);
	for (int i = 0; i < static_cast<int>(m_WorldShadeVector.size()); ++i)
	{
		LineTo
		(
			hDC,
			m_WorldShadeVector[i].x,
			m_WorldShadeVector[i].y
		);
	}
	LineTo
	(
		hDC,
		m_WorldShadeVector[0].x,
		m_WorldShadeVector[0].y
	);



	for (int i = 0; i < static_cast<int>(m_WorldShadeVector.size()); ++i)
	{
		MoveToEx
		(
			hDC,
			m_WorldShadeVector[i].x,
			m_WorldShadeVector[i].y,
			nullptr
		);
		LineTo
		(
			hDC,
			m_WorldPointVector[i].x,
			m_WorldPointVector[i].y
		);
	}


	//HBRUSH TempShadeBrush = CreateSolidBrush(RGB(0, 0, 0));
	//HBRUSH TempHoldBrush = (HBRUSH)SelectObject(hDC, TempShadeBrush);
	//Ellipse
	//(
	//	hDC,
	//	this->m_tInfo.vPos.x - static_cast<int>(200.0f * 0.63f),
	//	(this->m_tInfo.vPos.y + 80) * 0.5f - 50,
	//	this->m_tInfo.vPos.x + static_cast<int>(200.0f * 0.63f),
	//	(this->m_tInfo.vPos.y + 80) * 0.5f + 50
	//);
	//DeleteObject(SelectObject(hDC, TempHoldBrush));
}

void CUpCase::Release()
{
}

void CUpCase::Cal_WorldMatrix()
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
	D3DXMATRIX TempMatrixToWorld_Size_Shade = {};
	D3DXMATRIX TempMatrixToWorld_Rotation_Shade = {};
	D3DXMATRIX TempMatrixToWorld_Location_Shade = {};
	D3DXMatrixScaling(&TempMatrixToWorld_Size_Shade, 1.0f, 0.40f, 1.0f);
	D3DXMatrixTranslation(&TempMatrixToWorld_Location_Shade, m_tInfo.vPos.x, m_tInfo.vPos.y + 175, m_tInfo.vPos.z);
	D3DXMatrixRotationZ(&TempMatrixToWorld_Rotation_Shade, D3DXToRadian(m_fAngle));
	D3DXMATRIX TempMatrixToWorld_Shade = {};
	TempMatrixToWorld_Shade = TempMatrixToWorld_Rotation_Shade * TempMatrixToWorld_Location_Shade;
	for (int i = 0; i < static_cast<int>(m_LocalPointVector.size()); ++i)
	{
		D3DXVec3TransformCoord(&m_WorldShadeVector[i], &m_LocalShadeVector[i], &TempMatrixToWorld_Shade);
	}
	for (int i = 0; i < static_cast<int>(m_LocalPointVector.size()); ++i)
	{
		D3DXVec3TransformCoord(&m_WorldShadeVector[i], &m_WorldShadeVector[i], &TempMatrixToWorld_Size_Shade);
	}
}
