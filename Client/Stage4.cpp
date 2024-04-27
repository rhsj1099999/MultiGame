#include "stdafx.h"
#include "Stage4.h"
#include "AbstractFactory.h"
#include "ObjMgr.h"
#include "UIMgr.h"
#include "SceneMgr.h"
#include "KeyMgr.h"

CStage4::CStage4()
{

}

CStage4::~CStage4()
{
	Release();
}

void CStage4::Initialize()
{
	CObjMgr::Get_Instance()->Add_Object(OBJ_DOWNCASE, CAbstractFactory<CDownCase>::Create());
	CObjMgr::Get_Instance()->Add_Object(OBJ_UPCASE, CAbstractFactory<CUpCase>::Create());
	CObjMgr::Get_Instance()->Add_Object(OBJ_MIDCASE, CAbstractFactory<CMidCase>::Create());
	//만약 보상이 있는곳을 찌르면 업데이트가 바뀜 = 엔터키를 누르면 이전씬으로 가라
}

void CStage4::Update()
{
	if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN))
	{
		CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);
	}
	else
	{
		CObjMgr::Get_Instance()->Update();
	}

}

void CStage4::Late_Update()
{
}

void CStage4::Render(HDC hDC)
{
	CObjMgr::Get_Instance()->Render(hDC);
	CUIMgr::Get_Instance()->Render(hDC);
}

void CStage4::Release()
{
	CObjMgr::Get_Instance()->Release();
	CObjMgr::Get_Instance()->Destroy_Instance();
}
