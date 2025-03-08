#pragma once
class CPlayingRoom
{
	public:		CPlayingRoom() {};
	public:		CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList);
	public:		~CPlayingRoom();

	public:		ClientSession** GetClients() { return _roomMembers; }
	public:		void Init(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList);
	public:		void ClientDead(ClientSession* pSession);
	public:		void ExecutionMessage_InRoom(PacketHeader::PacketType eType, void* pData, int DataSize);



	private:	ClientSession* _roomMembers[MAXCLIENTS] = { nullptr, };
	private:	vector<bool> _insertedHoles; 
	private:	float _cylinerRotated = 0.0f;
	private:	int _currUserOrder = 0;
	private:	int _currUsers = 0;
	private:	int _answerHoleIndex = 0;
	private:	bool _isGameEnd = false;
};

