#include "stdafx.h"
#include "PirateHead.h"
#include "UpCase.h"

CPirateHead::CPirateHead()
{
}

void CPirateHead::Initialize()
{
	m_PirateHeadSize = { 100,100 };
	m_tInfo.vPos.x = 400;
	m_tInfo.vPos.y = 180;
}

int CPirateHead::Update()
{
	CUpCase* TempCastPtr = static_cast<CUpCase*>(m_pMakeMe);

	if (m_bForcedGoUp == true || TempCastPtr->GetIsStageEnd() == true)
	{
		m_fPirateHeadVelocity = -15.0f;
	}
	
	m_tInfo.vPos.y += m_fPirateHeadVelocity;
	return 0;
}

void CPirateHead::Late_Update()
{
}

void CPirateHead::Render(HDC hDC)
{
	CUpCase* TempCastPtr = dynamic_cast<CUpCase*>(m_pMakeMe);
	if (TempCastPtr->GetIsStageEnd() == true)
	{

	}
	Ellipse
	(
		hDC,
		m_tInfo.vPos.x - m_PirateHeadSize.x / 2,
		m_tInfo.vPos.y - m_PirateHeadSize.y / 2,
		m_tInfo.vPos.x + m_PirateHeadSize.x / 2,
		m_tInfo.vPos.y + m_PirateHeadSize.y / 2
	);
}

void CPirateHead::Release()
{
}

void CPirateHead::Cal_WorldMatrix()
{
}

void CPirateHead::SetAngle(const float& parmAngle)
{
}

void CPirateHead::AftInit(CObj* parmObjPtr)
{
	m_pMakeMe = parmObjPtr;
}

void CPirateHead::SetReward(const REWARD& parmRewards)
{
}

void CPirateHead::JudgeReward()
{
}
