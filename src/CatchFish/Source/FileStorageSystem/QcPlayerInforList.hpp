#pragma once

#include "FileStorageSystem\QcFileStorageBase.hpp"
class QcPlayerInfo;
class QcPlayerInforList : public QcFileStorageBase
{
public:
	QcPlayerInforList();
	~QcPlayerInforList();

	virtual void Write(QcFileWriter& writer);
	virtual void Read(QcFileReader& reader);
	QcPlayerInfo* GetPlayerInfo(u32 index) {return m_pPlayerInfoList[index];}
private:
	QcPlayerInfo* m_pPlayerInfoList[QmMaxPlayer];
};