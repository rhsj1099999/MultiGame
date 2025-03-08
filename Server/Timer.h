#pragma once
class CTimer : public CBase
{
	public:		static CTimer* GetInstance();
	public:		static void Destroy_Instance();
	private:	static CTimer* _instance;

	public:		CTimer();
	public:		~CTimer();

	public:		void Release()override {};

	public:		void Init();
	public:		void Tick();

	public:		DWORD GetDeltaTime();
	public:		DWORD GetCurrTime();


	private:	DWORD _oldTime = {};
	private:	DWORD _currTime = {};
	private:	DWORD _deltaTime = {};
};

