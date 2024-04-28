#pragma once
#include "Scene.h"

class Stage_Matching : public CScene
{
public:
	Stage_Matching();
	virtual ~Stage_Matching();
public:
	// CScene을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;

private:
	char m_Buffer[BUFSIZE] = {};
	bool m_bClientConnected = false;
	WSADATA m_wsa = {};
	SOCKET m_Socket = {};
	SOCKADDR_IN m_ServerAddr = {};
	vector<thread> m_vecWorkerThreads;

	OverlappedExtend* m_pOverlapEX = nullptr;
	WSABUF m_wsaBuffer;


};