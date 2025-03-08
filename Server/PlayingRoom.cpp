#include "stdafx.h"
#include "PlayingRoom.h"
#include "MainServer.h"

CPlayingRoom::CPlayingRoom(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
}

CPlayingRoom::~CPlayingRoom()
{
}

void CPlayingRoom::Init(ClientSession* pThreeSession[], list<ClientSession*>& pReturnList)
{
	m_iAnswerHole = rand() % (HOLE_HORIZON * HOLE_VERTICAL);
	m_iPlayingOrder = rand() % MAXCLIENTS;

	for (int i = 0; i < MAXCLIENTS; ++i)
	{
		m_arrClients[i] = pThreeSession[i];
		MySend<int>(m_arrClients[i], m_iPlayingOrder, PacketHeader::PacketType::TURNCHANGED);
	}
}

void CPlayingRoom::Tick()
{
	

}

void CPlayingRoom::ClientDead(ClientSession* pSession)
{
	int DisconnectedPlayer = 0;

	for (int i = 0; i < MAXCLIENTS; i++)
	{
		if (m_arrClients[i] == pSession)
		{
			DisconnectedPlayer = i;

			m_arrClients[i] = nullptr;

			--m_iCurrPlayer;
			
			if (m_iCurrPlayer <= 0)
			{
				CMainServer::GetInstance()->DeleteRoom(this);
				cout << "Room Destroy" << endl;
				return;
			}

			if (m_iPlayingOrder == i)
			{
				while (true)
				{
					m_iPlayingOrder = (m_iPlayingOrder + 1) % MAXCLIENTS;

					if (m_arrClients[m_iPlayingOrder] != nullptr)
						break;
				}
			}
		}
	}



	char Buffer_Char[MAX_PATH] = {};

	PAK_ChattingMessage::MSGType TempType = PAK_ChattingMessage::MSGType::Sys;
	memcpy(Buffer_Char, &TempType, sizeof(TempType));
	memcpy(&Buffer_Char[sizeof(TempType)], &DisconnectedPlayer, sizeof(DisconnectedPlayer));

	wchar_t Buffer[MAX_PATH] = {};
	wsprintf(Buffer, L"Player [ %d ] Disconnected.", DisconnectedPlayer);
	int CharByte = (lstrlenW(Buffer) * 2) + NULLSIZE;
	memcpy(&Buffer_Char[sizeof(TempType) + sizeof(DisconnectedPlayer)], &Buffer, CharByte);
	
	bool bTempGameEndSig = false;

	if (m_bIsGameEnd == false)
	{
		bTempGameEndSig = true;
	}

	if (m_iCurrPlayer == 1)
	{
		for (int i = 0; i < MAXCLIENTS; i++)
		{
			if (m_arrClients[i] == nullptr)
				continue;

			//접속종료 메세지
			MySend_Ptr(m_arrClients[i], Buffer_Char, sizeof(TempType) + sizeof(DisconnectedPlayer) + CharByte, PacketHeader::PacketType::SERVERCHATSHOOT);

			if (bTempGameEndSig)
			{
				//우승-게임종료 메세지
				cout << "One Room's Game End / OnePlayer" << endl;
				MySend<int>(m_arrClients[i], m_iPlayingOrder, PacketHeader::PacketType::FORCEDGAMEEND);
			}
		}
	}
	else
	{
		for (int i = 0; i < MAXCLIENTS; i++)
		{
			if (m_arrClients[i] == nullptr)
				continue;

			//접속종료 메세지
			MySend_Ptr(m_arrClients[i], Buffer_Char, sizeof(TempType) + sizeof(DisconnectedPlayer) + CharByte, PacketHeader::PacketType::SERVERCHATSHOOT);

			//턴변경 메세지
			if (m_bIsGameEnd == false)
				MySend<int>(m_arrClients[i], m_iPlayingOrder, PacketHeader::PacketType::TURNCHANGED);
		}
	}
}

