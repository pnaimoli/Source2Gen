#pragma once

template <unsigned int size, class TBadType>
class UnknownType
{
public:
	TBadType& Get()
	{
		return *(TBadType*)&m_data;
	}

	template <class CustomT>
	CustomT& Get()
	{
		return *(CustomT*)&m_data;
	}

	operator TBadType&()
	{
		return Get();
	}

protected:
	char m_data[size];
};

template <unsigned int size, template <typename...> class TOuterBadType, typename... Args>
class UnknownAtomicType
{
public:
	TOuterBadType<Args...>& Get()
	{
		return *(TOuterBadType<Args...>*)&m_data;
	}

	template <typename CustomT>
	CustomT& Get()
	{
		return *(CustomT*)&m_data;
	}

protected:
	char m_data[size];
};
