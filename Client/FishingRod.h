#pragma once
#include "Obj.h"
class CFishingRod :
    public CObj
{
public:
    CFishingRod();
    virtual ~CFishingRod() override;

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
    bool Get_BaitActivated() { return m_bBaitActivated; }

    void Set_BaitActivated(bool _bBaitActivated) { m_bBaitActivated = _bBaitActivated; }

public:
    void Key_Input();
    void Update_CollisionRectBait();
    RECT Get_CollisionRectBait() { return m_tCollisionRectBait; }

private:
    D3DXVECTOR3     m_vLocalBar[4];             // ����� ��Ʈ ���� ��ǥ
    D3DXVECTOR3     m_vWorldBar[4];             // ����� ��Ʈ ���� ��ǥ
    float           m_fBarAngle;                // ����� ȸ�� ����

    D3DXVECTOR3     m_vLocalReel;               // �� ���� ��ǥ
    D3DXVECTOR3     m_vWorldReel;               // �� ���� ��ǥ
    float           m_fReelAngle;               // �� ���� ȸ�� ����

    D3DXVECTOR3     m_vLocalReelEllipse[4];     // �� �� ���� ��ǥ
    D3DXVECTOR3     m_vWorldReelEllipse[4];     // �� �� ���� ��ǥ

    D3DXVECTOR3     m_vLocalReelSeat[4];        // ����Ʈ ��Ʈ ���� ��ǥ
    D3DXVECTOR3     m_vWorldReelSeat[4];        // ����Ʈ ��Ʈ ���� ��ǥ

    D3DXVECTOR3     m_vWorldFishingLine;        // ������ ���� ���� ��ǥ
    int             m_iLengthFishingLine;       // ������ ����

    D3DXVECTOR3     m_vWorldBait[4];            // �̳� ��Ʈ ���� ��ǥ

    float           m_fSpeed;                   // ���ô� �¿� �̵� ���ǵ�

    RECT            m_tCollisionRectBait;       // �̳� �浹 ��Ʈ

    bool            m_bBaitActivated;           // �̳� Ȱ��ȭ ����
};

