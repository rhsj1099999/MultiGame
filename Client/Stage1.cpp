#include "stdafx.h"
#include "Stage1.h"
#include "ObjMgr.h"
#include "CollisionMgr.h"
#include "AbstractFactory.h"
#include "MolePlayer.h"
#include "UIMgr.h"
#include "Tunnel.h"
#include "SceneMgr.h"
#include "KeyMgr.h"

CStage1::CStage1()
{
}

CStage1::~CStage1()
{
	Release();
}

void CStage1::Initialize()
{
	//두더지 마릿수 초기화
	CUIMgr::Get_Instance()->Set_MoleCnt(0);

	//두더지잡기 플레이어 생성
	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLEPLAYER, CAbstractFactory<CMolePlayer>::Create());

	//두더지땅굴 생성
	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLETUNNEL, CAbstractFactory<CTunnel>::Create(300.f, 160.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLETUNNEL, CAbstractFactory<CTunnel>::Create(500.f, 160.f));

	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLETUNNEL, CAbstractFactory<CTunnel>::Create(200.f, 360.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLETUNNEL, CAbstractFactory<CTunnel>::Create(400.f, 360.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLETUNNEL, CAbstractFactory<CTunnel>::Create(600.f, 360.f));

	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLETUNNEL, CAbstractFactory<CTunnel>::Create(300.f, 540.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_MOLETUNNEL, CAbstractFactory<CTunnel>::Create(500.f, 540.f));

}

void CStage1::Update()
{
	CObjMgr::Get_Instance()->Update();

	// 임시 테스트용
	if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN))
	{
		CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);
	}
}

void CStage1::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();

	//두더지 50마리 잡으면 월드스테이지로 이동
	if (CUIMgr::Get_Instance()->Get_MoleCnt() == 50)
		CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);

}

void CStage1::Render(HDC hDC)
{
	CObjMgr::Get_Instance()->Render(hDC);
	CUIMgr::Get_Instance()->Render(hDC);
}

void CStage1::Release()
{
	CObjMgr::Get_Instance()->Destroy_Instance();
}
