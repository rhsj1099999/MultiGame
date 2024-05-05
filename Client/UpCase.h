#pragma once
#include "Obj.h"
class CUpCase : public CObj
{
	float m_fAngle_GetServer = 0.0;
	bool m_bIsStageEnd = false;
	float m_fAngle = 0.0f;
	float m_fAngleSpeed = 0.0f;
	float m_fCaseDiameter = 400.0f;
	int Points = 18;
	vector<D3DXVECTOR3> m_LocalPointVector;
	vector<D3DXVECTOR3> m_WorldPointVector;
	vector<CObj*> m_HolesPtrVector;
	vector<RECT> m_HoleRectVector;
	POINT m_MouseClickedPoint = {};



	vector<D3DXVECTOR3> m_LocalShadeVector;
	vector<D3DXVECTOR3> m_WorldShadeVector;

public: // Data
public: // Func
	CUpCase();
	//virtual void		Initialize(void)PURE;
	//virtual int			Update(void)PURE;
	//virtual void		Late_Update(void)PURE;
	//virtual void		Render(HDC hDC)PURE;
	//virtual void		Release(void)PURE;

	void SetIsStageEnd(const bool& parmBool)
	{
		this->m_bIsStageEnd = parmBool;
	}
	const bool& GetIsStageEnd()
	{
		return this->m_bIsStageEnd;
	}
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
	virtual void Cal_WorldMatrix() override;
};

