#pragma once
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
	//두더지잡기에서 두더지카운트
	int m_iMoleCnt;
};

