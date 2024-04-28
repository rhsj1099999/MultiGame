#pragma once
#include "Scene.h"
class CStage_Exit : public CScene
{
public: // Func
public: // Data
	CStage_Exit()
	{

	}
	virtual ~CStage_Exit()
	{
		Release();
	}
	virtual void	Initialize();
	virtual void	Update();
	virtual void	Late_Update();
	virtual void	Render(HDC hDC);
	virtual void	Release();
};

