#include "pch.hpp"
#include "QcPlayerInforList.hpp"
#include "QcPlayerInfo.hpp"

QcPlayerInforList::QcPlayerInforList(UInt32 nPlayer)
{
	for (UInt32 i=0; i<nPlayer; i++)
	{
		m_pPlayerInfoList.push_back(new QcPlayerInfo(this));
	}
}

QcPlayerInforList::~QcPlayerInforList()
{
	QcList<QcPlayerInfo*>::iterator iter(m_pPlayerInfoList.begin());
	for (; iter != m_pPlayerInfoList.end(); iter++)
	{
		delete *iter;
	}
	m_pPlayerInfoList.clear();
}

void QcPlayerInforList::Write(QcFileWriter& writer)
{
	QcList<QcPlayerInfo*>::iterator iter(m_pPlayerInfoList.begin());
	for (; iter != m_pPlayerInfoList.end(); iter++)
	{
		(*iter)->Write(writer);
	}
}
void QcPlayerInforList::Read(QcFileReader& reader)
{
	QcList<QcPlayerInfo*>::iterator iter(m_pPlayerInfoList.begin());
	for (; iter != m_pPlayerInfoList.end(); iter++)
	{
		(*iter)->Read(reader);
	}
}