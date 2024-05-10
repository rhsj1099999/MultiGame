#pragma once
#include "Scene.h"

class Stage_Matching : public CScene
{
public:
	Stage_Matching();
	virtual ~Stage_Matching();
public:
	// CScene��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;

private:
	char m_Buffer[BUFSIZE] = {};
	bool m_bSceneChangeeTrigger = false;
};