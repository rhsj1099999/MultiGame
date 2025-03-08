#include "stdafx.h"
#include "Timer.h"

CTimer* CTimer::_instance = nullptr;

CTimer::CTimer()
{
}

CTimer::~CTimer()
{
}

CTimer* CTimer::GetInstance()
{
	if (_instance == nullptr)
	{
		_instance = new CTimer();
	}
	return _instance;
}

void CTimer::Destroy_Instance()
{
	if (_instance)
	{
		delete _instance;
		_instance = nullptr;
	}
}

void CTimer::Init()
{
	_oldTime = GetTickCount();
	_currTime = GetTickCount();
}

void CTimer::Tick()
{
	_currTime = GetTickCount();
	_deltaTime = _currTime - _oldTime;
	_oldTime = _currTime;
}

DWORD CTimer::GetDeltaTime()
{
	return _deltaTime;
}

DWORD CTimer::GetCurrTime()
{
	return _currTime;
}
