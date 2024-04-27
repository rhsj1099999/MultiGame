#include "stdafx.h"
#include "Roulette.h"
#include "KeyMgr.h"

void CRoulette::Initialize(void)
{
	m_bIsGame = false;
	m_bSpin = false;

	// 삼각형
	m_tInfo.vScale = { 250.f, 250.f, 0.f };
	m_fAngle = D3DXToRadian(18.f * m_iRouletteNumber);
	m_fSpeed = 0.f;
	m_vPoint[0] = { m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f };
	float fX = m_tInfo.vPos.x + (m_tInfo.vScale.x * cos(m_fAngle));
	float fY = m_tInfo.vPos.y - (m_tInfo.vScale.y * sin(m_fAngle));
	m_vPoint[1] = { fX,fY, 0.f };
	fX = m_tInfo.vPos.x + (m_tInfo.vScale.x * cos(m_fAngle +  D3DXToRadian(36.f)));
	fY = m_tInfo.vPos.y - (m_tInfo.vScale.y * sin(m_fAngle +  D3DXToRadian(36.f)));
	m_vPoint[2] = { fX,fY, 0.f };

	// 숫자 
	fX = m_tInfo.vPos.x + (220.f * cos(m_fAngle + D3DXToRadian(10.f)));
	fY = m_tInfo.vPos.y - (220.f * sin(m_fAngle + D3DXToRadian(10.f)));
	m_vNumberPoint[0] = { fX,fY, 0.f };

	fX = m_tInfo.vPos.x + (220.f * cos(m_fAngle + D3DXToRadian(26.f)));
	fY = m_tInfo.vPos.y - (220.f * sin(m_fAngle + D3DXToRadian(26.f)));
	m_vNumberPoint[1] = { fX,fY, 0.f };

	fX = m_tInfo.vPos.x + (170.f * cos(m_fAngle + D3DXToRadian(7.f)));
	fY = m_tInfo.vPos.y - (170.f * sin(m_fAngle + D3DXToRadian(7.f)));
	m_vNumberPoint[2] = { fX,fY, 0.f };

	fX = m_tInfo.vPos.x + (170.f * cos(m_fAngle + D3DXToRadian(29.f)));
	fY = m_tInfo.vPos.y - (170.f * sin(m_fAngle + D3DXToRadian(29.f)));
	m_vNumberPoint[3] = { fX,fY, 0.f };

	fX = m_tInfo.vPos.x + (120.f * cos(m_fAngle + D3DXToRadian(4.f)));
	fY = m_tInfo.vPos.y - (120.f * sin(m_fAngle + D3DXToRadian(4.f)));
	m_vNumberPoint[4] = { fX,fY, 0.f };

	fX = m_tInfo.vPos.x + (120.f * cos(m_fAngle + D3DXToRadian(32.f)));
	fY = m_tInfo.vPos.y - (120.f * sin(m_fAngle + D3DXToRadian(32.f)));
	m_vNumberPoint[5] = { fX,fY, 0.f };

	for (int i = 0; i < 3; ++i)
		m_vOriginPoint[i] = m_vPoint[i];

	for (int i = 0; i < 6; i++)
		m_vNumberOriginPoint[i] = m_vNumberPoint[i];
}

int CRoulette::Update(void)
{
	if (m_vPoint[1].y > m_vPoint[2].y)
	{
		m_tRect = { int(m_vPoint[1].x),
		int(m_vPoint[2].y),
		int(m_vPoint[2].x),
		int(m_vPoint[1].y) };
	}
	else
	{
		m_tRect = { int(m_vPoint[1].x),
		int(m_vPoint[1].y),
		int(m_vPoint[2].x),
		int(m_vPoint[2].y) };
	}
	if (m_fSpeed < 0)
	{
		if(m_bSpin)
 			m_bSpin = false;
		m_fSpeed = 0;
	}
	else if (m_fSpeed >= 0)
	{
		m_fAngle += D3DXToRadian(m_fSpeed);
		m_fSpeed -= 0.017;
	}

	Cal_WorldMatrix();

	return 0;
}

void CRoulette::Late_Update(void)
{
}

