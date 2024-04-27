#pragma once

#include "Obj.h"

class CMonster : public CObj
{
public:
	CMonster();
	~CMonster();

public:
	void		Set_Player(CObj* pPlayer) { m_pTarget = pPlayer;  }

public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize(void) override;
	virtual int Update(void) override;
	virtual void Render(HDC hDC) override;
	virtual void Release(void) override;

private:
	CObj*		m_pTarget = nullptr;

};

