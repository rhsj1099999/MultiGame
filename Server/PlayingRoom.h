#pragma once
class CPlayingRoom
{
public:
	CPlayingRoom() {};
	CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList);
	~CPlayingRoom();

	void Init(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList);
	void Tick();
	float GetCurrAngle() { return m_fCurrAngle; }
	void SetCurrAngle(float fAngle) { m_fCurrAngle = fAngle; }
	ClientSession** GetClients() { return m_arrClients; }

private:
	float m_fRotated = 0.0f;
	list<int> m_Wholes;
	ClientSession* m_arrClients[CLIENT3] = { nullptr, };
	int m_iPlayingOrder = 0;


	float m_fCurrAngle = 0.0f;
	vector<bool> m_InsertedIndices; //Ä®ÀÌ ²ÅÈù°÷µé
};

