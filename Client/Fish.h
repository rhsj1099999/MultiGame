#pragma once
#include "Obj.h"

class CFish :
    public CObj
{
public:
    CFish();
    virtual ~CFish() override;

public:
    // CObj��(��) ���� ��ӵ�
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
    D3DXVECTOR3     m_vLocalFish[10];       // ����� ���� ��ǥ
    D3DXVECTOR3     m_vWorldFish[10];       // ����� ���� ��ǥ

    //float           m_fSpeed;               // ����� �̵� ���ǵ�
    int             m_iDir;                 // ����� ����       1 : ������, -1 : ����

    RECT            m_tCollisionRect;       // ����� �浹 ��Ʈ(��)
};

