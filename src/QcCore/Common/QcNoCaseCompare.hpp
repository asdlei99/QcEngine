#pragma once

inline int QfStrNoCaseCmp(const char* dst, const char* src, int iANSIOnly = 1)
{
	if (iANSIOnly)
	{
		int f, l;
		do
		{
			if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
				f -= 'A' - 'a';
			if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
				l -= 'A' - 'a';
		}
		while ( f && (f == l) );

		return(f - l);
	}
	return _stricmp(dst, src);
}

class QcNoCaseCompare/* : public std::binary_function<QcString, QcString, bool>*/
{
public:
	QcNoCaseCompare(const char* psValue = NULL) : m_psValue(psValue) {}
	bool operator()(const QcString& left, const QcString& right) const
	{
		return QfStrNoCaseCmp(left.c_str(), right.c_str()) < 0;
	}
	bool operator==(const char* psValue) const
	{
		return QfStrNoCaseCmp(m_psValue, psValue) == 0;
	}
	bool operator!=(const char* psValue) const
	{
		return QfStrNoCaseCmp(m_psValue, psValue) != 0;
	}
protected:
	const char* m_psValue;
};