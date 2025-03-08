#include "stdafx.h"
#include "CaseHoles.h"
#include "UpCase.h"
#include "KeyMgr.h"
#include "UIMgr.h"
//#include "Cursor.h"
#include "CServerManager.h"


CCaseHoles::CCaseHoles()
{
	

}

void CCaseHoles::Initialize()
{
	this->m_eObjID = OBJ_CASEHOLES;
	this->m_pMakeMePtr = nullptr;
	this->m_fAngleSpeed = 3.0f;
	this->m_fAngle = 0.0f;
	this->m_tInfo.vPos = { 0.0f,0.0f,0.0f };
	this->LocalHoleLT = { this->m_tInfo.vPos.x + (this->m_fCaseDiameter / 2 * cosf(D3DXToRadian(this->m_fAngle))),this->m_tInfo.vPos.y + (this->m_fCaseDiameter / 2 * sinf(D3DXToRadian(this->m_fAngle))),0.0f };
	this->LocalHoleRT = { this->m_tInfo.vPos.x + (this->m_fCaseDiameter / 2 * cosf(D3DXToRadian(this->m_fAngle - 5.0f))),this->m_tInfo.vPos.y + (this->m_fCaseDiameter / 2 * sinf(D3DXToRadian(this->m_fAngle - 5.0f))),0.0f };

	this->m_vLocalBladeStart = { this->m_tInfo.vPos.x + (this->m_fCaseDiameter / 2 * cosf(D3DXToRadian(this->m_fAngle - 2.5f))),this->m_tInfo.vPos.y + (this->m_fCaseDiameter / 2 * sinf(D3DXToRadian(this->m_fAngle - 2.5f))),0.0f };
	this->m_vLocalBladeEnd = { this->m_tInfo.vPos.x + ((this->m_fCaseDiameter + this->BladeLength * 3) / 2 * cosf(D3DXToRadian(this->m_fAngle - 2.5f))),this->m_tInfo.vPos.y + ((this->m_fCaseDiameter + this->BladeLength*3) / 2 * sinf(D3DXToRadian(this->m_fAngle - 2.5f))),0.0f };
	this->m_vLocalBladeGuard = { this->m_tInfo.vPos.x + ((this->m_fCaseDiameter + this->BladeLength * 2) / 2 * cosf(D3DXToRadian(this->m_fAngle - 2.5f))),this->m_tInfo.vPos.y + ((this->m_fCaseDiameter + this->BladeLength * 2) / 2 * sinf(D3DXToRadian(this->m_fAngle - 2.5f))),0.0f };
}


int CCaseHoles::Update()
{
	if (m_bIsServerMode == true)
	{
		m_fAngle = CServerManager::Get_Instance()->GetCurrentAngle();
		m_fAngle += m_fOffsetAngle_Server;
		if (this->m_fAngle > 360.0f)
		{
			this->m_fAngle -= 360.0f;
		}
		if (this->m_fAngle < 0.0f)
		{
			this->m_fAngle += 360.0f;
		}

		if (this->m_fAngle > 5.0f && this->m_fAngle < 180.0f)
		{
			this->m_bIsOn = true;
		}
		else
		{
			this->m_bIsOn = false;
		}
		Cal_WorldMatrix();

		if (CServerManager::Get_Instance()->GetMyTurn() == true)
		{
#pragma region MyTurn
			if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON))
			{
				GetCursorPos(&m_MouseClickedPoint);
				ScreenToClient(g_hWnd, &m_MouseClickedPoint);
				if (PtInRect(&this->m_InsertArea, m_MouseClickedPoint) == true)
				{
					bool bCanInsert = CServerManager::Get_Instance()->GetIsCanInsert(m_iHoleIndex);

					if (this->m_bIsOn == false || this->m_bIsInserted == true || bCanInsert == false)
						return 0;

					this->m_bIsInserted = true;

					CServerManager::Get_Instance()->SetInsert(m_iHoleIndex);

					CServerManager::Get_Instance()->SetMyTurn(false);

					m_iColorIndex = CServerManager::Get_Instance()->GetRoomDesc().MyNumber;

					ClientSession* pSession = CServerManager::Get_Instance()->GetSession();

					PAK_BLADEINSERT TempData = {};

					TempData.RoomSessionDesc = CServerManager::Get_Instance()->GetRoomDesc();

					TempData.Index = m_iHoleIndex;

					MySend<PAK_BLADEINSERT>(pSession, TempData, PacketHeader::PacketType::PLAYERBLADEINSERTED);
				}
			}
#pragma endregion MyTurn
		}
	}
	else
	{
#pragma region NoServerMode
		if (CKeyMgr::Get_Instance()->Key_Pressing('Q'))
		{
			this->m_fAngle += this->m_fAngleSpeed;

			if (this->m_fAngle > 360.0f)
			{
				this->m_fAngle = 0.0f;
			}
		}
		else
		{
			if (CKeyMgr::Get_Instance()->Key_Pressing('E'))
			{
				this->m_fAngle -= this->m_fAngleSpeed;
				if (this->m_fAngle < 0.0f)
				{
					this->m_fAngle = 360.0f;
				}
			}
		}
		if (this->m_fAngle > 5.0f && this->m_fAngle < 180.0f)
		{
			this->m_bIsOn = true;
		}
		else
		{
			this->m_bIsOn = false;
		}
		Cal_WorldMatrix();
		if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LBUTTON))
		{
			GetCursorPos(&m_MouseClickedPoint);
			ScreenToClient(g_hWnd, &m_MouseClickedPoint);
			if (PtInRect(&this->m_InsertArea, m_MouseClickedPoint) == true)
			{
				if (this->m_bIsOn == true)
				{
					if (this->m_bIsInserted == false)
					{
						if (CUIMgr::Get_Instance()->Get_Money() >= 50)
						{
							this->m_bIsInserted = true;
							CUIMgr::Get_Instance()->Add_Money(-50);
							JudgeReward();
						}
					}
				}
			}
		}
