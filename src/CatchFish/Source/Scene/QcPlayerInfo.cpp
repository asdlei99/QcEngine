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
	//���ڼ�¼
	iLiRunLast	+= (int)(iFireScore - iCatchScore);

	iFireScore	= 0;
	iCatchScore	= 0;
	m_pFileBase->SetDirty(TRUE);
	Sleep(10);
}

void QcPlayerInfo::ClearAll()
{
	//���ڼ�¼
	iLiRunLast	= 0;	

	iInScore		= 0;		//�Ϸ���
	iCurScore		= 0;		//��ǰ�ܷ���
	iCatchScore		= 0;		//�������
	iFireScore		= 0;		//���ڷ���

	iTotalInScore	= 0;
	iInScore		= 0;			//�Ϸ�
	iInCoin			= 0;			//Ͷ��

	iTotalOutScore	= 0;		//�˲�Ʊ��
	iOutScore		= 0;
	iOutCoin		= 0;	
	iOutLottery		= 0;

	iWinScoreModeWinScore = 0;
	m_pFileBase->SetDirty(TRUE);
	Sleep(10);
}