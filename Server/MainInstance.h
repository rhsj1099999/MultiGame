#pragma once

class CTimer;
class CMainServer;
class CMainInstance
{
	public:		static CMainInstance* GetInstance();
	private:	static CMainInstance* _instance;
	public:		~CMainInstance();

	public:		void Stop() { _activated = false; }
	public:		void Init();
	public:		void Tick();



	private:	CTimer* _timer = nullptr;
	private:	CMainServer* _server = nullptr;
	private:	bool _activated = true;
};

