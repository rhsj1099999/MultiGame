#include "stdafx.h"
#include "FishingRod.h"

#include "KeyMgr.h"
#include "UIMgr.h"

CFishingRod::CFishingRod()
    : m_fSpeed(0.f)
    , m_fBarAngle(55.f)
    , m_fReelAngle(0.f)
    , m_iLengthFishingLine(0)
    , m_bBaitActivated(true)
{
    ZeroMemory(&m_tCollisionRectBait, sizeof(m_tCollisionRectBait));
}

CFishingRod::~CFishingRod()
{
    Release();
}

void CFishingRod::Initialize(void)
{
    // ����Ʈ ���� ������ (0, 0, 0)���� �������� ��, �̵��� ��ǥ(= ����Ʈ ���� ��ǥ ����)
    m_tInfo.vPos = { 100.f, 100.f, 0.f };
    // ���ô� ���� ����
    m_tInfo.vDir = { 1.0f, 0.f, 0.f };
    
    // ����Ʈ ���� ������ (0, 0, 0)���� �������� ��, ������� ����� ���� ��Ʈ ��ǥ
    m_vLocalBar[0] = { -5.f, -120.f, 0.f };
    m_vLocalBar[1] = { 5.f, -120.f, 0.f };
    m_vLocalBar[2] = { 5.f, 50.f, 0.f };
    m_vLocalBar[3] = { -5.f, 50.f, 0.f };

    // ����Ʈ ���� ������ (0, 0, 0)���� �������� ��, ������� ����Ʈ ���� ��Ʈ ��ǥ
    m_vLocalReelSeat[0] = { -15.f, -15.f, 0.f };
    m_vLocalReelSeat[1] = { 15.f, -15.f, 0.f };
    m_vLocalReelSeat[2] = { 15.f, 15.f, 0.f };
    m_vLocalReelSeat[3] = { -15.f, 15.f, 0.f };

    // ����Ʈ ���� ������ (0, 0, 0)���� �������� ��, ������� �� ���� ���� ��ǥ
    m_vLocalReel = { 40.f, 0.f, 0.f };

    // ���ô� �¿� �̵� ���ǵ� ����
    m_fSpeed = 3.f;
}

