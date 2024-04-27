#pragma once
#include "Obj.h"
class CFishingRod :
    public CObj
{
public:
    CFishingRod();
    virtual ~CFishingRod() override;

public:
    // CObj을(를) 통해 상속됨
    virtual void Initialize(void) override;
    virtual int Update(void) override;
    virtual void Late_Update(void) override;
    virtual void Render(HDC hDC) override;
    virtual void Release(void) override;
    virtual void OnTrigger() override;
    virtual void Cal_WorldMatrix() override;

public:
    bool Get_BaitActivated() { return m_bBaitActivated; }

    void Set_BaitActivated(bool _bBaitActivated) { m_bBaitActivated = _bBaitActivated; }

public:
    void Key_Input();
    void Update_CollisionRectBait();
    RECT Get_CollisionRectBait() { return m_tCollisionRectBait; }

private:
    D3DXVECTOR3     m_vLocalBar[4];             // 막대기 렉트 로컬 좌표
    D3DXVECTOR3     m_vWorldBar[4];             // 막대기 렉트 월드 좌표
    float           m_fBarAngle;                // 막대기 회전 각도

    D3DXVECTOR3     m_vLocalReel;               // 릴 로컬 좌표
    D3DXVECTOR3     m_vWorldReel;               // 릴 월드 좌표
    float           m_fReelAngle;               // 릴 중점 회전 각도

    D3DXVECTOR3     m_vLocalReelEllipse[4];     // 릴 원 로컬 좌표
    D3DXVECTOR3     m_vWorldReelEllipse[4];     // 릴 원 월드 좌표

    D3DXVECTOR3     m_vLocalReelSeat[4];        // 릴시트 렉트 로컬 좌표
    D3DXVECTOR3     m_vWorldReelSeat[4];        // 릴시트 렉트 월드 좌표

    D3DXVECTOR3     m_vWorldFishingLine;        // 낚시줄 월드 중점 좌표
    int             m_iLengthFishingLine;       // 낚시줄 길이

    D3DXVECTOR3     m_vWorldBait[4];            // 미끼 렉트 월드 좌표

    float           m_fSpeed;                   // 낚시대 좌우 이동 스피드

    RECT            m_tCollisionRectBait;       // 미끼 충돌 렉트

    bool            m_bBaitActivated;           // 미끼 활성화 여부
};

