#pragma once

class QcStructException
{
public:
	QcStructException(unsigned int u, EXCEPTION_POINTERS* pExp)
		: m_u(u), m_pExp(pExp){}

	static void SETransFunc(unsigned int u, EXCEPTION_POINTERS* pExp)
	{
		MakeDumpFile("SETransFunc", pExp);
		//throw QcStructException(u, pExp);
	}
protected:
	unsigned int m_u;
	EXCEPTION_POINTERS* m_pExp;
};