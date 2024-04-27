#pragma once
#include "Obj.h"
class CPirateHead : public CObj
{
	CObj* m_pMakeMe = nullptr;
public: // Data
public: // Func
	CPirateHead();
	//virtual void		Initialize(void)PURE;
	//virtual int			Update(void)PURE;
	//virtual void		Late_Update(void)PURE;
	//virtual void		Render(HDC hDC)PURE;
	//virtual void		Release(void)PURE;

	POINT m_PirateHeadSize = {};
	float m_fPirateHeadVelocity = 0.0f;
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
	virtual void Cal_WorldMatrix() override;
	void SetAngle(const float& parmAngle);
	void AftInit(CObj* parmObjPtr);
	void SetReward(const REWARD& parmRewards);
	void JudgeReward();
};

