#pragma once

class CScene
{
public:
	CScene();
	virtual ~CScene();
	virtual void SetServerMode(bool bMode);

public:
	virtual void	Initialize()	PURE;
	virtual void	Update()		PURE;
	virtual void	Late_Update()	PURE;
	virtual void	Render(HDC hDC)	PURE;
	virtual void	Release()		PURE;
protected:
	bool m_bIsServerMode = false;
};

