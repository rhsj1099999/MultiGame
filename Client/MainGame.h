#pragma once

#include "Include.h"

class CObj;
class CMainGame
{
public:
	CMainGame();
	~CMainGame();
	static CMainGame* Get_Instance()
	{
		static CMainGame Instance;
		return &Instance;
	}


public:
	void		Initialize(void);	// �����͸� �ʱ�ȭ�ϴ� �Լ�
	void		Update(DWORD dwCurrTime);		// �� �����Ӹ��� �����ϴ� �Լ�
	void		Late_Update();
	void		Render(void);		// �� �����Ӹ��� ����ϴ� �Լ�
	void		Release(void);		// ��ü �ı� �� �޸� ���� �Լ�
	DWORD       GetCurrTime() { return m_dwCurrTime; }

private:
	HDC			m_DC;
	HDC         m_DCBackBuffer;
	HBITMAP     m_hBackBufferBitmap;
	DWORD		m_dwCurrTime;
};

