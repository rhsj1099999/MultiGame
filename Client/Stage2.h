#pragma once
#include "Scene.h"

class CStage2 : public CScene
{
public:
	CStage2();
	virtual ~CStage2();

public:
	// CScene��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
public:
	void Key_Input();
};

