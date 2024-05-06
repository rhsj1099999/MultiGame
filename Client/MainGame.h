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
	void		Initialize(void);	// 데이터를 초기화하는 함수
	void		Update(DWORD dwCurrTime);		// 매 프레임마다 갱신하는 함수
	void		Late_Update();
	void		Render(void);		// 매 프레임마다 출력하는 함수
	void		Release(void);		// 객체 파괴 시 메모리 해제 함수
	DWORD       GetCurrTime() { return m_dwCurrTime; }

private:
	HDC			m_DC;
	HDC         m_DCBackBuffer;
	HBITMAP     m_hBackBufferBitmap;
	DWORD		m_dwCurrTime;
};

