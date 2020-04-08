#pragma once

#include <windows.h>
#include "Thread\QcSyncObj.hpp"

class QcCriticalSection : public QcSyncObj
{
public:
	QcCriticalSection(u32 iSpinCount)
	{
		::InitializeCriticalSectionAndSpinCount(&m_sect, iSpinCount);
	}
	QcCriticalSection()
	{
		::InitializeCriticalSection(&m_sect);
	}
	virtual ~QcCriticalSection()
	{
		DeleteCriticalSection(&m_sect);
	}
	virtual bool Lock()
	{
		EnterCriticalSection(&m_sect);
		return true;
	}
	virtual bool UnLock()
	{
		LeaveCriticalSection(&m_sect);
		return true;
	}
private:
	CRITICAL_SECTION m_sect;
};
