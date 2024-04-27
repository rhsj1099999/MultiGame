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
	// �̳��� Ȱ��ȭ �ȵ������� ����
	if (!(dynamic_cast<CFishingRod*>(_fishingRod)->Get_BaitActivated()))
		return;

	RECT		rc{};

	for (auto& fish : _Src)
	{
		// �̳��� ����Ⱑ �浹���� ��
		if (IntersectRect(&rc, &dynamic_cast<CFishingRod*>(_fishingRod)->Get_CollisionRectBait(),
			&dynamic_cast<CFish*>(fish)->Get_CollisionRect()))
		{
			// �����̽�Ű�� ������
			if (CKeyMgr::Get_Instance()->Key_Down(VK_SPACE))
			{
				// ����Ⱑ �������(����⸦ ����)
				fish->Set_Dead(true);
				// �̳��� �������(��Ȱ��ȭ�ǰ�)
				dynamic_cast<CFishingRod*>(_fishingRod)->Set_BaitActivated(false);
				// ���� ����
				CUIMgr::Get_Instance()->Add_Money(400);
			}
			// �����̽�Ű�� �ȴ�����
			else
			{
				// �̳��� �������(��Ȱ��ȭ�ǰ�)
				dynamic_cast<CFishingRod*>(_fishingRod)->Set_BaitActivated(false);
				// ���� ����
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
/// �δ������ �浹ó���Լ�
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
