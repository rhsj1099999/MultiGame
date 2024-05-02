#pragma once
#include "Include.h"
#include "Obj.h"

class CServerManager
{
public:
	CServerManager();
	~CServerManager();
public:
	int		Update();
	void	Late_Update();
	void	Release();
	void	InitServer();

	bool GetClientConnected() { return m_bClientConnected; }
	int GetServerUser() { return m_iCurrUser; }

public:

public:
	static CServerManager* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CServerManager;
		}
		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	void WorkerEntry_D(HANDLE hHandle, char* pOut, int size = 100);
	void ExecuetionMessage(PREDATA::OrderType eType, void* Data, int DataSize);
private:
	static CServerManager* m_pInstance;
	/*---------------
	Handle, Core, Etc
	---------------*/
	WSADATA m_wsa = {};
	SOCKET m_Socket = {};
	SOCKADDR_IN m_ServerAddr = {};
	vector<thread> m_vecWorkerThreads;
	ClientSession* m_pSession = nullptr;
	HANDLE m_IOCPHandle = INVALID_HANDLE_VALUE;

	/*---------------
		Data
	---------------*/
	bool m_bClientConnected = false;
	int m_iCurrUser = 1; //At Least Me

};

