#pragma once
#include "Scene.h"
#include "Include.h"
#include "Player.h"

class CWorldMap : public CScene
{
public:
	CWorldMap();
	~CWorldMap();
public:
	// CScene��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
};

