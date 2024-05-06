#include "stdafx.h"
#include "PlayingRoom.h"

CPlayingRoom::CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
	m_arrClients[0] = pThreeSession[0];
	m_arrClients[1] = pThreeSession[1];
	m_arrClients[2] = pThreeSession[2];
}

CPlayingRoom::~CPlayingRoom()
{
}

void CPlayingRoom::Init(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
    m_InsertedIndices.resize(15);

	m_arrClients[0] = pThreeSession[0];
	m_arrClients[1] = pThreeSession[1];
	m_arrClients[2] = pThreeSession[2];

	m_iPlayingOrder = rand() % 1;

	m_arrClients[m_iPlayingOrder]->eClientState = ClientSession::ClientState::TURNON;

    for (int i = 0; i < CLIENT3; i++)
    {
        ClientSession* pSession = m_arrClients[i];

        MySend<int>(pSession, m_iPlayingOrder, PREDATA::OrderType::TURNCHANGED);
    }
}

void CPlayingRoom::Tick()
{


}
