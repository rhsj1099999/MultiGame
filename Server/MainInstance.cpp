#include "stdafx.h"
#include "MainInstance.h"

#include "Timer.h"
#include "MainServer.h"

CMainInstance* CMainInstance::_instance = nullptr;

CMainInstance* CMainInstance::GetInstance()
{
	if (_instance == nullptr)
	{
		_instance = new CMainInstance();
	}
	return _instance;
}

CMainInstance::~CMainInstance()
{
	_timer->Destroy_Instance();
	_timer = nullptr;

	_server->Release();
	
	_server->Destroy_Instance();
	_server = nullptr;
}


void CMainInstance::Init()
{
	_timer = CTimer::GetInstance();
	_timer->Init();

	_server = CMainServer::GetInstance();
	_server->Init();
}

void CMainInstance::Tick()
{
	_timer->Tick();
	_server->Tick();
}
