#include "stdafx.h"
#include "Arrow.h"
#include "Roulette.h"
#include "ObjMgr.h"

void CArrow::Initialize(void)
{
    m_tInfo.vScale = { 3.f, 100.f, 0.f };
    m_tInfo.vPos.x = 272.f;
    m_tInfo.vPos.y = 45.f;

    m_vPoint[0] = { m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f };
    m_vPoint[1] = { m_tInfo.vPos.x - 7.f, m_tInfo.vPos.y, 0.f };

    m_fAngle = D3DXToRadian(270.f);
    float fX = m_tInfo.vPos.x + (100.f * cos(m_fAngle));
    float fY = m_tInfo.vPos.y - (50.f * sin(m_fAngle));
    m_vPoint[2] = { fX,fY, 0.f };
    m_vPoint[3] = { m_tInfo.vPos.x + 7.f, m_tInfo.vPos.y, 0.f };
    for (int i = 0; i < 4; ++i)
        m_vOriginPoint[i] = m_vPoint[i];

    m_fAngle = D3DXToRadian(0.f);
}

int CArrow::Update(void)
{
    if (m_fAngle > D3DXToRadian(360.f))
    {
        m_fAngle = D3DXToRadian(0.f);
    }
    if (m_fAngle > D3DXToRadian(0.f))
    {
        m_fAngle += D3DXToRadian(3.f);
    }
       
    
    Cal_WorldMatrix();
    return 0;
}

void CArrow::Late_Update(void)
{
}

void CArrow::Render(HDC hDC)
{
    /*RECT temp = this->Get_Rect();
    Rectangle(hDC,
        temp.left,
        temp.top,
        temp.right,
        temp.bottom
    );*/
    MoveToEx(hDC, (int)m_vPoint[0].x, (int)m_vPoint[0].y, nullptr);
    for (int i = 0; i < 4; ++i)
    {
        LineTo(hDC, (int)m_vPoint[i].x, (int)m_vPoint[i].y);
    }
    LineTo(hDC, (int)m_vPoint[0].x, (int)m_vPoint[0].y);
}

void CArrow::Release(void)
{
}

void CArrow::Cal_WorldMatrix()
{
    D3DXMATRIX	matScale, matRotZ, matTrans;

    D3DXMatrixScaling(&matScale, -1.f, 1.f, 1.f);
    D3DXMatrixRotationZ(&matRotZ, m_fAngle);
    D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);
    m_tInfo.matWorld = matScale * matRotZ * matTrans;

    for (int i = 0; i < 4; ++i)
    {
        m_vPoint[i] = m_vOriginPoint[i];
        m_vPoint[i] -= {m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f};
        D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &m_tInfo.matWorld);
    }
}

void CArrow::OnRoulette(CObj* _pObj)
{
    m_iCurNum = dynamic_cast<CRoulette*>(_pObj)->Get_RouletteNumber();
    if (m_iPreNum != m_iCurNum)
    {
        m_fAngle = m_fAngle = D3DXToRadian(300.f);
        m_iPreNum = m_iCurNum;
    }

    if (!dynamic_cast<CRoulette*>(_pObj)->Get_Spin() && dynamic_cast<CRoulette*>(_pObj)->Get_Game())
    {
        dynamic_cast<CRoulette*>(_pObj)->Set_Game(false);
        CObjMgr::Get_Instance()->Roulette_Result();
    }
}
