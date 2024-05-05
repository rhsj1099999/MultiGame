#pragma once
#include "Scene.h"

class CStage4 : public CScene
{
public:
	CStage4();
	virtual ~CStage4();
	virtual void SetServerMode(bool bMode) override;
public:
	// CScene��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
};

