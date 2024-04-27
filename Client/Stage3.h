#pragma once
#include "Scene.h"

class CStage3 : public CScene
{
public:
	CStage3();
	virtual ~CStage3() override;

public:
	// CScene을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
};

