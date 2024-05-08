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
				//�������� ������ �ʾ����� ���� ������
				m_arrKeyState[i].bPrevDown = true;
				m_arrKeyState[i].eState = KEY_STATE::DOWN;
			}
			else
			{
				//�������� ���Ⱦ���, ���絵 ������
				m_arrKeyState[i].eState = KEY_STATE::HOLD;
			}
		}
		else
		{
			m_arrKeyState[i].bPrevDown = false;

			if (KEY_STATE::NONE == m_arrKeyState[i].eState || KEY_STATE::UP == m_arrKeyState[i].eState)
			{
				// �������� �ȴ��Ȱų�, ������ �� ������ ��
				m_arrKeyState[i].eState = KEY_STATE::NONE;
			}
			else
			{
				// ������ �ѹ� �����ų�, �� ���� ������ ��
				m_arrKeyState[i].eState = KEY_STATE::UP;
			}
		}
	}
}

KEY_STATE CKeyMgr2::Get_Key_State(int _iKey)
{
	return m_arrKeyState[_iKey].eState;
}