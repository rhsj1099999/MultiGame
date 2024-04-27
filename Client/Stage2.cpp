#include "stdafx.h"
#include "Stage2.h"
#include "UIMgr.h"
#include "Roulette.h"
#include "ObjMgr.h"
#include "AbstractFactory.h"
#include "KeyMgr.h"
#include "Arrow.h"
#include "Betting.h"
#include "SceneMgr.h"

CStage2::CStage2()
{
}

CStage2::~CStage2()
{
	Release();
}

void CStage2::Initialize()
{
	CUIMgr::Get_Instance()->Set_RoulletWaiting();
	for (int i = 0; i < 10; i++)
	{
		CObjMgr::Get_Instance()->Add_Object(
			OBJ_ROULETTE,
			CAbstractFactory<CRoulette>::Create_Roulette(i)
		);
	}
	CObjMgr::Get_Instance()->Add_Object(
		OBJ_ARROW,
		CAbstractFactory<CArrow>::Create()
	);
	CObjMgr::Get_Instance()->Add_Object(
		OBJ_BETTING,
		CAbstractFactory<CBetting>::Create()
	);
}

void CStage2::Update()
{
	Key_Input();
	CObjMgr::Get_Instance()->Update();
}

void CStage2::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CStage2::Render(HDC hDC)
{
	CObjMgr::Get_Instance()->Render(hDC);
	CUIMgr::Get_Instance()->Render(hDC);
}

void CStage2::Release()
{
	CObjMgr::Get_Instance()->Release();
	CObjMgr::Get_Instance()->Destroy_Instance();
}

void CStage2::Key_Input()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPACE))
	{
		CObjMgr::Get_Instance()->Spin_Roulette();
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN))
	{
		CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);
	}
}
