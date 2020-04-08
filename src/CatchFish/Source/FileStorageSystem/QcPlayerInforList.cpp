#include "pch.hpp"
#include "QcPlayerInforList.hpp"
#include "QcPlayerInfo.hpp"

QcPlayerInforList::QcPlayerInforList()
{
	QcFileWriter writer;
	for (u32 i=0; i<QmMaxPlayer; ++i)
	{
		m_pPlayerInfoList[i] = new QcPlayerInfo(this);
	}
}

QcPlayerInforList::~QcPlayerInforList()
{
	for (int i=0; i<QmCountOf(m_pPlayerInfoList); ++i)
		delete m_pPlayerInfoList[i];
}

void QcPlayerInforList::Write(QcFileWriter& writer)
{
	for (int i=0; i<QmCountOf(m_pPlayerInfoList); ++i)
		m_pPlayerInfoList[i]->Write(writer);
}
void QcPlayerInforList::Read(QcFileReader& reader)
{
	for (int i=0; i<QmCountOf(m_pPlayerInfoList); ++i)
		m_pPlayerInfoList[i]->Read(reader);
}