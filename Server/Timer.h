#pragma once
class CTimer
{
public:
	CTimer();
	~CTimer();
	static CTimer* GetInstance();
	static void Destroy_Instance();
	void Init();
	void Tick();
	DWORD GetDeltaTime();
	DWORD GetCurrTime();

private:
	DWORD m_dwOldTime = {};
	DWORD m_dwCurrTime = {};
	DWORD m_dwDeltaTime = {};
	static CTimer* m_pInstance;
};

