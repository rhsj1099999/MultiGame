#pragma once
class CPlayingRoom
{
public:
	CPlayingRoom() = delete;
	CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList);
	~CPlayingRoom();

	void Tick();

private:
	float m_fRotated = 0.0f;
	list<int> m_Wholes;
	ClientSession* m_arrClients[3] = { nullptr, };
};

