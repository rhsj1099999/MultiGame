#include "stdafx.h"

#include "CollisionMgr.h"
#include "KeyMgr.h"
#include "UIMgr.h"

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::Collision_Rect(list<CObj*> _Dst, list<CObj*> _Src)
{
	RECT		rc{};

	for (auto& Dest : _Dst)
	{
		for (auto& Src : _Src)
		{
			if (IntersectRect(&rc, &Dest->Get_Rect(), &Src->Get_Rect()))
			{
				Dest->OnTrigger();
				Src->OnTrigger();
			}
		}
	}
}

