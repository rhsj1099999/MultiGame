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
	_answerHoleIndex = rand() % (HOLE_HORIZON * HOLE_VERTICAL);
	_currUserOrder = rand() % MAXCLIENTS;
	_currUsers = 0;

	for (int i = 0; i < MAXCLIENTS; ++i)
	{
		_roomMembers[i] = pThreeSession[i];

		if (pThreeSession[i] != nullptr)
		{
			_currUsers++;
		}

		MySend<int>(_roomMembers[i], _currUserOrder, PacketHeader::PacketType::TURNCHANGED);
	}
}

void CPlayingRoom::ClientDead(ClientSession* pSession)
{
	int DisconnectedPlayer = 0;

	for (int i = 0; i < MAXCLIENTS; i++)
	{
		if (_roomMembers[i] == pSession)
		{
			DisconnectedPlayer = i;

			_roomMembers[i] = nullptr;

			--_currUsers;
			
			if (_currUsers <= 0)
			{
				CMainServer::GetInstance()->DeleteRoom(this);
				cout << "Room Destroy" << endl;
				return;
			}

			if (_currUserOrder == i)
			{
				while (true)
				{
					_currUserOrder = (_currUserOrder + 1) % MAXCLIENTS;

					if (_roomMembers[_currUserOrder] != nullptr)
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

	if (_isGameEnd == false)
	{
		bTempGameEndSig = true;
	}

	if (_currUsers == 1)
	{
		for (int i = 0; i < MAXCLIENTS; i++)
		{
			if (_roomMembers[i] == nullptr)
				continue;

			//접속종료 메세지
			MySend_Ptr
			(
				_roomMembers[i],
				Buffer_Char,
				sizeof(TempType) + sizeof(DisconnectedPlayer) + CharByte,
				PacketHeader::PacketType::SERVERCHATSHOOT
			);

			if (bTempGameEndSig)
			{
				//우승-게임종료 메세지
				cout << "One Room's Game End / OnePlayer" << endl;
				MySend<int>(_roomMembers[i], _currUserOrder, PacketHeader::PacketType::FORCEDGAMEEND);
			}
		}
	}
	else
	{
		for (int i = 0; i < MAXCLIENTS; i++)
		{
			if (_roomMembers[i] == nullptr)
				continue;

			//접속종료 메세지
			MySend_Ptr(_roomMembers[i], Buffer_Char, sizeof(TempType) + sizeof(DisconnectedPlayer) + CharByte, PacketHeader::PacketType::SERVERCHATSHOOT);

			//턴변경 메세지
			if (_isGameEnd == false)
				MySend<int>(_roomMembers[i], _currUserOrder, PacketHeader::PacketType::TURNCHANGED);
		}
	}
}

void CPlayingRoom::ExecutionMessage_InRoom(PacketHeader::PacketType eType, void* pData, int DataSize)
{
	switch (eType)
	{
	case PacketHeader::PacketType::ROTATEANGLE:
		{
		PAK_ROTATEANGLE* pCasted = static_cast<PAK_ROTATEANGLE*>(pData);

		float fAngle = pCasted->Angle;

		for (int i = 0; i < MAXCLIENTS; ++i)
		{
			if (i == pCasted->RoomSessionDesc.MyNumber)
				continue;

			if (_roomMembers[i] == nullptr)
				continue;
			MySend<float>(_roomMembers[i], fAngle, PacketHeader::PacketType::FOLLOWANGLE);
		}
	}
		break;
	case PacketHeader::PacketType::PLAYERBLADEINSERTED:
		{
		PAK_BLADEINSERT* pCasted = static_cast<PAK_BLADEINSERT*>(pData);

		int InsertedIndex = pCasted->Index;

		if (InsertedIndex == _answerHoleIndex)
		{
			_isGameEnd = true;

			char Temp[8] = {};
			memcpy(Temp, &InsertedIndex, sizeof(int));
			memcpy(&Temp[sizeof(int)], &pCasted->RoomSessionDesc.MyNumber, sizeof(int));

			cout << "One Room's Game End / Find Answer" << endl;

			for (int i = 0; i < MAXCLIENTS; ++i)
			{
				if (_roomMembers[i] == nullptr)
					continue;

				MySend_Ptr(_roomMembers[i], Temp, sizeof(int) * 2, PacketHeader::PacketType::GAMEEND);
			}
		}
		else
		{
			int NextTurn = pCasted->RoomSessionDesc.MyNumber;

			while (true)
			{
				NextTurn = (NextTurn + 1) % MAXCLIENTS;

				if (_roomMembers[NextTurn] != nullptr)
					break;
			}

			_currUserOrder = NextTurn;

			PAK_INSERTFOLLOW TempFollowData = {};
			TempFollowData.HoldIndex = InsertedIndex;
			TempFollowData.PlayerIndex = pCasted->RoomSessionDesc.MyNumber;

			for (int i = 0; i < MAXCLIENTS; ++i)
			{
				if (_roomMembers[i] == nullptr)
					continue;

				if (i != pCasted->RoomSessionDesc.MyNumber)
					MySend<PAK_INSERTFOLLOW>(_roomMembers[i], TempFollowData, PacketHeader::PacketType::FOLLOWINDEX);

				MySend<int>(_roomMembers[i], NextTurn, PacketHeader::PacketType::TURNCHANGED);
			}
		}
	}
		break;
	case PacketHeader::PacketType::CLIENTCHATSHOOT:
		for (int i = 0; i < MAXCLIENTS; i++)
		{
			if (_roomMembers[i] == nullptr)
				continue;

			MySend_Ptr(_roomMembers[i], pData, DataSize, PacketHeader::PacketType::SERVERCHATSHOOT);
		}
		break;

	default:
		{
			SR1_MSGBOX("패킷이 잘못왔네요?");
		}
		break;
	}
}