int CFishingRod::Update(void)
{
    Key_Input();

    D3DXMATRIX matScale; // ũ�� ��ȯ ���(ũ)
    D3DXMATRIX matRotZ;  // ���� ��ȯ ���(��)
    D3DXMATRIX matTrans; // �̵� ��ȯ ���(��)

    // ����� ��Ʈ �� ���� �̵� �� ȸ�� �ڵ�
    // �̵� �� ��ŭ �̵� ��ȯ ��Ŀ� ����
    D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);
    // ȸ�� �� ��ŭ ���� ��ȯ ��Ŀ� ����
    D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fBarAngle));
    
    // ���� ��� = ũ�� ��ȯ ��� X ���� ��ȯ ��� X �̵� ��ȯ ��� X ���� ��ȯ ��� X �θ� ��ȯ ���
    m_tInfo.matWorld = matRotZ * matTrans;
    
    // ����� ��Ʈ �� ������ ���� ���� ���ͷ� ��ȯ
    for (int i = 0; i < 4; ++i)
        D3DXVec3TransformCoord(&m_vWorldBar[i], &m_vLocalBar[i], &m_tInfo.matWorld);



    // ����Ʈ ��Ʈ �� ���� �̵�, �ڵ�
    // ���� ���� ��� �׵���ķ� �ʱ�ȭ
    D3DXMatrixIdentity(&m_tInfo.matWorld);

    // ���� ��� = ũ�� ��ȯ ��� X ���� ��ȯ ��� X �̵� ��ȯ ��� X ���� ��ȯ ��� X �θ� ��ȯ ���
    m_tInfo.matWorld = matTrans;

    // ����Ʈ ��Ʈ �� ������ ���� ���� ���ͷ� ��ȯ
    for (int i = 0; i < 4; ++i)
        D3DXVec3TransformCoord(&m_vWorldReelSeat[i], &m_vLocalReelSeat[i], &m_tInfo.matWorld);

    

    // �� ���� �̵� �ڵ�
    // ���� ���� ��� �׵���ķ� �ʱ�ȭ
    D3DXMatrixIdentity(&m_tInfo.matWorld);

    // �� ���� ȸ�� ���� ��ŭ ���� ��Ŀ� ����
    D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fReelAngle));

    // ���� ��� = ũ�� ��ȯ ��� x ���� ��ȯ ��� x �̵� ��ȯ ��� x ���� ��ȯ ��� x �θ� ��ȯ ���
    m_tInfo.matWorld = matRotZ * matTrans;

    // �� ������ ���� ���� ���ͷ� ��ȯ
    D3DXVec3TransformCoord(&m_vWorldReel, &m_vLocalReel, &m_tInfo.matWorld);

    
    // ������ ���� ���� ��ǥ ����
    m_vWorldFishingLine = { m_vWorldBar[1].x, m_vWorldBar[1].y + m_iLengthFishingLine, 0.f };

    // �̳� ��Ʈ ���� ��ǥ ����
    m_vWorldBait[0] = { m_vWorldFishingLine.x - 8.f, m_vWorldFishingLine.y - 8.f, 0.f };
    m_vWorldBait[1] = { m_vWorldFishingLine.x + 8.f, m_vWorldFishingLine.y - 8.f, 0.f };
    m_vWorldBait[2] = { m_vWorldFishingLine.x + 8.f, m_vWorldFishingLine.y + 8.f, 0.f };
    m_vWorldBait[3] = { m_vWorldFishingLine.x - 8.f, m_vWorldFishingLine.y + 8.f, 0.f };

    // �̳� �浹 ��Ʈ ������Ʈ
    Update_CollisionRectBait();

    return 0;
}

void CFishingRod::Late_Update(void)
{
    // ������ ���̰� 0�϶� �̳��� ��Ȱ��ȭ ���¸� Ȱ��ȭ ��Ų��.
    if (0 >= m_iLengthFishingLine)
    {
        // P ������
        if (CKeyMgr::Get_Instance()->Key_Down('P'))
        {
            if (!m_bBaitActivated)
            {
                if (100 <= CUIMgr::Get_Instance()->Get_Money())
                {
                    CUIMgr::Get_Instance()->Add_Money(-100);
                    m_bBaitActivated = true;
                }
            }
        }
    }
}

