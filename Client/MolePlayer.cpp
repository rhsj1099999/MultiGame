#include "stdafx.h"
#include "MolePlayer.h"
#include "KeyMgr2.h"
#include "Mole.h"

CMolePlayer::CMolePlayer()
{
	//ZeroMemory(&m_tColliderRect, sizeof(RECT));
}

CMolePlayer::~CMolePlayer()
{
}

void CMolePlayer::Initialize(void)
{
	m_fAngle = 0.f;
	m_tInfo.vPos = { 0.f, 0.f, 0.f };

	m_bAttack = false;
	m_dwAttackCheck = GetTickCount64();

	m_iCombo = 0;
	m_fMoneyScale = 1.f;
	m_dwComboTimer = GetTickCount64();

	m_vPoint[0] = { -25.f, 0.f, 0.f };
	m_vPoint[1] = { -25.f, -75.f, 0.f };
	m_vPoint[2] = { -50.f, -75.f, 0.f };
	m_vPoint[3] = { -50.f, -150.f, 0.f };
	m_vPoint[4] = { 50.f, -150.f, 0.f };
	m_vPoint[5] = { 50.f, -75.f, 0.f };
	m_vPoint[6] = { 25.f, -75.f, 0.f };
	m_vPoint[7] = { 25.f, 0.f, 0.f };
	
	m_vColliderPoint[0] = { -12.5f, 37.5f, 0.f };
	m_vColliderPoint[1] = { -12.5f, -37.5f, 0.f };
	m_vColliderPoint[2] = { 12.5f, -37.5f, 0.f };
	m_vColliderPoint[3] = { 12.5f, 37.5f, 0.f };

	m_tInfo.vLook = { 1.f, 0.f, 0.f };
	m_tInfo.vScale = { 1.f,1.f, 1.f };
	m_tColliderInfo.vScale = { 1.f, 1.f, 1.f };

	m_fSpeed = 5.f;
}

int CMolePlayer::Update(void)
{
	//2초동안 콤보 이어나가지 못할 시 콤보수 초기화
	if (m_dwComboTimer + 1500 < GetTickCount64())
		m_iCombo = 0;

	if (m_iCombo < 10)
		m_fMoneyScale = 1.f;
	else if (m_iCombo >= 10 && m_iCombo < 20)
		m_fMoneyScale = 1.5f;
	else if (m_iCombo >= 20 && m_iCombo < 30)
		m_fMoneyScale = 2.f;
	else if (m_iCombo >= 30 && m_iCombo < 40)
		m_fMoneyScale = 2.5f;
	else if (m_iCombo >= 40 && m_iCombo <= 50)
		m_fMoneyScale = 3.f;

	//망치중점 마우스좌표 받아오기
	m_tInfo.vPos = { ::Get_Mouse().x, ::Get_Mouse().y, ::Get_Mouse().z };

	m_tColliderInfo.vPos = { -37.5f, -112.5f, 0.f };

	Key_Input();

	Cal_WorldMatrix();

    return OBJ_NOEVENT;
}

void CMolePlayer::Late_Update(void)
{
}

void CMolePlayer::Render(HDC hDC)
{

	MoveToEx(hDC, m_PointVec[7].x, m_PointVec[7].y, nullptr);
	for(int i = 0 ; i< 8;++i)
		LineTo(hDC, m_PointVec[i].x, m_PointVec[i].y);
	
	MoveToEx(hDC, m_ColliderPointVec[3].x, m_ColliderPointVec[3].y, nullptr);
	for (int i = 0; i < 4; ++i)
		LineTo(hDC, m_ColliderPointVec[i].x, m_ColliderPointVec[i].y);

	TCHAR szBuff[32];
	wsprintf(szBuff, L"%d 콤보", m_iCombo);
	TextOut(hDC, 20, 80, szBuff, lstrlen(szBuff));

	swprintf_s(szBuff, L"배율 : x %.1f ", m_fMoneyScale);
	TextOut(hDC, 20, 100, szBuff, lstrlen(szBuff));

}

void CMolePlayer::Release(void)
{
}


// 항등 행렬을 만드는 함수(항등 행렬로 만들고자 하는 행렬의 주소)
// D3DXMatrixIdentity(&m_tInfo.matWorld);

// 크기 변환 행렬을 만드는 함수
// D3DXMatrixScaling(&matScale, x배율, y배율, z배율)

// 회전 변환 행렬을 만드는 함수
// D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(각도));

// 위치 변환 행렬을 만드는 함수
// D3DXMatrixTranslation(&matTrans, x좌표, y좌표, z좌표);

// 역행렬을 만드는 함수
// D3DXMatrixInverse(결과 행렬 주소, 0, 원본 행렬 주소)

//크자이공부
// m_tInfo.matWorld = matScale * matRotZ * matTrans;

// 벡터와 행렬의 곱셈을 수행하는 함수

// 결과: 위치 벡터 D3DXVec3TransformCoord(결과 벡터의 주소, 위치 벡터, 행렬)
// 결과: 방향 벡터 D3DXVec3TransformNormal(결과 벡터의 주소, 방향 벡터, 행렬)

void CMolePlayer::Cal_WorldMatrix()
{
	D3DXMATRIX RotZ;
	D3DXMatrixRotationZ(&RotZ, m_fAngle);

	D3DXMATRIX Trans;
	D3DXMatrixTranslation(&Trans, m_tInfo.vPos.x, m_tInfo.vPos.y, m_tInfo.vPos.z);

	//월드행렬은 매 프레임
	m_tInfo.matWorld = RotZ * Trans;

	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_PointVec[i], &m_vPoint[i], &m_tInfo.matWorld);


	D3DXMATRIX ColliderTrans;
	D3DXMatrixTranslation(&ColliderTrans, m_tColliderInfo.vPos.x, m_tColliderInfo.vPos.y, m_tColliderInfo.vPos.z);
	m_tColliderInfo.matWorld = ColliderTrans * m_tInfo.matWorld;

	for (int i = 0; i < 4; i++)
		D3DXVec3TransformCoord(&m_ColliderPointVec[i], &m_vColliderPoint[i], &m_tColliderInfo.matWorld);


}

void CMolePlayer::OnTrigger()
{
}

void CMolePlayer::OnCollision(CObj* _other)
{
	if (dynamic_cast<CMole*>(_other)->Get_Scale() == 1.f)
	{
		m_iCombo++;
		m_dwComboTimer = GetTickCount64();
	}
}

void CMolePlayer::Key_Input(void)
{
	if (CKeyMgr2::Get_Instance()->Get_Key_State(VK_LBUTTON) == KEY_STATE::DOWN)
	{
		m_bAttack = true;
		m_fAngle = (PI / 2) * 3;
	}
	
	if (CKeyMgr2::Get_Instance()->Get_Key_State(VK_LBUTTON) == KEY_STATE::UP)
	{
		m_bAttack = false;
		m_fAngle = 0.f;
	}

}


/// <summary>
/// 충돌체 Get함수
/// </summary>
/// <returns></returns>
RECT CMolePlayer::Get_ColliderRect()
{
	float left = m_ColliderPointVec[0].x;
	float top = m_ColliderPointVec[0].y;
	for (int i = 0; i < 4; ++i)
	{
		if (m_ColliderPointVec[i].x <= left)
			left = m_ColliderPointVec[i].x;
		if (m_ColliderPointVec[i].y <= top)
			top = m_ColliderPointVec[i].y;
	}

	RECT temp = { left, top, left + 75.f ,top + 25.f};
	
	return temp;
}

