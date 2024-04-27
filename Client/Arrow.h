#pragma once
#include "Obj.h"

class CArrow : public CObj
{
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(HDC hDC) override;
	virtual void Release(void) override;
	virtual void Cal_WorldMatrix() override;
	void	OnRoulette(CObj* _pObj);
	int		Get_CurNum() { return m_iCurNum; }
private:
	D3DXVECTOR3			m_vPoint[4];
	D3DXVECTOR3			m_vOriginPoint[4];
	float m_fAngle;
	int m_iCurNum = 0;
	int m_iPreNum = 0;
};

