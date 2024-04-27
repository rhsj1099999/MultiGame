#pragma once

#include "Include.h"

enum class KEY_STATE 
{
	NONE, // ���� ������ �ʾҰ�, �������� ������ ���� ����
	DOWN,  // ���� ó������ ���� ����(���� ���� ����)
	HOLD, // �������� ���Ȱ�, ���絵 ���� ����
	UP  // �������� ��������, ���� �� ����(�� �� ����)
};

struct tKeyInfo 
{
	KEY_STATE	eState;
	bool		bPrevDown; // ������ ���ȴ��� Ȯ�ο�

	tKeyInfo() : eState(KEY_STATE::NONE), bPrevDown(false) {} // �⺻ ������
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

