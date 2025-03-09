#pragma once
#include "Include.h"
#include "Obj.h"

class CCaseHoles;
class CPirateHead;
class CServerManager
{
public:
	CServerManager();
	~CServerManager();
public:
	int		Update();
	void	Late_Update();
	void	Release();
	void	InitServer();


	//bool ChattingLengthCheck();
	//void ChattingShowOn();
private:
	bool m_bCanChat = false;
public:
	void SetCanChat(bool bCanChat);
	void ChattingUpdate();
	void ShowChattings(HDC hDC);
	void Render(HDC hDC);
	void SendHeartBeat();
	void ConnectTry();

	void ServerDamaged();
	bool GetClientConnected() { return m_bClientConnected; }
	int GetServerUser() { return m_iCurrUser; }
	float GetCurrentAngle() {return m_fCurrAngle;}
	float GetPrevAngle() {return m_fPrevAngle;}
	void SetCurrentAngle(float fAngle) { m_fCurrAngle = fAngle; }
	void SetPreviousAngle(float fAngle) { m_fPrevAngle = fAngle; }
	ClientSession* GetSession() { return m_pSession; }
	PlayingRoomSessionDesc* GetRoomDescPtr();
	PlayingRoomSessionDesc& GetRoomDesc() { return m_tRoomDesc; }



	
public:

public:
	static CServerManager* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CServerManager;
		}
		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

	void SetHoleVecPtr(vector<CCaseHoles*>* Ptr)
	{
		if (m_vecCaseHoles != nullptr)
		{
			SR1_MSGBOX("Init Wrong");
		}
		m_vecCaseHoles = Ptr;
	}

	bool GetMyTurn() { return m_bCanMove; }
	void SetMyTurn(bool bTurn) { m_bCanMove = bTurn; }

	void SetRoomDesc(PlayingRoomSessionDesc& Ptr)
	{
		m_tRoomDesc._myNumber = Ptr._myNumber;
		m_tRoomDesc._myRoomPtr = Ptr._myRoomPtr;
	}

	bool GetIsCanInsert(int Index) { return m_HoleVector[Index]; }
	void SetInsert(int Index) { m_HoleVector[Index] = false; }
	int GetCurrentPlayer() { return m_iCurrentPlayer; }
	void ClearDatas();
private:
	void WorkerEntry_D(HANDLE hHandle, char* pOut, int size = 100);
	bool ExecuetionMessage(PacketHeader::PacketType packetType, void* pPacketData, int DataSize);
	PlayingRoomSessionDesc m_tRoomDesc = {};

private:
	static CServerManager* m_pInstance;
	/*---------------
	Handle, Core, Etc
	---------------*/
	WSADATA m_wsa = {};
	SOCKET m_Socket = {};
	SOCKADDR_IN m_ServerAddr = {};
	vector<thread> m_vecWorkerThreads;
	ClientSession* m_pSession = nullptr;
	HANDLE m_IOCPHandle = INVALID_HANDLE_VALUE;
	vector<bool> m_HoleVector;

	/*---------------
		Data
	---------------*/
	bool m_bClientConnected = false;
	int m_iCurrUser = 1; //At Least Me
	bool m_bCanMove = false;
	float m_fCurrAngle = 0.0;
	float m_fPrevAngle = 0.0;
	int m_iCurrentPlayer = -1;
	bool m_bIsChatReady = false;

	deque<PAK_ChattingMessage> m_Chattings;
	CRITICAL_SECTION m_ChatQueueCS = {};

	bool m_bGameIsEnd = false;
	bool m_bIMWIN = false;
	int m_iWhoWins = -1;
	DWORD m_dwConnectTrying = {};

public:
	void EnterCriticalSection_Chat() { EnterCriticalSection(&m_ChatQueueCS); }
	void LeaveCriticalSection_Chat() { LeaveCriticalSection(&m_ChatQueueCS); }

	bool GetGameEnd() { return m_bGameIsEnd; }
	bool GetWin() { return m_bIMWIN; }
	int GetWhoWin() { return m_iWhoWins; }
	CPirateHead* SetPirateHeadPtr(CPirateHead* Ptr) { return m_pPirateHeadPtr = Ptr; }

private:
	CPirateHead* m_pPirateHeadPtr = nullptr;
	vector<CCaseHoles*>* m_vecCaseHoles = nullptr;
	DWORD m_dwHeartBitCounter = {};
};