void CRoulette::Render(HDC hDC)
{
	/*Rectangle(hDC,
		int(m_vPoint[1].x),
		int(m_vPoint[1].y),
		int(m_vPoint[2].x),
		int(m_vPoint[2].y)
	);*/

	MoveToEx(hDC, (int)m_vPoint[0].x, (int)m_vPoint[0].y, nullptr);
	for (int i = 0; i < 3; ++i)
	{
		LineTo(hDC, (int)m_vPoint[i].x, (int)m_vPoint[i].y);
	}
	LineTo(hDC, (int)m_vPoint[0].x, (int)m_vPoint[0].y);

	// 숫자
	if (m_iRouletteNumber == 0)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[4].x, (int)m_vNumberPoint[4].y);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
		LineTo(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y);
		LineTo(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y);
	}
	else if (m_iRouletteNumber == 1)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
	}
	else if (m_iRouletteNumber == 2)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y);
		LineTo(hDC, (int)m_vNumberPoint[3].x, (int)m_vNumberPoint[3].y);
		LineTo(hDC, (int)m_vNumberPoint[2].x, (int)m_vNumberPoint[2].y);
		LineTo(hDC, (int)m_vNumberPoint[4].x, (int)m_vNumberPoint[4].y);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
	}
	else if (m_iRouletteNumber == 3)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y);
		LineTo(hDC, (int)m_vNumberPoint[3].x, (int)m_vNumberPoint[3].y);
		LineTo(hDC, (int)m_vNumberPoint[2].x, (int)m_vNumberPoint[2].y);
		MoveToEx(hDC, (int)m_vNumberPoint[3].x, (int)m_vNumberPoint[3].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
		LineTo(hDC, (int)m_vNumberPoint[4].x, (int)m_vNumberPoint[4].y);
	}
	else if (m_iRouletteNumber == 4)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[2].x, (int)m_vNumberPoint[2].y);
		LineTo(hDC, (int)m_vNumberPoint[3].x, (int)m_vNumberPoint[3].y);
		MoveToEx(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
	}
	else if (m_iRouletteNumber == 5)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y);
		LineTo(hDC, (int)m_vNumberPoint[2].x, (int)m_vNumberPoint[2].y);
		LineTo(hDC, (int)m_vNumberPoint[3].x, (int)m_vNumberPoint[3].y);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
		LineTo(hDC, (int)m_vNumberPoint[4].x, (int)m_vNumberPoint[4].y);
	}
	else if (m_iRouletteNumber == 6)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y);
		LineTo(hDC, (int)m_vNumberPoint[4].x, (int)m_vNumberPoint[4].y);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
		LineTo(hDC, (int)m_vNumberPoint[3].x, (int)m_vNumberPoint[3].y);
		LineTo(hDC, (int)m_vNumberPoint[2].x, (int)m_vNumberPoint[2].y);
	}
	else if (m_iRouletteNumber == 7)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
	}
	else if (m_iRouletteNumber == 8)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[4].x, (int)m_vNumberPoint[4].y);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
		LineTo(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y);
		LineTo(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y);
		MoveToEx(hDC, (int)m_vNumberPoint[2].x, (int)m_vNumberPoint[2].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[3].x, (int)m_vNumberPoint[3].y);
	}
	else if (m_iRouletteNumber == 9)
	{
		MoveToEx(hDC, (int)m_vNumberPoint[3].x, (int)m_vNumberPoint[3].y, nullptr);
		LineTo(hDC, (int)m_vNumberPoint[2].x, (int)m_vNumberPoint[2].y);
		LineTo(hDC, (int)m_vNumberPoint[0].x, (int)m_vNumberPoint[0].y);
		LineTo(hDC, (int)m_vNumberPoint[1].x, (int)m_vNumberPoint[1].y);
		LineTo(hDC, (int)m_vNumberPoint[5].x, (int)m_vNumberPoint[5].y);
		LineTo(hDC, (int)m_vNumberPoint[4].x, (int)m_vNumberPoint[4].y);
	}
}

void CRoulette::Release(void)
{
}

void CRoulette::Cal_WorldMatrix()
{
	D3DXMATRIX	matScale, matRotZ, matTrans;

	D3DXMatrixScaling(&matScale, -1.f, 1.f, 1.f);
	D3DXMatrixRotationZ(&matRotZ, m_fAngle);
	D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);

	m_tInfo.matWorld = matScale * matRotZ * matTrans;

	for (int i = 0; i < 3; ++i)
	{
		m_vPoint[i] = m_vOriginPoint[i];
		m_vPoint[i] -= {m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f};
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_tInfo.matWorld);
	}

	for (int i = 0; i < 6; ++i)
	{
		m_vNumberPoint[i] = m_vNumberOriginPoint[i];
		m_vNumberPoint[i] -= {m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f};
		D3DXVec3TransformCoord(&m_vNumberPoint[i], &m_vNumberPoint[i], &m_tInfo.matWorld);
	}

}
