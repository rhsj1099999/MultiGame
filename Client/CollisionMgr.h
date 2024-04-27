#pragma once
#include "Obj.h"

class CCollisionMgr
{
public:
	CCollisionMgr();
	~CCollisionMgr();

public:
	static void	Collision_Rect(list<CObj*> _Dst, list<CObj*> _Src);
	static void Collision_FishingRod_Fish(CObj* _fishingRod, list<CObj*> _Src);
	static void Collision_Roulette(list<CObj*> _Dst, list<CObj*> _Src);
	static void	Collision_MoleHunt(list<CObj*> _Dst, list<CObj*> _Src);
};

