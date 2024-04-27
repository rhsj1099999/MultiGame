#pragma once
#include "Obj.h"
class CMole : public CObj
{
private:
	D3DXVECTOR3 m_vPoint[6];
	D3DXVECTOR3 m_PointVec[6];

	INFO m_tColliderInfo;
	D3DXVECTOR3 m_vColliderPoint[4];
	D3DXVECTOR3 m_ColliderPointVec[4];
	
	float m_fAngle;
	float m_fYscale;
	

	DWORD m_dwReviveInterval;
	int m_iReviveDelay;
public:
	CMole();
	~CMole();

	// CObj을(를) 통해 상속됨
	virtual void Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(HDC hDC) override;
	virtual void Release(void) override;
	virtual void Cal_WorldMatrix() override;
	virtual void OnTrigger();
	
	
	virtual void OnCollision(CObj* _other);

	float		Get_Scale() { return m_fYscale; }

public:
	RECT		Get_ColliderRect();

};

