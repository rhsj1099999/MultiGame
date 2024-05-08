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
	m_iAnswerHole = rand() % (HOLE_HORIZON * HOLE_VERTICAL);

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

void CPlayingRoom::ClientDead(ClientSession* pSession)
{
	int DisconnectedPlayer = 0;

	for (int i = 0; i < CLIENT3; i++)
	{
		if (m_arrClients[i] == pSession)
		{
			DisconnectedPlayer = i;

			m_arrClients[i] = nullptr;

			--m_iCurrPlayer;
			
			if (m_iCurrPlayer <= 0)
			{
				delete this;
				cout << "Room Destroy" << endl;
				return;
			}

			if (m_iPlayingOrder == i)
			{
				while (true)
				{
					m_iPlayingOrder = (m_iPlayingOrder + 1) % CLIENT3;

					if (m_arrClients[m_iPlayingOrder] != nullptr)
						break;
				}
			}
		}
	}
	char Buffer_Char[MAXCHATLEN_TOC] = {};
	MSGType TempType = MSGType::Sys;
	memcpy(Buffer_Char, &TempType, sizeof(TempType));
	memcpy(&Buffer_Char[sizeof(TempType)], &DisconnectedPlayer, sizeof(DisconnectedPlayer));

	wchar_t Buffer[MAX_PATH] = {};
	wsprintf(Buffer, L"Player [ %d ] Disconnected.", DisconnectedPlayer);
	
	WideCharToMultiByte(CP_UTF8, 0, Buffer, -1, &Buffer_Char[sizeof(TempType) + sizeof(DisconnectedPlayer)], sizeof(Buffer), NULL, NULL);

	int Byte = lstrlenW(Buffer) * 2;
	Byte += NULLSIZE + sizeof(TempType) + sizeof(DisconnectedPlayer);

	for (int i = 0; i < CLIENT3; i++)
	{
		if (m_arrClients[i] == nullptr)
			continue;

		MySend_Ptr(m_arrClients[i], Buffer_Char, Byte, PREDATA::OrderType::SERVERCHATSHOOT);
		MySend<int>(m_arrClients[i], m_iPlayingOrder, PREDATA::OrderType::TURNCHANGED);
	}


}

void CPlayingRoom::ExecutionMessage_InRoom(PREDATA::OrderType eType, void* pData, int DataSize)
{
	switch (eType)
	{
	case PREDATA::OrderType::USERCOUNT:
		break;
	case PREDATA::OrderType::MESSAGECHANGE:
		break;
	case PREDATA::OrderType::SCENECHANGE_TOPLAY:
		break;
	case PREDATA::OrderType::SCENECHANGE_TOWORLD:
		break;
	case PREDATA::OrderType::TURNOFF:
		break;
	case PREDATA::OrderType::TURNON:
		break;
	case PREDATA::OrderType::TURNCHANGED:
		break;
	case PREDATA::OrderType::ROTATEANGLE:
	{
		PAK_ROTATEANGLE* pCasted = static_cast<PAK_ROTATEANGLE*>(pData);

		float fAngle = pCasted->Angle;

		for (int i = 0; i < CLIENT3; ++i)
		{
			if (i == pCasted->RoomSessionDesc.MyNumber)
				continue;

			if (m_arrClients[i] == nullptr)
				continue;
			MySend<float>(m_arrClients[i], fAngle, PREDATA::OrderType::FOLLOWANGLE);
		}
	}

		break;
	case PREDATA::OrderType::FOLLOWANGLE:
		break;
	case PREDATA::OrderType::PLAYERBLADEINSERTTRY:
		break;
	case PREDATA::OrderType::PLAYERBLADEINSERTED:
	{
		PAK_BLADEINSERT* pCasted = static_cast<PAK_BLADEINSERT*>(pData);

		int InsertedIndex = pCasted->Index;

		if (InsertedIndex == m_iAnswerHole)
		{
			char Temp[8] = {};
			memcpy(Temp, &InsertedIndex, sizeof(int));
			memcpy(&Temp[sizeof(int)], &pCasted->RoomSessionDesc.MyNumber, sizeof(int));

			for (int i = 0; i < CLIENT3; ++i)
			{
				if (m_arrClients[i] == nullptr)
					continue;

				MySend_Ptr(m_arrClients[i], Temp, sizeof(int) * 2, PREDATA::OrderType::GAMEEND);
			}
		}
		else
		{
			int NextTurn = pCasted->RoomSessionDesc.MyNumber;

			while (true)
			{
				NextTurn = (NextTurn + 1) % CLIENT3;

				if (m_arrClients[NextTurn] != nullptr)
					break;
			}

			m_iPlayingOrder = NextTurn;

			PAK_INSERTFOLLOW TempFollowData = {};
			TempFollowData.HoldIndex = InsertedIndex;
			TempFollowData.PlayerIndex = pCasted->RoomSessionDesc.MyNumber;

			for (int i = 0; i < CLIENT3; ++i)
			{
				if (m_arrClients[i] == nullptr)
					continue;

				if (i != pCasted->RoomSessionDesc.MyNumber)
					MySend<PAK_INSERTFOLLOW>(m_arrClients[i], TempFollowData, PREDATA::OrderType::FOLLOWINDEX);

				MySend<int>(m_arrClients[i], NextTurn, PREDATA::OrderType::TURNCHANGED);
			}
		}
	}
		break;

	case PREDATA::OrderType::FOLLOWINDEX:
		break;
	case PREDATA::OrderType::HEARTBEAT:
		break;
	case PREDATA::OrderType::SERVERCHATSHOOT:
		break;
	case PREDATA::OrderType::CLIENTCHATSHOOT:
		for (int i = 0; i < CLIENT3; i++)
		{
			if (m_arrClients[i] == nullptr)
				continue;

			MySend_Ptr(m_arrClients[i], pData, DataSize, PREDATA::OrderType::SERVERCHATSHOOT);
		}

		break;
	case PREDATA::OrderType::SOMECLIENTDEAD:
		break;
	case PREDATA::OrderType::END:
		break;
	default:
		break;
	}
}
