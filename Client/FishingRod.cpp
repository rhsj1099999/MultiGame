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
    // 릴시트 로컬 중점을 (0, 0, 0)으로 가정했을 때, 이동할 좌표(= 릴시트 월드 좌표 중점)
    m_tInfo.vPos = { 100.f, 100.f, 0.f };
    // 낚시대 방향 벡터
    m_tInfo.vDir = { 1.0f, 0.f, 0.f };
    
    // 릴시트 로컬 중점을 (0, 0, 0)으로 가정했을 때, 상대적인 막대기 로컬 렉트 좌표
    m_vLocalBar[0] = { -5.f, -120.f, 0.f };
    m_vLocalBar[1] = { 5.f, -120.f, 0.f };
    m_vLocalBar[2] = { 5.f, 50.f, 0.f };
    m_vLocalBar[3] = { -5.f, 50.f, 0.f };

    // 릴시트 로컬 중점을 (0, 0, 0)으로 가정했을 때, 상대적인 릴시트 로컬 렉트 좌표
    m_vLocalReelSeat[0] = { -15.f, -15.f, 0.f };
    m_vLocalReelSeat[1] = { 15.f, -15.f, 0.f };
    m_vLocalReelSeat[2] = { 15.f, 15.f, 0.f };
    m_vLocalReelSeat[3] = { -15.f, 15.f, 0.f };

    // 릴시트 로컬 중점을 (0, 0, 0)으로 가정했을 때, 상대적인 릴 로컬 중점 좌표
    m_vLocalReel = { 40.f, 0.f, 0.f };

    // 낚시대 좌우 이동 스피드 결정
    m_fSpeed = 3.f;
}

int CFishingRod::Update(void)
{
    Key_Input();

    D3DXMATRIX matScale; // 크기 변환 행렬(크)
    D3DXMATRIX matRotZ;  // 자전 변환 행렬(자)
    D3DXMATRIX matTrans; // 이동 변환 행렬(이)

    // 막대기 렉트 각 점들 이동 및 회전 코드
    // 이동 값 만큼 이동 변환 행렬에 적용
    D3DXMatrixTranslation(&matTrans, m_tInfo.vPos.x, m_tInfo.vPos.y, 0.f);
    // 회전 값 만큼 자전 변환 행렬에 적용
    D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fBarAngle));
    
    // 월드 행렬 = 크기 변환 행렬 X 자전 변환 행렬 X 이동 변환 행렬 X 공전 변환 행렬 X 부모 변환 행렬
    m_tInfo.matWorld = matRotZ * matTrans;
    
    // 막대기 렉트 각 점들을 월드 공간 벡터로 변환
    for (int i = 0; i < 4; ++i)
        D3DXVec3TransformCoord(&m_vWorldBar[i], &m_vLocalBar[i], &m_tInfo.matWorld);



    // 릴시트 렉트 각 점들 이동, 코드
    // 기존 월드 행렬 항등행렬로 초기화
    D3DXMatrixIdentity(&m_tInfo.matWorld);

    // 월드 행렬 = 크기 변환 행렬 X 자전 변환 행렬 X 이동 변환 행렬 X 공전 변환 행렬 X 부모 변환 행렬
    m_tInfo.matWorld = matTrans;

    // 릴시트 렉트 각 점들을 월드 공간 벡터로 변환
    for (int i = 0; i < 4; ++i)
        D3DXVec3TransformCoord(&m_vWorldReelSeat[i], &m_vLocalReelSeat[i], &m_tInfo.matWorld);

    

    // 릴 중점 이동 코드
    // 기존 월드 행렬 항등행렬로 초기화
    D3DXMatrixIdentity(&m_tInfo.matWorld);

    // 릴 중점 회전 각도 만큼 자전 행렬에 적용
    D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fReelAngle));

    // 월드 행렬 = 크기 변환 행렬 x 자전 변환 행렬 x 이동 변환 행렬 x 공전 변환 행렬 x 부모 변환 행렬
    m_tInfo.matWorld = matRotZ * matTrans;

    // 릴 중점을 월드 공간 벡터로 변환
    D3DXVec3TransformCoord(&m_vWorldReel, &m_vLocalReel, &m_tInfo.matWorld);

    
    // 낚시줄 중점 월드 좌표 적용
    m_vWorldFishingLine = { m_vWorldBar[1].x, m_vWorldBar[1].y + m_iLengthFishingLine, 0.f };

    // 미끼 렉트 월드 좌표 적용
    m_vWorldBait[0] = { m_vWorldFishingLine.x - 8.f, m_vWorldFishingLine.y - 8.f, 0.f };
    m_vWorldBait[1] = { m_vWorldFishingLine.x + 8.f, m_vWorldFishingLine.y - 8.f, 0.f };
    m_vWorldBait[2] = { m_vWorldFishingLine.x + 8.f, m_vWorldFishingLine.y + 8.f, 0.f };
    m_vWorldBait[3] = { m_vWorldFishingLine.x - 8.f, m_vWorldFishingLine.y + 8.f, 0.f };

    // 미끼 충돌 렉트 업데이트
    Update_CollisionRectBait();

    return 0;
}

