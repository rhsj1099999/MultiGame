#pragma once

#include "Include.h"

enum class KEY_STATE 
{
	NONE, // 현재 눌리지 않았고, 이전에도 눌리지 않은 상태
	DOWN,  // 현재 처음으로 누른 상태(지금 누른 시점)
	HOLD, // 이전에도 눌렸고, 현재도 누른 상태
	UP  // 이전에는 눌렸으나, 현재 뗀 상태(막 땐 시점)
};

struct tKeyInfo 
{
	KEY_STATE	eState;
	bool		bPrevDown; // 이전에 눌렸는지 확인용

	tKeyInfo() : eState(KEY_STATE::NONE), bPrevDown(false) {} // 기본 생성자
};

class CKeyMgr2
{
private :
	CKeyMgr2();
	~CKeyMgr2();

	
private:
	bool		m_bKeyState[VK_MAX];
	tKeyInfo	m_arrKeyState[VK_MAX];

public:
	void		Update();
	KEY_STATE	Get_Key_State(int _iKey);
	static CKeyMgr2*	m_pInstance;

public:
	static CKeyMgr2* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CKeyMgr2;
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

public:
	bool Key_Down(int _iKey);
	bool Key_Up(int _iKey);
	bool Key_Hold(int _iKey);
};

