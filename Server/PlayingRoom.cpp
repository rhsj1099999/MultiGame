#include "stdafx.h"
#include "PlayingRoom.h"

CPlayingRoom::CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
}

CPlayingRoom::~CPlayingRoom()
{
}

void CPlayingRoom::Init(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
	m_iPlayingOrder = rand() % CLIENT3;
	for (int i = 0; i < CLIENT3; ++i)
	{
		m_arrClients[i] = pThreeSession[i];
		MySend<int>(m_arrClients[i], m_iPlayingOrder, PREDATA::OrderType::TURNCHANGED);
	}
}

void CPlayingRoom::Tick()
{
	

}
