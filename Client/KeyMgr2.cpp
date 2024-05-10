#include "stdafx.h"
#include "KeyMgr2.h"

CKeyMgr2* CKeyMgr2::m_pInstance = nullptr;

CKeyMgr2::CKeyMgr2()
	: m_bKeyState()
{

}

CKeyMgr2::~CKeyMgr2()
{

}

bool CKeyMgr2::Key_Down(int _iKey)
{
	if (GetForegroundWindow() != g_hWnd)
	{
		return false;
	}

	if (!m_bKeyState[_iKey] && (GetAsyncKeyState(_iKey) & 0x8000))
	{
		m_bKeyState[_iKey] = !m_bKeyState[_iKey];
		return true;
	}

	for (size_t i = 0; i < VK_MAX; ++i)
	{
		if (m_bKeyState[i] && !(GetAsyncKeyState(i) & 0x8000))
			m_bKeyState[i] = !m_bKeyState[i];
	}
	
	return false;
}

bool CKeyMgr2::Key_Up(int _iKey)
{
	return false;
}

bool CKeyMgr2::Key_Hold(int _iKey)
{
	return false;
}

void CKeyMgr2::Update()
{
	for (size_t i = 0; i < VK_MAX; ++i)
	{
		if (GetAsyncKeyState(i) & 0x8000)
		{
			if ( !m_arrKeyState[i].bPrevDown && KEY_STATE::NONE == m_arrKeyState[i].eState )
			{
				//이전에는 눌리지 않았으나 현재 눌렸음
				m_arrKeyState[i].bPrevDown = true;
				m_arrKeyState[i].eState = KEY_STATE::DOWN;
			}
			else
			{
				//이전에도 눌렸었고, 현재도 눌렸음
				m_arrKeyState[i].eState = KEY_STATE::HOLD;
			}
		}
		else
		{
			m_arrKeyState[i].bPrevDown = false;

			if (KEY_STATE::NONE == m_arrKeyState[i].eState || KEY_STATE::UP == m_arrKeyState[i].eState)
			{
				// 이전에도 안눌렸거나, 이전에 뗀 상태일 때
				m_arrKeyState[i].eState = KEY_STATE::NONE;
			}
			else
			{
				// 이전에 한번 눌렀거나, 꾹 누른 상태일 때
				m_arrKeyState[i].eState = KEY_STATE::UP;
			}
		}
	}
}

KEY_STATE CKeyMgr2::Get_Key_State(int _iKey)
{
	return m_arrKeyState[_iKey].eState;
}