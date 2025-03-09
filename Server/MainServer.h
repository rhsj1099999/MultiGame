#pragma once


class CPlayingRoom;

class CMainServer : public CBase
{
	typedef void (CMainServer::* VFPtr)(void*);

	public:		static CMainServer* GetInstance();
	public:		static void Destroy_Instance();
	private:	static CMainServer* _instance;

	public:		void Release()override;
	public:		CMainServer();
	public:		~CMainServer();

	public:		void DeleteRoom(CPlayingRoom* RoomPtr);
	public:		void Init();
	public:		void Tick();


	private:	void ConnectTry();
	private:	void WorkerEntry_D(HANDLE hHandle);
	private:	void LiveCheck();
	private:	void MatchingRoom();
	private:	bool ExecuetionMessage(PacketHeader::PacketType eType, void* Data, int DataSize);

	private:	void Lock_Queue(VFPtr pFArr[], int ArrSize, void* Args[]);
	private:	void Lock_Queue_Push(void* Ptr);
	private:	void Lock_Queue_ChangingRoom(void* Ptr);
	private:	void Lock_Session(VFPtr pFArr[], int ArrSize, void* Args[]);

	
	private:	WSADATA _wsaData = {};
	private:	SOCKET _serverSocket = {};
	private:	SOCKADDR_IN _serverAddress = {};
	private:	vector<thread> _workerThreads;
	private:	ClientSession* _serverSession = nullptr;
	private:	HANDLE _iocpHandle = INVALID_HANDLE_VALUE;



	private:	int _currUserCount = 0;

	private:	list<ClientSession*> _currClientSessions;
	private:	mutex _mutex_CurrClientSession = {};

	private:	list<ClientSession*> _currWaitingClientSessions;
	private:	mutex _mutex_CurrWaitingClientSession = {};

	private:	list<CPlayingRoom*> _currPlayingRooms;
	private:	mutex _mutex_CurrPlayingRooms = {};



	


};

