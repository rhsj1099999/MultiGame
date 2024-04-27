#pragma once
#include "Scene.h"

class CStage2 : public CScene
{
public:
	CStage2();
	virtual ~CStage2();

public:
	// CScene을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
public:
	void Key_Input();
};

