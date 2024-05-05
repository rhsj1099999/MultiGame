#pragma once

#include "Include.h"

class CObj abstract
{
public:
	CObj();
	virtual ~CObj();

public:
	const INFO&		Get_Info(void) const { return m_tInfo; }
	RECT			Get_Rect();
	bool			Get_Dead() { return m_bDead; }
	const OBJID& Get_ObjID()
	{
		return this->m_eObjID;
	}

	void			Set_Speed(float _fSpeed) { m_fSpeed = _fSpeed; }
	void			Set_Pos(float _fX, float _fY);
	void			Set_Dead(bool _bDead) { m_bDead = _bDead; }
public:
	virtual void		Initialize(void)	PURE;
	virtual int			Update(void)		PURE;
	virtual void		Late_Update(void)	PURE;
	virtual void		Render(HDC hDC)		PURE;
	virtual void		Release(void)		PURE;
	virtual void		OnTrigger();
	virtual void		Cal_WorldMatrix()	PURE;
	virtual void		OnCollision(CObj* _other);
	virtual void		SetServerMode(bool bMode) { m_bIsServerMode = bMode; }

protected:
	INFO			m_tInfo;

	float			m_fSpeed;		// ∞¥√º ¿Ãµø Ω∫««µÂ

	bool			m_bDead;		// ∞¥√º ªÁ∏¡ ø©∫Œ

	OBJID			m_eObjID;

	bool			m_bIsServerMode = false;
};

