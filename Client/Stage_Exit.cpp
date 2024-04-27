#include "stdafx.h"
#include "Stage_Exit.h"
#include "ObjMgr.h"

void CStage_Exit::Initialize()
{
}

void CStage_Exit::Update()
{
	PostQuitMessage(0);
}

void CStage_Exit::Late_Update()
{
}

void CStage_Exit::Render(HDC hDC)
{
}

void CStage_Exit::Release()
{
	CObjMgr::Get_Instance()->Release();
	CObjMgr::Get_Instance()->Destroy_Instance();
}
