#pragma once
#include "Obj.h"
class CMolePlayer : public CObj
{
private:
	float m_fAngle;
	bool m_bAttack;
	DWORD m_dwAttackCheck;
	
	D3DXVECTOR3 m_vPoint[8];
	D3DXVECTOR3 m_PointVec[8];

	INFO m_tColliderInfo;
	D3DXVECTOR3 m_vColliderPoint[4];
	D3DXVECTOR3 m_ColliderPointVec[4];

	int m_iCombo;
	float m_fMoneyScale;
	DWORD m_dwComboTimer;
	


public:
	CMolePlayer();
	~CMolePlayer();

	// CObj을(를) 통해 상속됨
	virtual void Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(HDC hDC) override;
	virtual void Release(void) override;
	virtual void Cal_WorldMatrix()override;
	virtual void OnTrigger();
	virtual void OnCollision(CObj* _other);
	
private:

	void		Key_Input(void);

public:
	//액세스 함수
	bool		Get_Attack() { return m_bAttack; }
	RECT		Get_ColliderRect();
	int			Get_Combo() { return m_iCombo; }
	float			Get_MoneyScale() { return m_fMoneyScale; }


};

