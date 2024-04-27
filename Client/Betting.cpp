#include "stdafx.h"
#include "Betting.h"
#include "KeyMgr.h"
#include "UIMgr.h"

void CBetting::Initialize(void)
{
	m_tInfo.vPos = { 650,50,0 };
	m_tInfo.vScale = { 100, 100, 0 };
	for (int i = 0; i < 12; i++)
		m_iBetMoneyArray[i] = 0;
	m_bGame = false;
}

int CBetting::Update(void)
{
	Key_Input();
	return 0;
}

void CBetting::Late_Update(void)
{
}

void CBetting::Render(HDC hDC)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			Rectangle(hDC,
				int(m_tInfo.vPos.x - m_tInfo.vScale.x * 0.5f + i * 100),
				int(m_tInfo.vPos.y - m_tInfo.vScale.y * 0.5f + j * 100),
				int(m_tInfo.vPos.x + m_tInfo.vScale.x * 0.5f + i * 100),
				int(m_tInfo.vPos.y + m_tInfo.vScale.y * 0.5f + j * 100));
		}
	}
	int iCount = 0;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (i <= 4)
			{
				TCHAR szBuff[32];
				wsprintf(szBuff, L"%d", iCount);
				TextOut(hDC, m_tInfo.vPos.x - 5.f + j * 100, m_tInfo.vPos.y - 40.f + i * 100, szBuff, lstrlen(szBuff));
				if (m_iBetMoneyArray[iCount] > 999)
				{
					wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
					TextOut(hDC, m_tInfo.vPos.x - 33.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
				}
				else if (m_iBetMoneyArray[iCount] > 99)
				{
					wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
					TextOut(hDC, m_tInfo.vPos.x - 30.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
				}
				else
				{
					wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
					TextOut(hDC, m_tInfo.vPos.x - 20.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
				}
				iCount++;
			}
			else
			{
				if (j == 0)
				{
					TCHAR szBuff[32];
					wsprintf(szBuff, L"筈");
					TextOut(hDC, m_tInfo.vPos.x - 10.f + j * 100, m_tInfo.vPos.y - 40.f + i * 100, szBuff, lstrlen(szBuff));
					if (m_iBetMoneyArray[iCount] > 999)
					{
						wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
						TextOut(hDC, m_tInfo.vPos.x - 33.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
					}
					else if (m_iBetMoneyArray[iCount] > 99)
					{
						wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
						TextOut(hDC, m_tInfo.vPos.x - 30.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
					}
					else
					{
						wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
						TextOut(hDC, m_tInfo.vPos.x - 20.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
					}
					iCount++;
				}
				else
				{
					TCHAR szBuff[32];
					wsprintf(szBuff, L"側");
					TextOut(hDC, m_tInfo.vPos.x - 10.f + j * 100, m_tInfo.vPos.y - 40.f + i * 100, szBuff, lstrlen(szBuff));
					if (m_iBetMoneyArray[iCount] > 999)
					{
						wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
						TextOut(hDC, m_tInfo.vPos.x - 33.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
					}
					else if (m_iBetMoneyArray[iCount] > 99)
					{
						wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
						TextOut(hDC, m_tInfo.vPos.x - 30.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
					}
					else
					{
						wsprintf(szBuff, L"%d含君", m_iBetMoneyArray[iCount]);
						TextOut(hDC, m_tInfo.vPos.x - 20.f + j * 100, m_tInfo.vPos.y + i * 100, szBuff, lstrlen(szBuff));
					}
					iCount++;
				}
			}
		}
	}
}

void CBetting::Release(void)
{
}

void CBetting::Cal_WorldMatrix()
{
}

void CBetting::Key_Input()
{
	if (m_bGame)
	{
		return;
	}
	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
	{
		if (CUIMgr::Get_Instance()->Get_Money() < 100)
		{
			return;
		}
		int iCol = abs(pt.x - WINCX) / m_tInfo.vScale.x;
		int iRow = pt.y / m_tInfo.vScale.y;
		if (iCol == 0)
		{
			if (iRow >= 0 && iRow < 6)
			{
				CUIMgr::Get_Instance()->Set_Money(CUIMgr::Get_Instance()->Get_Money() - 100);
				m_iBetMoneyArray[iRow * 2 + 1] += 100;
			}
		}
		else if (iCol == 1)
		{
			if (iRow >= 0 && iRow < 6)
			{
				CUIMgr::Get_Instance()->Set_Money(CUIMgr::Get_Instance()->Get_Money() - 100);
				m_iBetMoneyArray[iRow * 2] += 100;
			}
		}
	}
	if (CKeyMgr::Get_Instance()->Key_Down(VK_RBUTTON))
	{
		int iCol = abs(pt.x - WINCX) / m_tInfo.vScale.x;
		int iRow = pt.y / m_tInfo.vScale.y;
		if (iCol == 0)
		{
			if (iRow >= 0 && iRow < 6)
			{
				if (m_iBetMoneyArray[iRow * 2 + 1] >= 100)
				{
					CUIMgr::Get_Instance()->Set_Money(CUIMgr::Get_Instance()->Get_Money() + 100);
					m_iBetMoneyArray[iRow * 2 + 1] -= 100;
				}
			}
		}
		else if (iCol == 1)
		{
			if (iRow >= 0 && iRow < 6)
			{
				if (m_iBetMoneyArray[iRow * 2] >= 100)
				{
					CUIMgr::Get_Instance()->Set_Money(CUIMgr::Get_Instance()->Get_Money() + 100);
					m_iBetMoneyArray[iRow * 2] -= 100;
				}
			}
		}
	}

}