#pragma endregion NoServerMode
	}
	return 0;
}

void CCaseHoles::AftInit(CObj* parmObjPtr, const float& parmAngle, const float& parmYOffset)
{
	m_pMakeMePtr = parmObjPtr;
	m_fAngle = parmAngle; //aft
	m_fOffsetAngle_Server = parmAngle;
	m_tInfo.vPos = m_pMakeMePtr->Get_Info().vPos; //aft
	m_tInfo.vPos.y += parmYOffset; //aft
}

void CCaseHoles::Late_Update()
{


}

void CCaseHoles::Render(HDC hDC)
{
	if (this->m_bIsOn == true)
	{
		MoveToEx
		(
			hDC,
			WorldHoleLT.x,
			WorldHoleLT.y,
			nullptr
		);
		LineTo
		(
			hDC,
			WorldHoleRT.x,
			WorldHoleRT.y
		);
		LineTo
		(
			hDC,
			WorldHoleRB.x,
			WorldHoleRB.y
		);
		LineTo
		(
			hDC,
			WorldHoleLB.x,
			WorldHoleLB.y
		);
		LineTo
		(
			hDC,
			WorldHoleLT.x,
			WorldHoleLT.y
		);
		if (this->m_bIsInserted == true)
		{
			HPEN TempRedPen = (m_bIsServerMode == true) ? CreatePen(1, 5, m_Colors[m_iColorIndex]) : CreatePen(1, 5, RGB(255,0,0));
			HPEN TempHoldPen = (HPEN)SelectObject(hDC, TempRedPen);
			MoveToEx
			(
				hDC,
				m_vWorldBladeStart.x,
				m_vWorldBladeStart.y,
				nullptr
			);
			LineTo
			(
				hDC,
				m_vWorldBladeEnd.x,
				m_vWorldBladeEnd.y
			);
			MoveToEx
			(
				hDC,
				m_vWorldBladeGuard.x,
				m_vWorldBladeGuard.y + 15,
				nullptr
			);
			LineTo
			(
				hDC,
				m_vWorldBladeGuard.x,
				m_vWorldBladeGuard.y - 15
			);
			DeleteObject(SelectObject(hDC, TempHoldPen));
		}
	}
}

void CCaseHoles::Release()
{

}

