#pragma once
#include "Include.h"
#include "Obj.h"

class CServerManager
{
public:
	CServerManager();
	~CServerManager();
public:
	int		Update();
	void	Late_Update();
	void	Release();

public:

public:
	static CServerManager* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CServerManager;
		}
		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static CServerManager* m_pInstance;
};

