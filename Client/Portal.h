#pragma once
#include "Obj.h"

class CPortal : public CObj
{
	enum class PortalType
	{
		SINGLE,
		MULTI,
		EXIT,
		END,
	};
	//wstring m_PortalNames[static_cast<int>(PortalType::END)]
	//{
	//	L"Single",
	//	L"Multi",
	//	L"Exit"
	//};
public:
	CPortal();
	virtual ~CPortal();

	// CObj을(를) 통해 상속됨
	virtual void Initialize(void) override;
	virtual int Update(void) override;
	virtual void Late_Update(void) override;
	virtual void Render(HDC hDC) override;
	virtual void Release(void) override;
	virtual void OnTrigger() override;
	virtual void Cal_WorldMatrix() override;
public:
	void Set_PortalNumber(int _iNumber);/* { m_iPortalNumber = _iNumber; }*/
	int Get_PortalNumber() { return m_iPortalNumber; }
	void Change_Scene();
private:
	int m_iPortalNumber;
	PortalType m_ePortalType = PortalType::END;
	wstring m_PortalName = L"None";
};

