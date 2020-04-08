#pragma once

#include "FileSystem\QcGameFileBase.hpp"
class QcPlayerInfo;
class QcPlayerInforList : public QcFileStorageBase
{
public:
	QcPlayerInforList(UInt32 nPlayer);
	~QcPlayerInforList();

	virtual void Write(QcFileWriter& writer);
	virtual void Read(QcFileReader& reader);
protected:
	std::vector<QcPlayerInfo*> m_pPlayerInfoList;
};