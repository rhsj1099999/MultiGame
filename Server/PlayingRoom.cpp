#include "stdafx.h"
#include "PlayingRoom.h"

CPlayingRoom::CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
}

CPlayingRoom::~CPlayingRoom()
{
}

void CPlayingRoom::Init()
{
	m_iPlayingOrder = rand() % 3;

	for (int i = 0; i < 3; ++i)
	{
		if (i == m_iPlayingOrder)
		{
			m_arrClients[m_iPlayingOrder]->eClientState = ClientSession::ClientState::TURNON;
		}
		m_arrClients[m_iPlayingOrder]->eClientState = ClientSession::ClientState::TURNOFF;
	}



	//첫번째로 움직일 사람을 결정 -> 그사람은 TurnOn

	

}

void CPlayingRoom::Tick()
{
	//메세지 IO 싱크에 맞춰 전부에게 렌더링 상태를 맞출것
}
