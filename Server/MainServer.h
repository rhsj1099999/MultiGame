#pragma once






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

private:
	void ConnectTry();
	void TickWatingClients();
	void WorkerEntry_D(HANDLE hHandle);
	void LiveCheck();
	void MatchingRoom();
	bool ExecuetionMessage(PREDATA::OrderType eType, void* Data, int DataSize);

	void Lock_Queue(VFPtr pFArr[], int ArrSize, void* Args[]);
	void Lock_Queue_Push(void* Ptr);
	void Lock_Queue_ChangingRoom(void* Ptr);

	void Lock_Session(VFPtr pFArr[], int ArrSize, void* Args[]);
	void Lock_Session_ChangingState(void* Ptr);

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



	/*---------------
		Data
	---------------*/
	bool m_bClientConnected = false;
	int m_iCurrUser = 0;

	mutex m_ClassDataLock = {};
	list<ClientSession*> m_liClientSockets;
	queue<ClientSession*> m_queWaitingQueue;
	list<CPlayingRoom*> m_liPlayingRooms;

};

