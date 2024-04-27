#pragma once
#include "Obj.h"

class CBetting : public CObj
{
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(HDC hDC) override;
	virtual void Release(void) override;
	virtual void Cal_WorldMatrix() override;
public:
	void Key_Input();
	bool Get_Game() { return m_bGame; }
	void Set_Game(bool _bCheck) { m_bGame = _bCheck; }
	int* Get_Betting() { return m_iBetMoneyArray; }
private:
	bool m_bGame;
	int m_iBetMoneyArray[12];
};

