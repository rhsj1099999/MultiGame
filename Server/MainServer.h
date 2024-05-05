#pragma once

#define MAXTIMEOUT 250000
#define MAXLATECOUNT 2




class CPlayingRoom;


class CMainServer
{
	typedef void (CMainServer::*VFPtr)(void*);
public:
	static CMainServer* GetInstance();
	static void Destroy_Instance();
	CMainServer();
	~CMainServer();

	void Release();

	void Init();
	void Tick();

	bool SettingNextOrder(ClientSession* pSession);

public:
	//int		Update();
	//void	Late_Update();
	//void	Release();
	//void	InitServer();

	//bool GetClientConnected() { return m_bClientConnected; }
	//int GetServerUser() { return m_iCurrUser; }

private:
	void ConnectTry();
	void TickWatingClients();
	void WorkerEntry_D(HANDLE hHandle);
	void LiveCheck();
	void MatchingRoom();

	void Lock_Queue(VFPtr pFArr[], int ArrSize, void* Args[]);
	void Lock_Queue_Push(void* Ptr);
	void Lock_Queue_ChangingRoom(void* Ptr);

	void Lock_Session(VFPtr pFArr[], int ArrSize, void* Args[]);
	void Lock_Session_ChangingState(void* Ptr);

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
	queue<ClientSession*> m_queWaitingQueue;
	list<CPlayingRoom*> m_liPlayingRooms;

	/*---------------
		Data
	---------------*/
	bool m_bClientConnected = false;
	int m_iCurrUser = 0; //At Least Me

};

