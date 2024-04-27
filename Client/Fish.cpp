#include "stdafx.h"
#include "Fish.h"

CFish::CFish()
    : m_iDir(1)
    //, m_fSpeed(0)
{
    ZeroMemory(&m_tCollisionRect, sizeof(m_tCollisionRect));
}

CFish::~CFish()
{
    Release();
}

void CFish::Initialize(void)
{
    // ����� ������ (0, 0, 0)���� ���� ��, �̵��� ��ǥ(= ����� ���� ���� ��ǥ)
    m_tInfo.vPos = { 500.f, 500.f, 0.f };
    // ����� ���⺤��
    m_tInfo.vDir = { 1.0f, 0.f, 0.f };

    // ����� ������ (0, 0, 0)���� ���� ��, ������� �� ����� ���� ��ǥ
    m_vLocalFish[0] = { 0.f, 0.f, 0.f };
    m_vLocalFish[1] = { -15.f, -10.f, 0.f };
    m_vLocalFish[2] = { 0.f, -20.f, 0.f };
    m_vLocalFish[3] = { 50.f, -20.f, 0.f };
    m_vLocalFish[4] = { 55.f, -5.f, 0.f };
    m_vLocalFish[5] = { 60.f, -20.f, 0.f };
    m_vLocalFish[6] = { 60.f, 10.f, 0.f };
    m_vLocalFish[7] = { 55.f, -5.f, 0.f };
    m_vLocalFish[8] = { 50.f, 10.f, 0.f };
    m_vLocalFish[9] = { -10.f, 10.f, 0.f };
}

int CFish::Update(void)
{
    if (m_bDead)
        return OBJ_DEAD;

    D3DXMATRIX matScale; // ũ�� ��ȯ ���(ũ)
    D3DXMATRIX matRotZ;  // ���� ��ȯ ���(��)
    D3DXMATRIX matTrans; // �̵� ��ȯ ���(��)

    // ũ�� ��ȯ ��� �׵� ��ķ�
    D3DXMatrixIdentity(&matScale);
    
    // ���� ������ �������̸�
    if (1 == m_iDir)
    {
        // �����´�
        D3DXMatrixScaling(&matScale, -1.f, 1.f, 1.f);
    }
    // ���� ������ �����̸�
    else if (-1 == m_iDir)
    {
        // ũ�� �״��
        D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
    }

    // ����� �� ���� �̵� �ڵ�
    // �̵� �� ��ŭ �̵� ��ȯ ��Ŀ� ����
    D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);

    // ���� ��� = ũ�� ��ȯ ��� X ���� ��ȯ ��� X �̵� ��ȯ ��� X ���� ��ȯ ��� X �θ� ��ȯ ���
    m_tInfo.matWorld = matScale * matTrans;

    // ����� �� ������ ���� ���� ���ͷ� ��ȯ
    for (int i = 0; i < 10; ++i)
        D3DXVec3TransformCoord(&m_vWorldFish[i], &m_vLocalFish[i], &m_tInfo.matWorld);

    Update_CollisionRect();         // �浹ü ��Ʈ ������Ʈ
    Move();                         // ����� ������

    return OBJ_NOEVENT;
}

void CFish::Late_Update(void)
{
    // ���� ���� �Ѿ�� ���� �ݴ��
    // ���� �ٱ����� ������ ��
    if (m_vWorldFish[1].x < 0)
    {
        m_iDir *= -1;       // ���� ���� �ݴ��
        m_tInfo.vPos.x += -m_vWorldFish[1].x + 40.f; // �������� ���� �Ÿ� + 40 ��ŭ ���������� ������
    }
    // ������ �ٱ����� ������ ��
    else if (m_vWorldFish[1].x > WINCX)
    {
        m_iDir *= -1;       // ���� ���� �ݴ��
        m_tInfo.vPos.x -= m_vWorldFish[1].x - (float)WINCX + 40.f; // ���������� ���� �Ÿ� + 40 ��ŭ �������� ������
    }
}

void CFish::Render(HDC hDC)
{
    // ����� ����
    MoveToEx(hDC, m_vWorldFish[0].x, m_vWorldFish[0].y, nullptr);
    for (int i = 1; i <= 9; ++i)
        LineTo(hDC, m_vWorldFish[i].x, m_vWorldFish[i].y);
    LineTo(hDC, m_vWorldFish[0].x, m_vWorldFish[0].y);

    //// ��� �� ����
    //HPEN hGreenPen = CreatePen(PS_SOLID, 1, RGB(129, 193, 71));
    //HPEN hOldPen = (HPEN)SelectObject(hDC, hGreenPen); // ����� ���ÿ� ������ ���

    //// ���� �귯�� ����
    //HBRUSH hHollowBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    //HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hHollowBrush); // ����� ���ÿ� ������ ���

    //// �浹ü ��Ʈ ����
    //Rectangle(hDC, m_tCollisionRect.left, m_tCollisionRect.top,
    //      m_tCollisionRect.right, m_tCollisionRect.bottom);

    //// ���̶� �귯�� ���� ���� �� ������ ������ �����ֱ�
    //SelectObject(hDC, hOldPen);
    //SelectObject(hDC, hOldBrush);
    //// ���� ������ �游 ����
    //DeleteObject(hGreenPen);
}

void CFish::Release(void)
{
}

void CFish::OnTrigger()
{
    // ����Ⱑ ���ô� �̳��� �ε����� ��

    // �÷��̾ 
}

void CFish::Cal_WorldMatrix()
{

}

void CFish::Move()
{
    m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * m_iDir;
}

void CFish::Update_CollisionRect()
{
    m_tCollisionRect.left = LONG(m_vWorldFish[0].x - 8.f);
    m_tCollisionRect.top = LONG(m_vWorldFish[0].y - 8.f);
    m_tCollisionRect.right = LONG(m_vWorldFish[0].x + 8.f);
    m_tCollisionRect.bottom = LONG(m_vWorldFish[0].y + 8.f);
}