void CFishingRod::Render(HDC hDC)
{
    // ����� ��Ʈ ����
    MoveToEx(hDC, (int)m_vWorldBar[0].x, (int)m_vWorldBar[0].y, nullptr);
    for (int i = 1; i <= 3; ++i)
        LineTo(hDC, (int)m_vWorldBar[i].x, (int)m_vWorldBar[i].y);
    LineTo(hDC, (int)m_vWorldBar[0].x, (int)m_vWorldBar[0].y);

    // ����Ʈ ��Ʈ ����
    MoveToEx(hDC, (int)m_vWorldReelSeat[0].x, (int)m_vWorldReelSeat[0].y, nullptr);
    for (int i = 1; i <= 3; ++i)
        LineTo(hDC, (int)m_vWorldReelSeat[i].x, (int)m_vWorldReelSeat[i].y);
    LineTo(hDC, (int)m_vWorldReelSeat[0].x, (int)m_vWorldReelSeat[0].y);

    // �� ����
    Ellipse(hDC, int(m_vWorldReel.x - 10.f), int(m_vWorldReel.y - 10.f),
        int(m_vWorldReel.x + 10.f), int(m_vWorldReel.y + 10.f));
    // �� ������ ����
    MoveToEx(hDC, (int)m_tInfo.vPos.x, (int)m_tInfo.vPos.y, nullptr);
    LineTo(hDC, (int)m_vWorldReel.x, (int)m_vWorldReel.y);

    // ������ ����
    MoveToEx(hDC, (int)m_vWorldBar[1].x, (int)m_vWorldBar[1].y, nullptr);
    LineTo(hDC, (int)m_vWorldFishingLine.x, (int)m_vWorldFishingLine.y);

    // �̳� ��Ʈ ����
    // �̳��� Ȱ��ȭ �������� ����
    if (m_bBaitActivated)
    {
        MoveToEx(hDC, (int)m_vWorldBait[0].x, (int)m_vWorldBait[0].y, nullptr);
        for (int i = 1; i <= 3; ++i)
            LineTo(hDC, (int)m_vWorldBait[i].x, (int)m_vWorldBait[i].y);
        LineTo(hDC, (int)m_vWorldBait[0].x, (int)m_vWorldBait[0].y);
    }


    //// ��� �� ����
    //HPEN hGreenPen = CreatePen(PS_SOLID, 1, RGB(129, 193, 71));
    //HPEN hOldPen = (HPEN)SelectObject(hDC, hGreenPen); // ����� ���ÿ� ������ ���

    //// ���� �귯�� ����
    //HBRUSH hHollowBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    //HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hHollowBrush); // ����� ���ÿ� ������ ���

    //// �浹ü ��Ʈ ����
    //Rectangle(hDC, m_tCollisionRectBait.left, m_tCollisionRectBait.top,
    //    m_tCollisionRectBait.right, m_tCollisionRectBait.bottom);

    //// ���̶� �귯�� ���� ���� �� ������ ������ �����ֱ�
    //SelectObject(hDC, hOldPen);
    //SelectObject(hDC, hOldBrush);
    //// ���� ������ �游 ����
    //DeleteObject(hGreenPen);

    //// ������ ���� ��ǥ ���
    //TCHAR      szBuff1[64] = L"";
    //swprintf_s(szBuff1, L"���� ������ ���� x, y : %f, %f", m_vWorldFishingLine.x, m_vWorldFishingLine.y);
    //TextOut(hDC, 50, 200, szBuff1, lstrlen(szBuff1));
}

void CFishingRod::Release(void)
{

}

void CFishingRod::OnTrigger()
{
    
}

void CFishingRod::Cal_WorldMatrix()
{

}

void CFishingRod::Key_Input()
{
    // �� ����, �� �������� ����
    if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT))
    {
        // ������ ���̰� 0���� �۾����� 0���� ����, �� ����
        if (m_iLengthFishingLine <= 0)
        {
            m_fReelAngle = m_fReelAngle;
            m_iLengthFishingLine = 0;
        }
        else
        {
            m_fReelAngle -= 15.f;
            m_iLengthFishingLine -= 2;
        }
    }

    // �� Ǯ��, �� �ٱ������� ����
    else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
    {
        m_fReelAngle += 15.f;
        m_iLengthFishingLine += 2;
    }

    // ���ô� �·� �̵��ϱ�
    else if (CKeyMgr::Get_Instance()->Key_Pressing('A'))
    {
        m_tInfo.vPos += m_tInfo.vDir * -m_fSpeed;
    }

    // ���ô� ��� �̵��ϱ�
    else if (CKeyMgr::Get_Instance()->Key_Pressing('D'))
    {
        m_tInfo.vPos += m_tInfo.vDir * m_fSpeed;
    }


}

void CFishingRod::Update_CollisionRectBait()
{
    m_tCollisionRectBait.left = LONG(m_vWorldFishingLine.x - 8.f);
    m_tCollisionRectBait.top = LONG(m_vWorldFishingLine.y - 8.f);
    m_tCollisionRectBait.right = LONG(m_vWorldFishingLine.x + 8.f);
    m_tCollisionRectBait.bottom = LONG(m_vWorldFishingLine.y + 8.f);
}
