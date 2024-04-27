#pragma once

template<typename T>
bool CompareY(T _Dst, T _Src)
{
	return _Dst->Get_Info().fY < _Src->Get_Info().fY;
}



template<typename T>
void	Safe_Delete(T& Temp)
{
	if (Temp)
	{
		delete Temp;
		Temp = nullptr;
	}
}

template<typename T>
void	Safe_Delete_Array(T& Temp)
{
	if (Temp)
	{
		delete[] Temp;
		Temp = nullptr;
	}
}

// Functor

class CDeleteObj
{
public:
	template<typename T>
	void		operator()(T& Temp)
	{
		if (Temp)
		{
			delete Temp;
			Temp = nullptr;
		}
	}
};

class CDeleteMap
{
public:
	template<typename T>
	void		operator()(T& Temp)
	{
		if (Temp.second)
		{
			delete Temp.second;
			Temp.second = nullptr;
		}
	}
};

class CTagFinder
{
public:
	CTagFinder(const TCHAR* pTag)
		: m_pTag(pTag)
	{

	}

public:
	template<typename T>
	bool		operator()(T& Pair)
	{
		if (!lstrcmp(Pair.first, m_pTag))
			return true;

		return false;
	}

private:
	const TCHAR* m_pTag;
};