#include "stdafx.h"
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
