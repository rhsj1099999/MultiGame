#pragma once
#include "Windows.h"
class Timer
{
public:
	static Timer* GetInstance()
	{
		static Timer Instance;
		return &Instance;
	}
	void Init()
	{
		m_dwOldTime = GetTickCount();
		m_dwCurrTime = GetTickCount();
	}
	void Tick()
	{
		m_dwCurrTime = GetTickCount();
		m_dwDeltaTime = m_dwCurrTime - m_dwOldTime;
		m_dwOldTime = m_dwCurrTime;
	}
	DWORD GetDeltaTime()
	{
		return m_dwDeltaTime;
	}
	DWORD GetCurrTime()
	{
		return m_dwCurrTime;
	}

private:
	DWORD m_dwOldTime = {};
	DWORD m_dwCurrTime = {};
	DWORD m_dwDeltaTime = {};
};

