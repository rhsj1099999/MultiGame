#pragma once
#include "Obj.h"
class CWinEffect : public CObj
{
	float m_fAngle = 0.0f;
	float m_fAngleSpeed = 0.0f;
	float m_fCaseDiameter = 10.0f;
	float m_fRotatingSpeed = 1.0f;
	int Points = 18;


	vector<D3DXVECTOR3> m_LocalPointVector;
	vector<D3DXVECTOR3> m_WorldPointVector;
public: // Data
public: // Func
	CWinEffect();

	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
	virtual void Cal_WorldMatrix() override;
	COLORREF m_ColRef = {};
	//HBRUSH m_HBRectBrush;
};

