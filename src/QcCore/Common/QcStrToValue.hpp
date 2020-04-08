#pragma once


#define QmStrToValueTemplateFun2(type, psStr) template<> inline type StrToValue<type>(const char* psStr)
#define QmStrToValueTemplateFun(type) QmStrToValueTemplateFun2(type, psStr)

template<class T>
T StrToValue(const char* psStr)
{
	return T(psStr);
}

class QcStrToValue
{
	QmDisableCopyAssignment(QcStrToValue);
public:
	QcStrToValue(const char* psStr) : m_psValue(psStr) {}

	operator bool() const
	{
		return m_psValue == NULL || m_psValue[0] == '\0';
	}
	operator const char*() const
	{
		return m_psValue;
	}
	template<class T>
	operator T() const
	{
		return StrToValue<T>(m_psValue);
	}
protected:
	const char* m_psValue;
};

//
QmStrToValueTemplateFun(f32){ return (f32)atof(psStr);}
QmStrToValueTemplateFun(f64){ return atof(psStr);}
#define QmStrToIntType(type) QmStrToValueTemplateFun(type) { return type(atoi(psStr));}
QmStrToIntType(s8);
QmStrToIntType(s16);
QmStrToIntType(s32);
QmStrToIntType(u8);
QmStrToIntType(u16);
QmStrToIntType(u32);
QmStrToIntType(long);

template<class T>
inline u32 StrToValueList(const char* psStr, T* pValueList, u32 nCount, char splitChar=',')
{
	u32 iCount = 0;
	char ch = *psStr;
	const char* psValue = psStr;
	const char* pBegin = psStr;
	while (ch != 0 && iCount < nCount)
	{
		if (ch == splitChar)
		{
			if (psValue != pBegin)
			{
				*pValueList = StrToValue<T>(pBegin);
				++pValueList;
				++iCount;
			}
			++psValue;
			pBegin = psValue;
		}
		else
		{
			++psValue;
		}
		ch = *psValue;
	}
	if (psValue != pBegin && iCount < nCount)
	{
		*pValueList = StrToValue<T>(pBegin);
		++iCount;
	}
	return iCount;
}

template <class T, size_t _Size>
inline u32 StrToValueList(const char* psStr, T (&_Dst)[_Size], char splitChar=',')
{
	return StrToValueList(psStr, _Dst, _Size, splitChar);
}


