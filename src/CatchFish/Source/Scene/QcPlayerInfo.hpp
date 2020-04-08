#pragma once

#include "Thread\QcLock.hpp"
#include "FileSystem\QcGameFileBase.hpp"

class QcFileStorageBase;
class QcFileWriter;
class QcFileReader;
class QcPlayerInfo
{
public:
	QcPlayerInfo(QcFileStorageBase* pParentFileBase)
		: m_pFileBase(pParentFileBase)
	{
		//当前记录
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

		//上期记录
		iLiRunLast		= 0;		//上分数

		iGunLevel		= 1;

		iWinScoreModeWinScore = 0;
	}

	virtual ~QcPlayerInfo()
	{
	}
protected:
	QcFileStorageBase* m_pFileBase;
	//当前记录
	int		iCurScore;		//当前总分数

	//64位计分
	INT64		iCatchScore;	//捕鱼分数	
	INT64		iFireScore;		//开炮分数
	//64位计分-end

	int		iTotalInScore;		//总上分数
	int		iInScore;			//上分
	int		iInCoin;			//投币

	int		iTotalOutScore;		//总退分
	int		iOutScore;			//下分
	int		iOutCoin;			//退币
	int		iOutLottery;		//彩票

	int     iWinScoreModeWinScore;	//赢分退模式，所赢的分数
	//上期记录
	int		iLiRunLast;	

	int		iGunLevel;
public:
	virtual void Write(QcFileWriter& writer);
	virtual void Read(QcFileReader& reader);
	virtual void ClearCurrent();
	virtual void ClearAll();
public:
	int GetGunLevel()							{	QcLock gameLock(m_pFileBase->GetCS());	return iGunLevel; }
	void SetGunLevel(int val)					{	QcLock gameLock(m_pFileBase->GetCS());	iGunLevel = val; m_pFileBase->SetDirty(true);}

	//上期记录
	int GetLiRunLast()							{	QcLock gameLock(m_pFileBase->GetCS());	return iLiRunLast;}
	void SetLiRunLast(int iScore)				{	QcLock gameLock(m_pFileBase->GetCS());	iLiRunLast = iScore;	m_pFileBase->SetDirty(true);	}

	//当期记录
	//64位计分
	INT64 GetCatchScore()							{	QcLock gameLock(m_pFileBase->GetCS());	return iCatchScore;	}
	void SetCatchScore(INT64 iScore)				{	QcLock gameLock(m_pFileBase->GetCS());	iCatchScore = iScore;	m_pFileBase->SetDirty(true);	}
	//64位计分-end
	int GetTotalInScore()						{	QcLock gameLock(m_pFileBase->GetCS());	return iTotalInScore;}
	void SetTotalInScore(int iScore)			{	QcLock gameLock(m_pFileBase->GetCS());	iTotalInScore = iScore;	m_pFileBase->SetDirty(true);	}

	int GetInScore()							{	QcLock gameLock(m_pFileBase->GetCS());	return iInScore;}
	void SetInScore(int iScore)					{	QcLock gameLock(m_pFileBase->GetCS());	iInScore = iScore;	m_pFileBase->SetDirty(true);	}

	int GetInCoin()								{	QcLock gameLock(m_pFileBase->GetCS());	return iInCoin;}
	void SetInCoin(int iScore)					{	QcLock gameLock(m_pFileBase->GetCS());	iInCoin = iScore;	m_pFileBase->SetDirty(true);	}

	int GetCurScore()							{	QcLock gameLock(m_pFileBase->GetCS());	return iCurScore;	}
	void SetCurScore(int iScore)				{	QcLock gameLock(m_pFileBase->GetCS());	iCurScore = iScore;	m_pFileBase->SetDirty(true);	}

