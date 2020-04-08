#pragma once

#include "Thread\QcSyncObj.hpp"

class QcLock
{
	QmDisableCopyAssignment(QcLock);
public:
	explicit QcLock(QcSyncObj& pSynObj) : m_synObj(pSynObj)
	{
		m_synObj.Lock();
	}
	~QcLock()
	{
		m_synObj.UnLock();
	}
private:
	QcSyncObj& m_synObj; 
};