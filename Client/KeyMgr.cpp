#include "stdafx.h"
#include "KeyMgr.h"

CKeyMgr* CKeyMgr::m_pInstance = nullptr;

CKeyMgr::CKeyMgr()
{
	memset(m_bKeyState, 0, sizeof(m_bKeyState));
}

CKeyMgr::~CKeyMgr()
{
}

bool CKeyMgr::Key_Pressing(int _iKey)
{
	if (GetForegroundWindow() != g_hWnd)
	{
		return false;
	}

	if (GetAsyncKeyState(_iKey) & 0x8000)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CKeyMgr::Key_Down(int _iKey)
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

	for (int i = 0; i < VK_MAX; i++)
	{
		if (m_bKeyState[_iKey] && !(GetAsyncKeyState(_iKey) & 0x8000))
		{
			m_bKeyState[_iKey] = !m_bKeyState[_iKey];
		}
	}
	return false;
}

bool CKeyMgr::Key_Up(int _iKey)
{
	if (GetForegroundWindow() != g_hWnd)
	{
		return false;
	}

	if (m_bKeyState[_iKey] && !(GetAsyncKeyState(_iKey) & 0x8000))
	{
		m_bKeyState[_iKey] = !m_bKeyState[_iKey];
		return true;
	}

	for (int i = 0; i < VK_MAX; i++)
	{
		if (!m_bKeyState[_iKey] && (GetAsyncKeyState(_iKey) & 0x8000))
		{
			m_bKeyState[_iKey] = !m_bKeyState[_iKey];
		}
	}

	return false;
}
