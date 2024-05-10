#pragma once
#include "Include.h"
#include "Obj.h"

class CObjMgr
{
public:
	CObjMgr();
	~CObjMgr();
public:
	void	Add_Object(OBJID eID, CObj* pObj);
	int		Update();
	void	Late_Update();
	void	Render(HDC hDC);
	void	Release();

public:
	static CObjMgr* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CObjMgr;
		}
		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	list<CObj*>		m_ObjList[OBJ_END];
	static CObjMgr* m_pInstance;
	int* m_pBettingArray;
};

