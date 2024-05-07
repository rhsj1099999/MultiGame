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

private:
	float m_fRotated = 0.0f;
	int m_iPlayingOrder = 0;
	ClientSession* m_arrClients[CLIENT3] = { nullptr, };


	vector<bool> m_InsertedIndices; //Ä®ÀÌ ²ÅÈù°÷µé
};

