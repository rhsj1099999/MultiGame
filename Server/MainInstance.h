#pragma once

class CTimer;
class CMainServer;

class CMainInstance
{
public:
	static CMainInstance* GetInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new CMainInstance();
		}
		return m_pInstance;
	}
	~CMainInstance();

	void Init();
	void Tick();

private:
	static CMainInstance* m_pInstance;

	CTimer* m_pTimer = nullptr;
	CMainServer* m_pMainServer = nullptr;
};