void CCaseHoles::Cal_WorldMatrix()
{
	D3DXMATRIX TempMatrixToWorld_Size = {};
	D3DXMATRIX TempMatrixToWorld_Rotation = {};
	D3DXMATRIX TempMatrixToWorld_Location = {};
	D3DXMATRIX TempMatrixToWorld = {};

	D3DXMatrixScaling
	(
		&TempMatrixToWorld_Size,
		1.0f,
		0.5f,
		1.0f
	);
	D3DXMatrixTranslation
	(
		&TempMatrixToWorld_Location,
		this->m_tInfo.vPos.x,
		this->m_tInfo.vPos.y,
		this->m_tInfo.vPos.z
	);
	D3DXMatrixRotationZ
	(
		&TempMatrixToWorld_Rotation,
		D3DXToRadian(this->m_fAngle)
	);


	TempMatrixToWorld = TempMatrixToWorld_Rotation * TempMatrixToWorld_Location;

	D3DXVec3TransformCoord(&this->WorldHoleLT, &this->LocalHoleLT, &TempMatrixToWorld);
	D3DXVec3TransformCoord(&this->WorldHoleRT, &this->LocalHoleRT, &TempMatrixToWorld);
	D3DXVec3TransformCoord(&this->WorldHoleLT, &this->WorldHoleLT, &TempMatrixToWorld_Size);
	D3DXVec3TransformCoord(&this->WorldHoleRT, &this->WorldHoleRT, &TempMatrixToWorld_Size);
	this->WorldHoleLB = { this->WorldHoleLT.x, this->WorldHoleLT.y + this->m_fHoleYLength, 0.0f };
	this->WorldHoleRB = { this->WorldHoleRT.x, this->WorldHoleRT.y + this->m_fHoleYLength, 0.0f };
	this->m_InsertArea =
	{
		static_cast<int>(WorldHoleLT.x),
		static_cast<int>(WorldHoleLT.y),
		static_cast<int>(WorldHoleRB.x),
		static_cast<int>(WorldHoleRB.y)
	};


	D3DXMATRIX TempMatrixToWorld_Size_Blade = {};
	D3DXMATRIX TempMatrixToWorld_Rotation_Blade = {};
	D3DXMATRIX TempMatrixToWorld_Location_Blade = {};
	D3DXMATRIX TempMatrixToWorld_Blade = {};
	D3DXMatrixScaling(&TempMatrixToWorld_Size_Blade, 1.0f, 0.5f, 1.0f);
	D3DXMatrixTranslation(&TempMatrixToWorld_Location_Blade, this->m_tInfo.vPos.x, this->m_tInfo.vPos.y + this->m_fHoleYLength, this->m_tInfo.vPos.z);
	D3DXMatrixRotationZ(&TempMatrixToWorld_Rotation_Blade, D3DXToRadian(this->m_fAngle));
	TempMatrixToWorld_Blade = TempMatrixToWorld_Rotation_Blade * TempMatrixToWorld_Location_Blade;
	D3DXVec3TransformCoord(&this->m_vWorldBladeStart, &this->m_vLocalBladeStart, &TempMatrixToWorld_Blade);
	D3DXVec3TransformCoord(&this->m_vWorldBladeEnd, &this->m_vLocalBladeEnd, &TempMatrixToWorld_Blade);
	D3DXVec3TransformCoord(&this->m_vWorldBladeGuard, &this->m_vLocalBladeGuard, &TempMatrixToWorld_Blade);
	D3DXVec3TransformCoord(&this->m_vWorldBladeStart, &this->m_vWorldBladeStart, &TempMatrixToWorld_Size_Blade);
	D3DXVec3TransformCoord(&this->m_vWorldBladeEnd, &this->m_vWorldBladeEnd, &TempMatrixToWorld_Size_Blade);
	D3DXVec3TransformCoord(&this->m_vWorldBladeGuard, &this->m_vWorldBladeGuard, &TempMatrixToWorld_Size_Blade);
}

void CCaseHoles::SetAngle(const float& parmAngle)
{
	this->m_fAngle = parmAngle;
}



void CCaseHoles::SetReward(const REWARD& parmRewards)
{
	this->m_eReward = parmRewards;
}

void CCaseHoles::JudgeReward()
{
	switch (this->m_eReward)
	{
	default:
		break;
	case REWARD::RWD_NONE:
		break;
	case REWARD::RWD_LOSEALLMONEY:
	{
		CUIMgr::Get_Instance()->LoseAllMoney();
		CUpCase* TempCastPtr = dynamic_cast<CUpCase*>(this->m_pMakeMePtr);
	}
		break;
	case REWARD::RWD_GETMONEY:
	{
		CUIMgr::Get_Instance()->Add_Money(5000);
		CUpCase* TempCastPtr = dynamic_cast<CUpCase*>(this->m_pMakeMePtr);
		TempCastPtr->SetIsStageEnd(true);
	}
		break;
	}
}
