#pragma once
#include "Scene.h"

class CStage1 : public CScene
{
public:
	CStage1();
	virtual ~CStage1();
public:
	// CScene��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
};

