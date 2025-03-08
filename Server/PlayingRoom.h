#pragma once
class CPlayingRoom
{
public:
	CPlayingRoom() {};
	CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList);
	~CPlayingRoom();

	void Init(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList);
	void Tick();
	ClientSession** GetClients() { return m_arrClients; }

	void ClientDead(ClientSession* pSession);
	void ExecutionMessage_InRoom(PacketHeader::PacketType eType, void* pData, int DataSize);

private:
	float m_fRotated = 0.0f;
	int m_iPlayingOrder = 0;
	int m_iCurrPlayer = MAXCLIENTS;
	ClientSession* m_arrClients[MAXCLIENTS] = { nullptr, };
	int m_iAnswerHole = 0;
	bool m_bIsGameEnd = false;


	vector<bool> m_InsertedIndices; //Ä®ÀÌ ²ÅÈù°÷µé
};

