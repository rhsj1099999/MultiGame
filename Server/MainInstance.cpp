#include "stdafx.h"
#include "MainInstance.h"

#include "Timer.h"
#include "MainServer.h"

CMainInstance* CMainInstance::m_pInstance = nullptr;

CMainInstance::~CMainInstance()
{
	if (m_pTimer != nullptr)
	{
		m_pTimer->Destroy_Instance();
		m_pTimer = nullptr;
	}
		

	m_pMainServer->Release();
	
	if (m_pMainServer != nullptr)
	{
		m_pMainServer->Destroy_Instance();
		m_pMainServer = nullptr;
	}
		
}


void CMainInstance::Init()
{
	if (m_pTimer == nullptr)
		m_pTimer = CTimer::GetInstance();
	m_pTimer->Init();

	if (m_pMainServer == nullptr)
		m_pMainServer = CMainServer::GetInstance();
	m_pMainServer->Init();
}

void CMainInstance::Tick()
{
	m_bIsRunning = true;

	if (m_bIsTick == true)
	{
		if (m_pTimer != nullptr)
		{
			m_pTimer->Tick();
		}

		if (m_pMainServer != nullptr)
		{
			m_pMainServer->Tick();
		}
	}

	m_bIsRunning = false;
}
