#include "stdafx.h"
#include "Stage3.h"
#include "FishingRod.h"
#include "Fish.h"

#include "ObjMgr.h"
#include "AbstractFactory.h"
#include "UIMgr.h"
#include "KeyMgr.h"
#include "SceneMgr.h"

CStage3::CStage3()
{
}

CStage3::~CStage3()
{
	Release();
}

void CStage3::Initialize()
{
	CObjMgr::Get_Instance()->Add_Object(OBJ_FISHINGROD, CAbstractFactory<CFishingRod>::Create());

	CObjMgr::Get_Instance()->Add_Object(OBJ_FISH, CAbstractFactory<CFish>::Create(500.f, 500.f, 2.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_FISH, CAbstractFactory<CFish>::Create(400.f, 400.f, 4.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_FISH, CAbstractFactory<CFish>::Create(300.f, 300.f, 1.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_FISH, CAbstractFactory<CFish>::Create(350.f, 550.f, 6.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_FISH, CAbstractFactory<CFish>::Create(450.f, 250.f, 5.f));
	CObjMgr::Get_Instance()->Add_Object(OBJ_FISH, CAbstractFactory<CFish>::Create(200.f, 450.f, 3.f));
}

void CStage3::Update()
{
	CObjMgr::Get_Instance()->Update();

	// 엔터 누르면 월드맵으로 이동
	if (CKeyMgr::Get_Instance()->Key_Down(VK_RETURN))
		CSceneMgr::Get_Instance()->Scene_Change(SC_WORLDMAP);

	// G키 누르면 의문의 강태공이 돈을 줌
	if (CKeyMgr::Get_Instance()->Key_Down('G'))
		CUIMgr::Get_Instance()->Add_Money(3000);
}

void CStage3::Late_Update()
{
	CObjMgr::Get_Instance()->Late_Update();
}

void CStage3::Render(HDC hDC)
{
	// 수면 렌더
	MoveToEx(hDC, 0, 200, nullptr);
	LineTo(hDC, 800, 200);

	CObjMgr::Get_Instance()->Render(hDC);
	CUIMgr::Get_Instance()->Render(hDC);

	// 조작법 설명
	TCHAR      szBuff1[64] = L"";
	swprintf_s(szBuff1, L"낚시대 좌, 우 이동 : A, D");
	TextOut(hDC, 550, 50, szBuff1, lstrlen(szBuff1));

	// 조작법 설명
	TCHAR      szBuff2[128] = L"";
	swprintf_s(szBuff2, L"낚시줄 감기, 풀기 : 뱡향키 좌, 우");
	TextOut(hDC, 550, 70, szBuff2, lstrlen(szBuff2));
}

void CStage3::Release()
{
	CObjMgr::Get_Instance()->Release();
}
