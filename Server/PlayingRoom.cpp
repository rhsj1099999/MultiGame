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



	//ù��°�� ������ ����� ���� -> �׻���� TurnOn

	

}

void CPlayingRoom::Tick()
{
	//�޼��� IO ��ũ�� ���� ���ο��� ������ ���¸� �����
}
