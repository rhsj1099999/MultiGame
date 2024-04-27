#include "stdafx.h"
#include "MidCase.h"

CMidCase::CMidCase()
{
}

void CMidCase::Initialize()
{
	this->m_eObjID = OBJ_MIDCASE;
}

int CMidCase::Update()
{
	return 0;
}

void CMidCase::Late_Update()
{
}

void CMidCase::Render(HDC hDC)
{
	//HBRUSH TempHoldBrush = (HBRUSH)SelectObject(parmHDC, GetStockObject(HOLLOW_BRUSH));
	HPEN TempHollowPen = CreatePen(1, 3, RGB(255, 255, 255));
	HPEN TempHoldPen = (HPEN)SelectObject(hDC, TempHollowPen);
	Rectangle
	(
		hDC,
		200,
		150,
		600,
		450
	);
	DeleteObject(SelectObject(hDC, TempHoldPen));

	MoveToEx
	(
		hDC,
		200,
		150,
		nullptr
	);
	LineTo
	(
		hDC,
		200,
		450
	);


	MoveToEx
	(
		hDC,
		600,
		150,
		nullptr
	);
	LineTo
	(
		hDC,
		600,
		450
	);
}

void CMidCase::Release()
{
}

void CMidCase::Cal_WorldMatrix()
{
}
