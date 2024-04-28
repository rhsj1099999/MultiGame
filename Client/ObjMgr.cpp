#include "stdafx.h"
#include "Roulette.h"
#include "Betting.h"
#include "Arrow.h"

#include "ObjMgr.h"
#include "CollisionMgr.h"
#include "SceneMgr.h"
#include "UIMgr.h"

CObjMgr* CObjMgr::m_pInstance = nullptr;

CObjMgr::CObjMgr()
{
}

CObjMgr::~CObjMgr()
{
	Release();
}

void CObjMgr::Spin_Roulette()
{
	//int iRandom = rand() % 8 + 5; // rand() % 5+8
	//float fRandom = (rand() % 10) * 0.1f;
	//for (auto& iter = m_ObjList[OBJ_ROULETTE].begin();
	//	iter != m_ObjList[OBJ_ROULETTE].end();
	//	iter++)
	//{
	//	if (!dynamic_cast<CRoulette*>(*iter)->Get_Spin())
	//	{
	//		dynamic_cast<CRoulette*>(*iter)->Set_Speed((float)iRandom + fRandom);
	//		dynamic_cast<CRoulette*>(*iter)->Set_Spin();
	//		dynamic_cast<CRoulette*>(*iter)->Set_Game(true);
	//		dynamic_cast<CBetting*>(m_ObjList[OBJ_BETTING].front())->Set_Game(true);
	//		m_pBettingArray = dynamic_cast<CBetting*>(m_ObjList[OBJ_BETTING].front())->Get_Betting();
	//	}
	//	else
	//	{
	//		return;
	//	}
	//}
}
void CObjMgr::Roulette_Result()
{
	//int iResult = dynamic_cast<CArrow*>(m_ObjList[OBJ_ARROW].front())->Get_CurNum();
	//int iMoneyCount = 0;

	//for (int i = 0; i < 12; i++)
	//{
	//	iMoneyCount -= m_pBettingArray[i];
	//}

	//iMoneyCount += m_pBettingArray[iResult] * 5;
	//CUIMgr::Get_Instance()->Set_Money(CUIMgr::Get_Instance()->Get_Money() + m_pBettingArray[iResult] * 5);

	//if (iResult % 2 == 1)
	//{
	//	iMoneyCount += m_pBettingArray[10] * 2;
	//	CUIMgr::Get_Instance()->Set_Money(CUIMgr::Get_Instance()->Get_Money() + m_pBettingArray[10] * 2);
	//}
	//else if (iResult % 2 == 0)
	//{
	//	iMoneyCount += m_pBettingArray[11] * 2;
	//	CUIMgr::Get_Instance()->Set_Money(CUIMgr::Get_Instance()->Get_Money() + m_pBettingArray[11] * 2);
	//}

	//CUIMgr::Get_Instance()->Set_RoulletUI(iResult, iMoneyCount);
	//for (int i = 0; i < 12; i++)
	//{
	//	m_pBettingArray[i] = 0;
	//}
	//dynamic_cast<CBetting*>(m_ObjList[OBJ_BETTING].front())->Set_Game(false);

}

void CObjMgr::Add_Object(OBJID eID, CObj* pObj)
{
	if (OBJ_END <= eID || pObj == nullptr)
		return;

	m_ObjList[eID].push_back(pObj);
}

int CObjMgr::Update()
{
	int iResult = 0;
	for (size_t i = 0; i < OBJ_END; i++)
	{
		for (auto& iter = m_ObjList[i].begin();
			iter != m_ObjList[i].end();)
		{
			iResult = (*iter)->Update();
			if (iResult == OBJ_DEAD)
			{
				Safe_Delete<CObj*>(*iter);
				iter = m_ObjList[i].erase(iter);
			}
			else
				iter++;
		}
	}
	return iResult;
}

void CObjMgr::Late_Update()
{
	for (size_t i = 0; i < OBJ_END; i++)
	{
		for (auto& iter = m_ObjList[i].begin();
			iter != m_ObjList[i].end();
			iter++)
		{
			(*iter)->Late_Update();
		}
	}

	if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_WORLDMAP)
		CCollisionMgr::Collision_Rect(m_ObjList[OBJ_PLAYER], m_ObjList[OBJ_PORTAL]);
	//else if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_STAGE3)
	//	CCollisionMgr::Collision_FishingRod_Fish(m_ObjList[OBJ_FISHINGROD].back(), m_ObjList[OBJ_FISH]);
	//else if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_STAGE2)
	//	CCollisionMgr::Collision_Roulette(m_ObjList[OBJ_ROULETTE], m_ObjList[OBJ_ARROW]);
	//else if (CSceneMgr::Get_Instance()->GetCurSceneID() == SC_STAGE1)
	//	CCollisionMgr::Collision_MoleHunt(m_ObjList[OBJ_MOLEPLAYER], m_ObjList[OBJ_MOLE]);
}
void CObjMgr::Render(HDC hDC)
{
	for (size_t i = 0; i < OBJ_END; i++)
	{
		for (auto& iter = m_ObjList[i].begin();
			iter != m_ObjList[i].end();
			iter++)
		{
			(*iter)->Render(hDC);
		}
	}
}
void CObjMgr::Release()
{
	for (size_t i = 0; i < OBJ_END; i++)
	{
		for (CObj* pObj : m_ObjList[i])
		{
			Safe_Delete<CObj*>(pObj);
		}
		m_ObjList[i].clear();
	}
}
