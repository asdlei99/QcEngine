#pragma once

class QcSyncObj
{
public:
	virtual ~QcSyncObj() = 0 {}
	virtual bool Lock() = 0;
	virtual bool UnLock() = 0;
};