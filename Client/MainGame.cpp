#include "stdafx.h"
#include "MainGame.h"
#include "Player.h"
#include "Monster.h"
#include "SceneMgr.h"
#include "KeyMgr.h"
#include "UIMgr.h"
#include "ObjMgr.h"
#include "KeyMgr2.h"
#include "CServerManager.h"

CMainGame::CMainGame()
{
}


CMainGame::~CMainGame()
{
	Release();
}

void CMainGame::Initialize(void)
{
	srand((unsigned int)time(NULL));
	m_DC = GetDC(g_hWnd);
	// ¹é¹öÆÛ
	m_DCBackBuffer = CreateCompatibleDC(m_DC);
	m_hBackBufferBitmap = CreateCompatibleBitmap(m_DC, WINCX, WINCY);
	DeleteObject(SelectObject(m_DCBackBuffer, m_hBackBufferBitmap));

	CUIMgr::Get_Instance()->Initialize();
	CSceneMgr::Get_Instance()->Scene_Change(SCENEID::SC_WORLDMAP);

}

void CMainGame::Update(void)
{
	CKeyMgr2::Get_Instance()->Update();
	CSceneMgr::Get_Instance()->Update();
}

void CMainGame::Late_Update()
{
	CSceneMgr::Get_Instance()->Late_Update();
}

void CMainGame::Render(void)
{
	Rectangle(m_DCBackBuffer, 0, 0, WINCX, WINCY);
	CSceneMgr::Get_Instance()->Render(m_DCBackBuffer);
	BitBlt(m_DC, 0, 0, WINCX, WINCY, m_DCBackBuffer, 0, 0, SRCCOPY);
}

void CMainGame::Release(void)
{
	CObjMgr::Get_Instance()->Release();
	CSceneMgr::Get_Instance()->Release();
	//CServerManager::Get_Instance()->Release();

	CObjMgr::Get_Instance()->Destroy_Instance();
	CUIMgr::Get_Instance()->Destroy_Instance();
	CKeyMgr::Get_Instance()->Destroy_Instance();
	CSceneMgr::Get_Instance()->Destroy_Instance();
	CKeyMgr2::Get_Instance()->Destroy_Instance();
	CServerManager::Get_Instance()->Destroy_Instance();
	ReleaseDC(g_hWnd, m_DC);
}
