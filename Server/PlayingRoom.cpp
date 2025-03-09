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

	PAK_TurnChanged packetTurnChange = PAK_TurnChanged(_currUserOrder);

	for (int i = 0; i < MAXCLIENTS; ++i)
	{
		_roomMembers[i] = pThreeSession[i];

		if (pThreeSession[i] != nullptr)
		{
			_currUsers++;
		}

		MySend<PAK_TurnChanged>(_roomMembers[i], packetTurnChange, PacketHeader::PacketType::TURNCHANGED);
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
				return;
			}

			_isGameEnd = (_currUsers == 1);

			break;
		}
	}

	bool turnChanged = (_currUserOrder == DisconnectedPlayer);
	if (turnChanged == true)
	{
		while (true)
		{
			_currUserOrder = (_currUserOrder + 1) % MAXCLIENTS;

			if (_roomMembers[_currUserOrder] != nullptr)
			{
				break;
			}
		}
	}



	wchar_t Buffer[MAX_PATH] = {};
	wsprintf(Buffer, L"Player [ %d ] Disconnected.", DisconnectedPlayer);
	

	PAK_ChattingToSingle chattingPacketToClient = PAK_ChattingToSingle
	(
		PAK_ChattingMessage::MSGType::Sys,
		0,
		Buffer
	);

	PAK_TurnChanged turnChangedPacket = PAK_TurnChanged
	(
		_currUserOrder
	);


	for (int i = 0; i < MAXCLIENTS; i++)
	{
		if (_roomMembers[i] == nullptr)
		{
			continue;
		}

		MySend<PAK_ChattingToSingle>(_roomMembers[i], chattingPacketToClient, PacketHeader::PacketType::SERVERCHATSHOOT);

		if (turnChanged == true && _isGameEnd == false)
		{
			MySend<PAK_TurnChanged>(_roomMembers[i], turnChangedPacket, PacketHeader::PacketType::TURNCHANGED);
		}
	}


	if (_isGameEnd == true)
	{
		PAK_GameEnd packetGameEnd = PAK_GameEnd
		(
			_currUserOrder
		);

		MySend<PAK_GameEnd>(_roomMembers[_currUserOrder], packetGameEnd, PacketHeader::PacketType::FORCEDGAMEEND);
	}
}


void CPlayingRoom::BroadCast_RotateAngle(PAK_ROTATEANGLE* pRotateAngleFollow)
{
	PAK_ROTATEANGLE_Follow packet_AngleFollow = PAK_ROTATEANGLE_Follow
	(
		pRotateAngleFollow->_angle
	);

	for (int i = 0; i < MAXCLIENTS; ++i)
	{
		if (_roomMembers[i] == nullptr)
		{
			continue;
		}

		if (i == pRotateAngleFollow->_myNumber)
		{
			continue;
		}

		MySend<PAK_ROTATEANGLE_Follow>(_roomMembers[i], packet_AngleFollow, PacketHeader::PacketType::FOLLOWANGLE);
	}
}

void CPlayingRoom::BroadCast_BladeInserted(PAK_BLADEINSERT* pBladeInsertedPacket)
{
	//°ËÁõµé
	{
		if (pBladeInsertedPacket->_myNumber != _currUserOrder)
		{
			//ÇöÀç ¼ø¼­°¡ ¾Æ´Ñ¾Ö°¡ Ä®À» ²Å¾Ò´Ù
			return;
		}

		if (_roomMembers[pBladeInsertedPacket->_myNumber] == nullptr)
		{
			//Ä® ²ÅÀº ¾ê´Â ²÷±ä¾ê´Ù
			return;
		}
	}

	PAK_INSERTFOLLOW bladeInsertFollowPacket = PAK_INSERTFOLLOW
	(
		pBladeInsertedPacket->_myNumber,
		pBladeInsertedPacket->_insertedIndex
	);

	//Á¤´äÀÎ°¡¿ä?
	{
		_isGameEnd = (pBladeInsertedPacket->_insertedIndex == _answerHoleIndex);
	}

	PacketBase* packet_isGameEnd = nullptr;

	if (_isGameEnd == true)
	{
		PAK_GameEnd gameEndPacket = PAK_GameEnd(pBladeInsertedPacket->_myNumber);
		packet_isGameEnd = &gameEndPacket;
	}
	else
	{
		_currUserOrder = pBladeInsertedPacket->_myNumber + 1;
		while (true)
		{
			if (_currUserOrder == MAXCLIENTS)
			{
				_currUserOrder = 0;
			}

			if (_roomMembers[_currUserOrder] != nullptr)
			{
				break;
			}

			_currUserOrder++;
		}

		PAK_TurnChanged turnChangedPacket = PAK_TurnChanged(_currUserOrder);
		packet_isGameEnd = &turnChangedPacket;
	}


	//Å¬¶óµé¿¡°Ô Ä® ²ÅÈù°÷ µ¿±âÈ­¸¦ À§ÇÑ ÆÐÅ¶ º¸³»±â
	for (int i = 0; i < MAXCLIENTS; i++)
	{
		if (_roomMembers[i] == nullptr)
		{
			continue;
		}

		MySend<PAK_INSERTFOLLOW>(_roomMembers[i], bladeInsertFollowPacket, PacketHeader::PacketType::FOLLOWINDEX);

		if (_isGameEnd == true)
		{
			MySend<PAK_GameEnd>(_roomMembers[i], *(static_cast<PAK_GameEnd*>(packet_isGameEnd)), PacketHeader::PacketType::GAMEEND);
		}
		else
		{
			MySend<PAK_TurnChanged>(_roomMembers[i], *(static_cast<PAK_TurnChanged*>(packet_isGameEnd)), PacketHeader::PacketType::TURNCHANGED);
		}
	}
}

void CPlayingRoom::BroadCast_ClientChattingShoot(PAK_ChattingMessageToRoom* pClientChattingMessageToRoom)
{
	wstring messageReArrange;
	messageReArrange = L"User[" + std::to_wstring(pClientChattingMessageToRoom->_myNumber) + L"] : ";
	messageReArrange.append(pClientChattingMessageToRoom->_message);

	wchar_t buffer[MAX_PATH];
	wcscpy_s(buffer, messageReArrange.c_str());

	PAK_ChattingToSingle chattingPacketToClient = PAK_ChattingToSingle
	(
		pClientChattingMessageToRoom->_messageType,
		pClientChattingMessageToRoom->_time,
		buffer
	);

	for (int i = 0; i < MAXCLIENTS; i++)
	{
		if (_roomMembers[i] == nullptr)
			continue;

		MySend<PAK_ChattingToSingle>(_roomMembers[i], chattingPacketToClient, PacketHeader::PacketType::SERVERCHATSHOOT);
	}
}
