#pragma once
#include "Include.h"
#include "Obj.h"

class CCaseHoles;
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
	bool GetMyTurn() { return m_bCanMove; }
	void SetMyTurn(bool bTurn) { m_bCanMove = bTurn; }
	void SetRoomDesc(PlayingRoomSessionDesc* Ptr)
	{
		m_tRoomDesc.MyNumber = (*Ptr).MyNumber;
		m_tRoomDesc.MyRoomPtr = (*Ptr).MyRoomPtr;
	}

	bool GetIsCanInsert(int Index) { return m_HoleVector[Index]; }
	void SetInsert(int Index) { m_HoleVector[Index] = false; }
	int GetCurrentPlayer() { return m_iCurrentPlayer; }
private:
	void WorkerEntry_D(HANDLE hHandle, char* pOut, int size = 100);
	bool ExecuetionMessage(PREDATA::OrderType eType, void* Data, int DataSize);
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

	/*---------------
		Data
	---------------*/
	bool m_bClientConnected = false;
	int m_iCurrUser = 1; //At Least Me
	bool m_bCanMove = false;
	float m_fCurrAngle = 0.0;
	float m_fPrevAngle = 0.0;
	int m_iCurrentPlayer = -1;
	vector<bool> m_HoleVector;
public:
	void SetHoleVecPtr(vector<CCaseHoles*>* Ptr) 
	{
		if (m_vecCaseHoles != nullptr)
		{
			SR1_MSGBOX("Init Wrong");
		}
		m_vecCaseHoles = Ptr; 
	}
	vector<CCaseHoles*>* m_vecCaseHoles = nullptr;

};

