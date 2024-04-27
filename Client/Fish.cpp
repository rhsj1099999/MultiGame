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
    // 물고기 중점을 (0, 0, 0)으로 봤을 때, 이동할 좌표(= 물고기 중점 월드 좌표)
    m_tInfo.vPos = { 500.f, 500.f, 0.f };
    // 물고기 방향벡터
    m_tInfo.vDir = { 1.0f, 0.f, 0.f };

    // 물고기 중점을 (0, 0, 0)으로 봤을 때, 상대적인 각 물고기 부위 좌표
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

    D3DXMATRIX matScale; // 크기 변환 행렬(크)
    D3DXMATRIX matRotZ;  // 자전 변환 행렬(자)
    D3DXMATRIX matTrans; // 이동 변환 행렬(이)

    // 크기 변환 행렬 항등 행렬로
    D3DXMatrixIdentity(&matScale);
    
    // 진행 방향이 오른쪽이면
    if (1 == m_iDir)
    {
        // 뒤집는다
        D3DXMatrixScaling(&matScale, -1.f, 1.f, 1.f);
    }
    // 진행 방향이 왼쪽이면
    else if (-1 == m_iDir)
    {
        // 크기 그대로
        D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
    }

    // 물고기 각 점들 이동 코드
    // 이동 값 만큼 이동 변환 행렬에 적용
    D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);

    // 월드 행렬 = 크기 변환 행렬 X 자전 변환 행렬 X 이동 변환 행렬 X 공전 변환 행렬 X 부모 변환 행렬
    m_tInfo.matWorld = matScale * matTrans;

    // 물고기 각 점들을 월드 공간 벡터로 변환
    for (int i = 0; i < 10; ++i)
        D3DXVec3TransformCoord(&m_vWorldFish[i], &m_vLocalFish[i], &m_tInfo.matWorld);

    Update_CollisionRect();         // 충돌체 렉트 업데이트
    Move();                         // 물고기 움직임

    return OBJ_NOEVENT;
}

void CFish::Late_Update(void)
{
    // 보더 범위 넘어가면 방향 반대로
    // 왼쪽 바깥으로 나갔을 때
    if (m_vWorldFish[1].x < 0)
    {
        m_iDir *= -1;       // 진행 방향 반대로
        m_tInfo.vPos.x += -m_vWorldFish[1].x + 40.f; // 왼쪽으로 나간 거리 + 40 만큼 오른쪽으로 땡겨줌
    }
    // 오른쪽 바깥으로 나갔을 때
    else if (m_vWorldFish[1].x > WINCX)
    {
        m_iDir *= -1;       // 진행 방향 반대로
        m_tInfo.vPos.x -= m_vWorldFish[1].x - (float)WINCX + 40.f; // 오른쪽으로 나간 거리 + 40 만큼 왼쪽으로 땡겨줌
    }
}

void CFish::Render(HDC hDC)
{
    // 물고기 렌더
    MoveToEx(hDC, m_vWorldFish[0].x, m_vWorldFish[0].y, nullptr);
    for (int i = 1; i <= 9; ++i)
        LineTo(hDC, m_vWorldFish[i].x, m_vWorldFish[i].y);
    LineTo(hDC, m_vWorldFish[0].x, m_vWorldFish[0].y);

    //// 녹색 펜 생성
    //HPEN hGreenPen = CreatePen(PS_SOLID, 1, RGB(129, 193, 71));
    //HPEN hOldPen = (HPEN)SelectObject(hDC, hGreenPen); // 적용과 동시에 기존거 백업

    //// 투명 브러쉬 생성
    //HBRUSH hHollowBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    //HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hHollowBrush); // 적용과 동시에 기존거 백업

    //// 충돌체 렉트 렌더
    //Rectangle(hDC, m_tCollisionRect.left, m_tCollisionRect.top,
    //      m_tCollisionRect.right, m_tCollisionRect.bottom);

    //// 펜이랑 브러쉬 새로 만들어서 다 썼으면 기존꺼 돌려주기
    //SelectObject(hDC, hOldPen);
    //SelectObject(hDC, hOldBrush);
    //// 직접 생성한 펜만 삭제
    //DeleteObject(hGreenPen);
}

void CFish::Release(void)
{
}

void CFish::OnTrigger()
{
    // 물고기가 낚시대 미끼랑 부딪혔을 때

    // 플레이어가 
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