void CFishingRod::Late_Update(void)
{
    // 낚시줄 길이가 0일때 미끼가 비활성화 상태면 활성화 시킨다.
    if (0 >= m_iLengthFishingLine)
    {
        // P 누르면
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
    // 막대기 렉트 렌더
    MoveToEx(hDC, (int)m_vWorldBar[0].x, (int)m_vWorldBar[0].y, nullptr);
    for (int i = 1; i <= 3; ++i)
        LineTo(hDC, (int)m_vWorldBar[i].x, (int)m_vWorldBar[i].y);
    LineTo(hDC, (int)m_vWorldBar[0].x, (int)m_vWorldBar[0].y);

    // 릴시트 렉트 렌더
    MoveToEx(hDC, (int)m_vWorldReelSeat[0].x, (int)m_vWorldReelSeat[0].y, nullptr);
    for (int i = 1; i <= 3; ++i)
        LineTo(hDC, (int)m_vWorldReelSeat[i].x, (int)m_vWorldReelSeat[i].y);
    LineTo(hDC, (int)m_vWorldReelSeat[0].x, (int)m_vWorldReelSeat[0].y);

    // 릴 렌더
    Ellipse(hDC, int(m_vWorldReel.x - 10.f), int(m_vWorldReel.y - 10.f),
        int(m_vWorldReel.x + 10.f), int(m_vWorldReel.y + 10.f));
    // 릴 지지대 렌더
    MoveToEx(hDC, (int)m_tInfo.vPos.x, (int)m_tInfo.vPos.y, nullptr);
    LineTo(hDC, (int)m_vWorldReel.x, (int)m_vWorldReel.y);

    // 낚시줄 렌더
    MoveToEx(hDC, (int)m_vWorldBar[1].x, (int)m_vWorldBar[1].y, nullptr);
    LineTo(hDC, (int)m_vWorldFishingLine.x, (int)m_vWorldFishingLine.y);

    // 미끼 렉트 렌더
    // 미끼가 활성화 됐을때만 렌더
    if (m_bBaitActivated)
    {
        MoveToEx(hDC, (int)m_vWorldBait[0].x, (int)m_vWorldBait[0].y, nullptr);
        for (int i = 1; i <= 3; ++i)
            LineTo(hDC, (int)m_vWorldBait[i].x, (int)m_vWorldBait[i].y);
        LineTo(hDC, (int)m_vWorldBait[0].x, (int)m_vWorldBait[0].y);
    }


    //// 녹색 펜 생성
    //HPEN hGreenPen = CreatePen(PS_SOLID, 1, RGB(129, 193, 71));
    //HPEN hOldPen = (HPEN)SelectObject(hDC, hGreenPen); // 적용과 동시에 기존거 백업

    //// 투명 브러쉬 생성
    //HBRUSH hHollowBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    //HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hHollowBrush); // 적용과 동시에 기존거 백업

    //// 충돌체 렉트 렌더
    //Rectangle(hDC, m_tCollisionRectBait.left, m_tCollisionRectBait.top,
    //    m_tCollisionRectBait.right, m_tCollisionRectBait.bottom);

    //// 펜이랑 브러쉬 새로 만들어서 다 썼으면 기존꺼 돌려주기
    //SelectObject(hDC, hOldPen);
    //SelectObject(hDC, hOldBrush);
    //// 직접 생성한 펜만 삭제
    //DeleteObject(hGreenPen);

    //// 낚시줄 중점 좌표 출력
    //TCHAR      szBuff1[64] = L"";
    //swprintf_s(szBuff1, L"현재 낚시줄 중점 x, y : %f, %f", m_vWorldFishingLine.x, m_vWorldFishingLine.y);
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
    // 줄 감기, 휠 안쪽으로 감기
    if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT))
    {
        // 낚시줄 길이가 0보다 작아지면 0으로 고정, 휠 고정
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

    // 줄 풀기, 휠 바깥쪽으로 감기
    else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
    {
        m_fReelAngle += 15.f;
        m_iLengthFishingLine += 2;
    }

    // 낚시대 좌로 이동하기
    else if (CKeyMgr::Get_Instance()->Key_Pressing('A'))
    {
        m_tInfo.vPos += m_tInfo.vDir * -m_fSpeed;
    }

    // 낚시대 우로 이동하기
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
