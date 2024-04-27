#pragma once
#include "Obj.h"

class CRoulette : public CObj
{
public:
	// CObj을(를) 통해 상속됨
	virtual void Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(HDC hDC) override;
	virtual void Release(void) override;
	virtual void Cal_WorldMatrix() override;
public:
	void	Set_RouletteNumber(int _iNum) { m_iRouletteNumber = _iNum; }
	int		Get_RouletteNumber() { return m_iRouletteNumber; }
	void	Set_Speed(float _fSpeed) { if(!m_bSpin) m_fSpeed = _fSpeed; }
	bool	Get_Spin() { return m_bSpin; }
	void	Set_Spin() { m_bSpin = true; }
	bool	Get_Game() { return m_bIsGame; }
	void	Set_Game(bool _bGame) { m_bIsGame = _bGame; }
	RECT* Get_Roulette_Rect() { return &m_tRect; }
private:
	D3DXVECTOR3			m_vPoint[3];
	D3DXVECTOR3			m_vOriginPoint[3];

	D3DXVECTOR3			m_vNumberPoint[6];
	D3DXVECTOR3			m_vNumberOriginPoint[6];

	RECT m_tRect;
	bool m_bIsGame;
	bool m_bSpin;
	float m_fAngle;
	int m_iRouletteNumber;
};

