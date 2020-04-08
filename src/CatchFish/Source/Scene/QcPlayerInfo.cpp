#include "pch.hpp"
#include "QcPlayerInfo.hpp"
#include "FileSystem\QcGameFileBase.hpp"

void QcPlayerInfo::Write(QcFileWriter& writer)
{
	writer.Write(iCurScore);
	writer.Write(iCatchScore);
	writer.Write(iFireScore);
	writer.Write(iTotalInScore);
	writer.Write(iInScore);
	writer.Write(iInCoin);
	writer.Write(iTotalOutScore);
	writer.Write(iOutScore);
	writer.Write(iOutCoin);
	writer.Write(iOutLottery);
	writer.Write(iWinScoreModeWinScore);
	writer.Write(iLiRunLast);
	writer.Write(iGunLevel);
}
void QcPlayerInfo::Read(QcFileReader& reader)
{
	reader.Read(iCurScore);
	reader.Read(iCatchScore);
	reader.Read(iFireScore);
	reader.Read(iTotalInScore);
	reader.Read(iInScore);
	reader.Read(iInCoin);
	reader.Read(iTotalOutScore);
	reader.Read(iOutScore);
	reader.Read(iOutCoin);
	reader.Read(iOutLottery);
	reader.Read(iWinScoreModeWinScore);
	reader.Read(iLiRunLast);
	reader.Read(iGunLevel);
}

void QcPlayerInfo::ClearCurrent()
{
	//上期记录
	iLiRunLast	+= (int)(iFireScore - iCatchScore);

	iFireScore	= 0;
	iCatchScore	= 0;
	m_pFileBase->SetDirty(TRUE);
	Sleep(10);
}

void QcPlayerInfo::ClearAll()
{
	//上期记录
	iLiRunLast	= 0;	

	iInScore		= 0;		//上分数
	iCurScore		= 0;		//当前总分数
	iCatchScore		= 0;		//捕鱼分数
	iFireScore		= 0;		//开炮分数

	iTotalInScore	= 0;
	iInScore		= 0;			//上分
	iInCoin			= 0;			//投币

	iTotalOutScore	= 0;		//退彩票数
	iOutScore		= 0;
	iOutCoin		= 0;	
	iOutLottery		= 0;

	iWinScoreModeWinScore = 0;
	m_pFileBase->SetDirty(TRUE);
	Sleep(10);
}