#include "stdafx.h"
#include "Fish.h"
#include "FishingRod.h"
#include "Roulette.h"
#include "Arrow.h"

#include "CollisionMgr.h"
#include "KeyMgr.h"
#include "UIMgr.h"
#include "MolePlayer.h"
#include "Mole.h"

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

void CCollisionMgr::Collision_FishingRod_Fish(CObj* _fishingRod, list<CObj*> _Src)
{
	// 미끼가 활성화 안돼있으면 리턴
	if (!(dynamic_cast<CFishingRod*>(_fishingRod)->Get_BaitActivated()))
		return;

	RECT		rc{};

	for (auto& fish : _Src)
	{
		// 미끼랑 물고기가 충돌했을 때
		if (IntersectRect(&rc, &dynamic_cast<CFishingRod*>(_fishingRod)->Get_CollisionRectBait(),
			&dynamic_cast<CFish*>(fish)->Get_CollisionRect()))
		{
			// 스페이스키를 누르면
			if (CKeyMgr::Get_Instance()->Key_Down(VK_SPACE))
			{
				// 물고기가 사라지고(물고기를 잡음)
				fish->Set_Dead(true);
				// 미끼가 사라지고(비활성화되고)
				dynamic_cast<CFishingRod*>(_fishingRod)->Set_BaitActivated(false);
				// 돈을 얻음
				CUIMgr::Get_Instance()->Add_Money(400);
			}
			// 스페이스키를 안누르면
			else
			{
				// 미끼가 사라지고(비활성화되고)
				dynamic_cast<CFishingRod*>(_fishingRod)->Set_BaitActivated(false);
				// 돈을 잃음
				//CUIMgr::Get_Instance()->Add_Money(-200);
			}
			fish->OnTrigger();
		}
	}
}

void CCollisionMgr::Collision_Roulette(list<CObj*> _Dst, list<CObj*> _Src)
{
	RECT		rc{};

	for (auto& Dest : _Dst)
	{
		for (auto& Src : _Src)
		{
			if (IntersectRect(&rc, dynamic_cast<CRoulette*>(Dest)->Get_Roulette_Rect(), &Src->Get_Rect()))
			{
				dynamic_cast<CArrow*>(Src)->OnRoulette(Dest);
			}
		}
	}
}


/// <summary>
/// 두더지잡기 충돌처리함수
/// </summary>
/// <param name="_Dst"></param>
/// <param name="_Src"></param>
void CCollisionMgr::Collision_MoleHunt(list<CObj*> _Dst, list<CObj*> _Src)
{
	RECT		rc{};

	if (dynamic_cast<CMolePlayer*>(_Dst.front())->Get_Attack())
	{
		for (auto& Dest : _Dst)
		{
			RECT PlayerColRect = dynamic_cast<CMolePlayer*>(Dest)->Get_ColliderRect();

			for (auto& Src : _Src)
			{
				RECT MoleColRect = dynamic_cast<CMole*>(Src)->Get_ColliderRect();

				if (IntersectRect(&rc, &PlayerColRect, &MoleColRect))
				{
					Dest->OnCollision(Src);
					Src->OnCollision(Dest);
				}
			}
		}
	}

}
