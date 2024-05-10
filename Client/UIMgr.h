#pragma once
#include "Include.h"

class CUIMgr
{
public:
	void Initialize();
	void Render(HDC hDC);
	void Update();
	void Set_Money(int _iMoney) { m_iMoney = _iMoney; }
	void Add_Money(int _iMoney) { m_iMoney += _iMoney; }
	int Get_Money() { return m_iMoney; }
	void Set_RoulletWaiting() { m_bRoulletWaiting = true; }
	void Set_RoulletUI(int _iNumber, int _iMoney) {
		m_bRoulletWaiting = false;
		m_iRoulletNumber = _iNumber;
		m_iRoulletMoney = _iMoney;
	}
	//액세스함수(두더지잡기)
	void Set_MoleCnt(int _iMoleCnt) { m_iMoleCnt = _iMoleCnt; }
	int Get_MoleCnt() { return m_iMoleCnt; }
	void LoseAllMoney()
	{
		m_iMoney = 0;
	}

	struct UIRenderDesc
	{
		float m_fX = static_cast<float>(WINCX - 10);
		float m_fY = static_cast<float>(WINCY / 2);
		TCHAR szBuff[32] = {};
	};

	void SetUIText(HDC hDC, UIRenderDesc& Desc, SIZE& TextSize, const TCHAR* Buffer)
	{
		wsprintf(Desc.szBuff, Buffer);
		GetTextExtentPoint32(hDC, Desc.szBuff, lstrlen(Desc.szBuff), &TextSize);
		TextOut(hDC, Desc.m_fX - TextSize.cx, Desc.m_fY + (m_UIRenderStep * MESSAGEYDIFF), Desc.szBuff, lstrlen(Desc.szBuff));
		m_UIRenderStep += 1.0f;
	};


	void RenderTutorial(HDC hDC);
private:

	void RenderSceneChange(HDC hDC, UIRenderDesc& Desc);
	void RenderHowtoPlay(HDC hDC, UIRenderDesc& Desc);
	UIRenderDesc m_tUIRenderDesc = {};
	float m_UIRenderStep = 0.0f;




public:
	static CUIMgr* Get_Instance(void)
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CUIMgr;

		return m_pInstance;
	}

	void	Destroy_Instance(void)
	{
		if (nullptr != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static CUIMgr* m_pInstance;
	int m_iMoney;
	bool m_bRoulletWaiting;
	int m_iRoulletNumber;
	int m_iRoulletMoney;
	int m_iMoleCnt;
};

