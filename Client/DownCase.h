#pragma once
#include "Obj.h"
class CDownCase : public CObj
{
	float m_fAngle = 0.0f;
	float m_fAngleSpeed = 0.0f;
	float m_fCaseDiameter = 400.0f;
	int Points = 18;
	vector<D3DXVECTOR3> m_LocalPointVector;
	vector<D3DXVECTOR3> m_WorldPointVector;
public: // Data
public: // Func
	CDownCase();

	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
	virtual void Cal_WorldMatrix() override;
};

