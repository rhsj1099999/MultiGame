#pragma once

#define MAXTIMEOUT 2500
#define MAXLATECOUNT 2

class CMainServer
{
public:
	static CMainServer* GetInstance();
	static void Destroy_Instance();
	CMainServer();
	~CMainServer();

	void Release();

	void Init();
	void Tick();

public:
	//int		Update();
	//void	Late_Update();
	//void	Release();
	//void	InitServer();

	//bool GetClientConnected() { return m_bClientConnected; }
	//int GetServerUser() { return m_iCurrUser; }

private:
	void ConnectTry();
	void WorkerEntry_D(HANDLE hHandle);
	void LiveCheck();
	void MatchingRoom();
	//void ExecuetionMessage(PREDATA::OrderType eType, void* Data, int DataSize);
private:
	static CMainServer* m_pInstance;
	/*---------------
	Handle, Core, Etc
	---------------*/
	WSADATA m_wsa = {};
	SOCKET m_Socket = {};
	SOCKADDR_IN m_ServerAddr = {};
	vector<thread> m_vecWorkerThreads;
	ClientSession* m_pSession = nullptr;
	HANDLE m_IOCPHandle = INVALID_HANDLE_VALUE;

	list<ClientSession*> m_liClientSockets;

	/*---------------
		Data
	---------------*/
	bool m_bClientConnected = false;
	int m_iCurrUser = 0; //At Least Me

};

