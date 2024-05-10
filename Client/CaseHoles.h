#pragma once
#include "Obj.h"
class CCaseHoles : public CObj
{
	vector<D3DXVECTOR3> m_LocalPointVector;
	vector<D3DXVECTOR3> m_WorldPointVector;
	REWARD m_eReward = REWARD::RWD_NONE;
	//450, 300, 0.0f
	//400, 900, 0.0f
	POINT m_MouseClickedPoint = {};
	CObj* m_pMakeMePtr = nullptr;
	RECT m_InsertArea = {};

	D3DXVECTOR3 LocalHoleLT = {};
	D3DXVECTOR3 LocalHoleRT = {};
	D3DXVECTOR3 LocalHoleLB = {};
	D3DXVECTOR3 LocalHoleRB = {};
	D3DXVECTOR3 WorldHoleLT = {};
	D3DXVECTOR3 WorldHoleRT = {};
	D3DXVECTOR3 WorldHoleLB = {};
	D3DXVECTOR3 WorldHoleRB = {};
	D3DXVECTOR3 m_vLocalBladeStart = {};
	D3DXVECTOR3 m_vLocalBladeEnd = {};
	D3DXVECTOR3 m_vWorldBladeStart = {};
	D3DXVECTOR3 m_vWorldBladeEnd = {};

	D3DXVECTOR3 m_vLocalBladeGuard = {};
	D3DXVECTOR3 m_vWorldBladeGuard = {};

	//D3DXVECTOR3 HoleLT = {};
	//D3DXVECTOR3 HoleRT = {};
	//D3DXVECTOR3 HoleLB = {};
	//D3DXVECTOR3 HoleRB = {};

	float m_fOffsetAngle = 0.0f;
	bool m_bIsOn = false;
	float m_CaseDiameter = 400.0f;
	float BladeLength = 100.0f;
	int m_iColorIndex = 2;
	bool m_bIsInserted = false;
	float m_fAngle = 0.0f;
	float m_fOffsetAngle_Server = 0.0f;
	float m_fAngleSpeed = 0.0f;
	float m_fCaseDiameter = 400.0f;
	int Points = 20;
	float m_fHoleYLength = 50.0f;
	int m_iHoleIndex = -1;
public:
	void SetHoleIndex(int Index) { m_iHoleIndex = Index; }
	int GetHoleIndex() { return m_iHoleIndex; }
	void SetIsInserted(bool bInserted, int PlayerIndex) 
	{
		m_bIsInserted = bInserted;
		m_iColorIndex = PlayerIndex;
	}
	vector<D3DXVECTOR3> PointVector;
	COLORREF m_Colors[ALLOWMAXCLIENTS] =
	{
		RGB(255, 0, 0),
		RGB(0, 255, 0),
		RGB(0, 0, 255),
		RGB(255, 255, 0),
		RGB(0, 255, 255),
		RGB(255, 0, 255),
		RGB(0, 0, 0)
	};
	//vector<>
public: // Data
public: // Func
	CCaseHoles();
	//virtual void		Initialize(void)PURE;
	//virtual int			Update(void)PURE;
	//virtual void		Late_Update(void)PURE;
	//virtual void		Render(HDC hDC)PURE;
	//virtual void		Release(void)PURE;

	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Render(HDC hDC) override;
	virtual void Release() override;
	virtual void Cal_WorldMatrix() override;
	void SetAngle(const float& parmAngle);
	void AftInit(CObj* parmObjPtr, const float& parmAngle, const float& parmYOffset);
	void SetReward(const REWARD& parmRewards);
	void JudgeReward();
};

