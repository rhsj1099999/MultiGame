#pragma once
#include "Include.h"
#include "Scene.h"

class CSceneMgr
{
private:
	CSceneMgr();
	~CSceneMgr();

public:
	void		Scene_Change(SCENEID eID);
	void		Update(void);
	void		Late_Update(void);
	void		Render(HDC hDC);
	void		Release(void);
	SCENEID		GetCurSceneID() { return m_eCurScene; }

	void		Restart_Scene();
	void		Set_CutScene(bool _bCheck) { m_bCutScene = _bCheck; }
	bool		Get_CutScene() { return m_bCutScene; }

public:
	static CSceneMgr* Get_Instance()
	{
		if (!m_pInstance)
			m_pInstance = new CSceneMgr;

		return m_pInstance;
	}

	static void		Destroy_Instance(void)
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	static CSceneMgr* m_pInstance;
	CScene* m_pScene;
	SCENEID				m_ePreScene;
	SCENEID				m_eCurScene;
	bool m_bCutScene = false;
public:
	static bool				m_IsChange;

};

