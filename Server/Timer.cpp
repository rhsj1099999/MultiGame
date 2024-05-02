#include "stdafx.h"
#include "Timer.h"

CTimer* CTimer::m_pInstance = nullptr;

CTimer::CTimer()
{
}

CTimer::~CTimer()
{
}

CTimer* CTimer::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CTimer();
	}
	return m_pInstance;
}

void CTimer::Destroy_Instance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void CTimer::Init()
{
	m_dwOldTime = GetTickCount();
	m_dwCurrTime = GetTickCount();
}

void CTimer::Tick()
{
	m_dwCurrTime = GetTickCount();
	m_dwDeltaTime = m_dwCurrTime - m_dwOldTime;
	m_dwOldTime = m_dwCurrTime;
}

DWORD CTimer::GetDeltaTime()
{
	return m_dwDeltaTime;
}

DWORD CTimer::GetCurrTime()
{
	return m_dwCurrTime;
}
