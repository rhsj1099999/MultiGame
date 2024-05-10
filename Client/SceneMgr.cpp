#include "stdafx.h"
#include "SceneMgr.h"
#include "WorldMap.h"
#include "Stage4.h"
#include "Stage_Matching.h"
#include "Stage_Exit.h"
#include "CServerManager.h"

CSceneMgr* CSceneMgr::m_pInstance = nullptr;
bool		CSceneMgr::m_IsChange = false;

CSceneMgr::CSceneMgr() : m_pScene(nullptr), m_ePreScene(SC_END), m_eCurScene(SC_WORLDMAP)
{
}


CSceneMgr::~CSceneMgr()
{
	Release();
}


void CSceneMgr::Restart_Scene()
{
	if (nullptr != m_pInstance)
	{
		m_ePreScene = SC_END;
		Scene_Change(m_eCurScene);
		m_IsChange = true;
	}
}
void CSceneMgr::Scene_Change(SCENEID eID, bool bServerMode)
{
	m_eCurScene = eID;

	if (m_ePreScene != m_eCurScene)
	{
		Safe_Delete(m_pScene);

		switch (m_eCurScene)
		{
		case SC_WORLDMAP:
			m_pScene = new CWorldMap;
			break;
		case SC_STAGE4:
			m_pScene = new CStage4;
			break;
		case SC_MATCHINGSTAGE:
			m_pScene = new Stage_Matching;
			break;
		case SC_STAGE_EXIT:
			m_pScene = new CStage_Exit;
			break;
		}

		if (nullptr != m_pScene)
		{
			if (bServerMode == true)
			{
				m_pScene->SetServerMode(bServerMode);
			}
			m_pScene->Initialize();
		}
		m_ePreScene = m_eCurScene;
	}

}

void CSceneMgr::Update(void)
{
	m_pScene->Update();
}

void CSceneMgr::Late_Update(void)
{
	m_pScene->Late_Update();
}

void CSceneMgr::Render(HDC hDC)
{
	m_pScene->Render(hDC);
}

void CSceneMgr::Release(void)
{
	Safe_Delete(m_pScene);
}
