#pragma once

#include "Obj.h"
#include "Portal.h"
#include "DownCase.h"
#include "MidCase.h"
#include "UpCase.h"

template<typename T>
class CAbstractFactory
{
public:
	CAbstractFactory() {}
	~CAbstractFactory() {}

public:
	static CObj* Create(bool bServerMode = false)
	{
		CObj* pObj = new T;
		pObj->SetServerMode(bServerMode);
		pObj->Initialize();

		return pObj;
	}

	static CObj* Create(float _fX, float _fY)
	{
		CObj* pObj = new T;
		pObj->Initialize();
		pObj->Set_Pos(_fX, _fY);

		return pObj;
	}

	static CObj* Create(float _fX, float _fY, float _fSpeed)
	{
		CObj* pObj = new T;
		pObj->Initialize();
		pObj->Set_Pos(_fX, _fY);
		pObj->Set_Speed(_fSpeed);

		return pObj;
	}

	static CObj* Create_Portal(float _fX, float _fY, int _iPortalNum)
	{
		CObj* pObj = new T;
		pObj->Initialize();
		pObj->Set_Pos(_fX, _fY);
		dynamic_cast<CPortal*>(pObj)->Set_PortalNumber(_iPortalNum);
		return pObj;
	}

	static CObj* Create_Roulette(int _iRouletteNum)
	{
		CObj* pObj = new T;
		pObj->Set_Pos(275.f, 300.f);
		dynamic_cast<CRoulette*>(pObj)->Set_RouletteNumber(_iRouletteNum);
		pObj->Initialize();
		return pObj;
	}
};

