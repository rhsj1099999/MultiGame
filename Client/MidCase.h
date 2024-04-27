#pragma once
#include "Obj.h"
class CMidCase : public CObj
{
	//float m_fAngle = 0.0f;
	//float m_fAngleSpeed = 0.0f;
	//float m_fCaseDiameter = 400.0f;
	//int Points = 20;
	//vector<D3DXVECTOR3> m_LocalPointVector;
	//vector<D3DXVECTOR3> m_WorldPointVector;
public: // Data
public: // Func
	CMidCase();
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
	virtual void Cal_WorldMatrix() override;
	//virtual void		Initialize(void)PURE;
	//virtual int			Update(void)PURE;
	//virtual void		Late_Update(void)PURE;
	//virtual void		Render(HDC hDC)PURE;
	//virtual void		Release(void)PURE;


};