	//64位计分
	INT64 GetFireScore()						{	QcLock gameLock(m_pFileBase->GetCS());	return iFireScore;	}
	void SetFireScore(INT64 iScore)				{	QcLock gameLock(m_pFileBase->GetCS());	iFireScore = iScore; m_pFileBase->SetDirty(true);}
	//64位计分-end
	int GetWinModeWinScore()					{	QcLock gameLock(m_pFileBase->GetCS());	return iWinScoreModeWinScore;	}
	void SetWinModeWinScore(int iScore)			
	{	
		QcLock gameLock(m_pFileBase->GetCS());	iWinScoreModeWinScore = iScore;	
		assert(iWinScoreModeWinScore>=0);
		if (iWinScoreModeWinScore < 0) iWinScoreModeWinScore = 0;
		m_pFileBase->SetDirty(true);
	}
	void AddWinModeWinScore(int iScore)			
	{	
		QcLock gameLock(m_pFileBase->GetCS());	
		iWinScoreModeWinScore += iScore; 
		m_pFileBase->SetDirty(true); 
		assert(iWinScoreModeWinScore>=0);
		if (iWinScoreModeWinScore < 0) iWinScoreModeWinScore = 0;
	}

	//退币
	void AddOutScore(int iExitNumber)		
	{	
		QcLock gameLock(m_pFileBase->GetCS());	
		iOutScore += iExitNumber;
		iTotalOutScore += iExitNumber;	
		m_pFileBase->SetDirty(true);
	}

	void AddOutCoin(int iExitNumber)		
	{	
		QcLock gameLock(m_pFileBase->GetCS());
		iOutCoin += iExitNumber;

		iTotalOutScore += iExitNumber;	
		m_pFileBase->SetDirty(true);
	}

	void AddOutLottery(int iExitNumber)		
	{	
		QcLock gameLock(m_pFileBase->GetCS());
		iOutLottery += iExitNumber;

		iTotalOutScore += iExitNumber;
		m_pFileBase->SetDirty(true);
	}

	int GetOutScore()					{	QcLock gameLock(m_pFileBase->GetCS());	return iOutScore;	}
	void SetOutScore(int iScore)		{	QcLock gameLock(m_pFileBase->GetCS());	iOutScore = iScore;	m_pFileBase->SetDirty(true);	}

	int GetOutCoin()					{	QcLock gameLock(m_pFileBase->GetCS());	return iOutCoin;	}
	void SetOutCoin(int iScore)			{	QcLock gameLock(m_pFileBase->GetCS());	iOutCoin = iScore;	m_pFileBase->SetDirty(true);	}

	int GetOutLottery()					{	QcLock gameLock(m_pFileBase->GetCS());	return iOutLottery;	}
	void SetOutLottery(int iScore)		{	QcLock gameLock(m_pFileBase->GetCS());	iOutLottery = iScore;	m_pFileBase->SetDirty(true);	}

	int GetTotalOutScore()				{	QcLock gameLock(m_pFileBase->GetCS());	return iTotalOutScore;	}
	void SetTotalOutScore(int iScore)	{	QcLock gameLock(m_pFileBase->GetCS());	iTotalOutScore = iScore;	m_pFileBase->SetDirty(true);	}


	void Fire(int iScore)
	{
		QcLock gameLock(m_pFileBase->GetCS());

		iFireScore += iScore;
		iCurScore -= iScore;
		m_pFileBase->SetDirty(true);
	}

	void AddCatchScore(int iScore)
	{
		QcLock gameLock(m_pFileBase->GetCS());
		iCatchScore += iScore;
		m_pFileBase->SetDirty(true);
	}

	void AddScore(int iScore)
	{
		QcLock gameLock(m_pFileBase->GetCS());
		iCurScore += iScore;
		assert(iCurScore >= 0);
		if (iCurScore < 0)	iCurScore = 0;
		m_pFileBase->SetDirty(true);
	}

	void AddInScore(int iCoinNumber)
	{
		QcLock gameLock(m_pFileBase->GetCS());
		iInScore += iCoinNumber;
		iTotalInScore += iCoinNumber;
		m_pFileBase->SetDirty(true);

	}

	void AddInCoin(int iCoinNumber)
	{
		QcLock gameLock(m_pFileBase->GetCS());
		iInCoin += iCoinNumber;
		iTotalInScore += iCoinNumber;

		m_pFileBase->SetDirty(true);
	}
};