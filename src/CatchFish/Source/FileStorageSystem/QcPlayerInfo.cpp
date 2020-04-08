#include "pch.hpp"
#include "QcPlayerInfo.hpp"
#include "QcFileStorageBase.hpp"

void QcPlayerInfo::Write(QcFileWriter& writer)
{
	writer.Write(m_StorageData);
}
void QcPlayerInfo::Read(QcFileReader& reader)
{
	reader.Read(m_StorageData);
}

void QcPlayerInfo::ClearCurrent()
{
	QmLocker(GetCS());
	m_StorageData.iLiRunLast = (m_StorageData.iInCoin + m_StorageData.iInScore - m_StorageData.iOutScore - m_StorageData.iOutCoin - m_StorageData.iOutLottery); 

	m_StorageData.iFireScore	= 0;
	m_StorageData.iCatchScore	= 0;
	m_pFileBase->SetDirty(TRUE);
}

void QcPlayerInfo::ClearAll()
{
	QmLocker(GetCS());
	//上期记录
	m_StorageData.iLiRunLast	= 0;	

	m_StorageData.iInScore		= 0;		//上分数
	m_StorageData.iCurScore		= 0;		//当前总分数
	m_StorageData.iCatchScore		= 0;		//捕鱼分数
	m_StorageData.iFireScore		= 0;		//开炮分数
	m_StorageData.iExplodeScore	= 0;

	m_StorageData.iTotalInScore	= 0;
	m_StorageData.iInScore		= 0;			//上分
	m_StorageData.iInCoin			= 0;			//投币

	m_StorageData.iTotalOutScore	= 0;		//退彩票数
	m_StorageData.iOutScore		= 0;
	m_StorageData.iOutCoin		= 0;	
	m_StorageData.iOutLottery		= 0;

	m_StorageData.iWinScoreModeWinScore = 0;

	m_pFileBase->SetDirty(TRUE);
}