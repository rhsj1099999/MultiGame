#include "stdafx.h"
#include "WorldMap.h"
#include "AbstractFactory.h"
#include "UIMgr.h"
#include "ObjMgr.h"
#include "SceneMgr.h"

CWorldMap::CWorldMap()
{
}

CWorldMap::~CWorldMap()
{
	Release();
}

void CWorldMap::Initialize()
{
	CObjMgr::Get_Instance()->Add_Object(OBJ_PLAYER, CAbstractFactory<CPlayer>::Create(400, 300));
	CObjMgr::Get_Instance()->Add_Object(OBJ_PORTAL, CAbstractFactory<CPortal>::Create_Portal(100, 50, 0));
	CObjMgr::Get_Instance()->Add_Object(OBJ_PORTAL, CAbstractFactory<CPortal>::Create_Portal(700, 50, 1));
	CObjMgr::Get_Instance()->Add_Object(OBJ_PORTAL, CAbstractFactory<CPortal>::Create_Portal(100, 550, 2));
	//CObjMgr::Get_Instance()->Add_Object(OBJ_PORTAL, CAbstractFactory<CPortal>::Create_Portal(700, 550, 4));
	//// Exit ��Ż
	//CObjMgr::Get_Instance()->Add_Object(OBJ_PORTAL, CAbstractFactory<CPortal>::Create_Portal(700, 300, 5));
}

void CWorldMap::Update()
{
	CObjMgr::Get_Instance()->Update();
}

void CWorldMap::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CWorldMap::Render(HDC hDC)
{
	if (CUIMgr::Get_Instance()->Get_Money() < 100)
	{
		TCHAR szBuff[64];
		wsprintf(szBuff, L"%s", L"����� �����Դϴ�. 'F'Ű�� ���� ���α��� ���޹�������.");
		TextOut(hDC, 200, 400, szBuff, lstrlen(szBuff));
	}

	
	CObjMgr::Get_Instance()->Render(hDC);
	CUIMgr::Get_Instance()->Render(hDC);
}

void CWorldMap::Release()
{
	CObjMgr::Get_Instance()->Release();
}
