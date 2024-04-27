#pragma once
#include "Obj.h"
class CTunnel : public CObj
{

private:
	D3DXVECTOR3 m_vPoint[6];
	D3DXVECTOR3 m_PointVec[6];

	bool m_bSummon;
	DWORD m_dwSummonTime;
public:
	CTunnel();
	~CTunnel();
	
	// CObj을(를) 통해 상속됨
	virtual void Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(HDC hDC) override;
	virtual void Release(void) override;
	virtual void Cal_WorldMatrix() override;

	void	Summon_Mole();
};