void CPlayingRoom::ExecutionMessage_InRoom(PacketHeader::PacketType eType, void* pData, int DataSize)
{
	switch (eType)
	{
	case PacketHeader::PacketType::USERCOUNT:
		break;
	case PacketHeader::PacketType::MESSAGECHANGE:
		break;
	case PacketHeader::PacketType::SCENECHANGE_TOPLAY:
		break;
	case PacketHeader::PacketType::SCENECHANGE_TOWORLD:
		break;
	case PacketHeader::PacketType::TURNOFF:
		break;
	case PacketHeader::PacketType::TURNON:
		break;
	case PacketHeader::PacketType::TURNCHANGED:
		break;
	case PacketHeader::PacketType::ROTATEANGLE:
		{
		PAK_ROTATEANGLE* pCasted = static_cast<PAK_ROTATEANGLE*>(pData);

		float fAngle = pCasted->Angle;

		for (int i = 0; i < MAXCLIENTS; ++i)
		{
			if (i == pCasted->RoomSessionDesc.MyNumber)
				continue;

			if (m_arrClients[i] == nullptr)
				continue;
			MySend<float>(m_arrClients[i], fAngle, PacketHeader::PacketType::FOLLOWANGLE);
		}
	}
		break;
	case PacketHeader::PacketType::FOLLOWANGLE:
		break;
	case PacketHeader::PacketType::PLAYERBLADEINSERTTRY:
		break;
	case PacketHeader::PacketType::PLAYERBLADEINSERTED:
		{
		PAK_BLADEINSERT* pCasted = static_cast<PAK_BLADEINSERT*>(pData);

		int InsertedIndex = pCasted->Index;

		if (InsertedIndex == m_iAnswerHole)
		{
			m_bIsGameEnd = true;

			char Temp[8] = {};
			memcpy(Temp, &InsertedIndex, sizeof(int));
			memcpy(&Temp[sizeof(int)], &pCasted->RoomSessionDesc.MyNumber, sizeof(int));

			cout << "One Room's Game End / Find Answer" << endl;

			for (int i = 0; i < MAXCLIENTS; ++i)
			{
				if (m_arrClients[i] == nullptr)
					continue;

				MySend_Ptr(m_arrClients[i], Temp, sizeof(int) * 2, PacketHeader::PacketType::GAMEEND);
			}
		}
		else
		{
			int NextTurn = pCasted->RoomSessionDesc.MyNumber;

			while (true)
			{
				NextTurn = (NextTurn + 1) % MAXCLIENTS;

				if (m_arrClients[NextTurn] != nullptr)
					break;
			}

			m_iPlayingOrder = NextTurn;

			PAK_INSERTFOLLOW TempFollowData = {};
			TempFollowData.HoldIndex = InsertedIndex;
			TempFollowData.PlayerIndex = pCasted->RoomSessionDesc.MyNumber;

			for (int i = 0; i < MAXCLIENTS; ++i)
			{
				if (m_arrClients[i] == nullptr)
					continue;

				if (i != pCasted->RoomSessionDesc.MyNumber)
					MySend<PAK_INSERTFOLLOW>(m_arrClients[i], TempFollowData, PacketHeader::PacketType::FOLLOWINDEX);

				MySend<int>(m_arrClients[i], NextTurn, PacketHeader::PacketType::TURNCHANGED);
			}
		}
	}
		break;
	case PacketHeader::PacketType::FOLLOWINDEX:
		break;
	case PacketHeader::PacketType::HEARTBEAT:
		break;
	case PacketHeader::PacketType::SERVERCHATSHOOT:
		break;
	case PacketHeader::PacketType::CLIENTCHATSHOOT:
		for (int i = 0; i < MAXCLIENTS; i++)
		{
			if (m_arrClients[i] == nullptr)
				continue;

			MySend_Ptr(m_arrClients[i], pData, DataSize, PacketHeader::PacketType::SERVERCHATSHOOT);
		}
		break;
	case PacketHeader::PacketType::SOMECLIENTDEAD:
		break;
	case PacketHeader::PacketType::END:
		break;
	default:
		break;
	}
}
