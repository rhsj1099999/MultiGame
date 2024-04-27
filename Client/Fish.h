#pragma once
#include "Obj.h"

class CFish :
    public CObj
{
public:
    CFish();
    virtual ~CFish() override;

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
    void Move();
    void Update_CollisionRect();
    RECT Get_CollisionRect() { return m_tCollisionRect; }
private:
    D3DXVECTOR3     m_vLocalFish[10];       // 물고기 로컬 좌표
    D3DXVECTOR3     m_vWorldFish[10];       // 물고기 월드 좌표

    //float           m_fSpeed;               // 물고기 이동 스피드
    int             m_iDir;                 // 물고기 방향       1 : 오른쪽, -1 : 왼쪽

    RECT            m_tCollisionRect;       // 물고기 충돌 렉트(입)
};

