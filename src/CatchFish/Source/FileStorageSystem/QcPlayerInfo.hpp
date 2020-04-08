#pragma once

#include "Thread\QcLock.hpp"
#include "FileStorageSystem\QcFileStorageBase.hpp"

class QcFileStorageBase;
class QcFileWriter;
class QcFileReader;

class QcPlayerInfo
{
	struct QsStorageData
	{
		QsStorageData()
		{
			iCurScore		= 0;		//��ǰ�ܷ���
			iCatchScore		= 0;		//�������
			iFireScore		= 0;		//���ڷ���
			iExplodeScore   = 0;

			iTotalInScore	= 0;
			iInScore		= 0;			//�Ϸ�
			iInCoin			= 0;			//Ͷ��

			iTotalOutScore	= 0;		//�˲�Ʊ��
			iOutScore		= 0;
			iOutCoin		= 0;	
			iOutLottery		= 0;

			//���ڼ�¼
			iLiRunLast		= 0;		//�Ϸ���

			iGunLevel		= 1;

			iWinScoreModeWinScore = 0;
		}
		//��ǰ��¼
		int		iCurScore;		//��ǰ�ܷ���

		//64λ�Ʒ�
		INT64		iCatchScore;	//�������	
		INT64		iFireScore;		//���ڷ���
		INT64		iExplodeScore;	//��ը����
		//64λ�Ʒ�-end

		int		iTotalInScore;		//���Ϸ���
		int		iInScore;			//�Ϸ�
		int		iInCoin;			//Ͷ��

		int		iTotalOutScore;		//���˷�
		int		iOutScore;			//�·�
		int		iOutCoin;			//�˱�
		int		iOutLottery;		//��Ʊ

		int     iWinScoreModeWinScore;	//Ӯ����ģʽ����Ӯ�ķ���
		//���ڼ�¼
		int		iLiRunLast;	
		int		iGunLevel;
	};
public:
	QcPlayerInfo(QcFileStorageBase* pParentFileBase)
		: m_pFileBase(pParentFileBase)
	{
	}

	virtual ~QcPlayerInfo()
	{
	}
	QcCriticalSection& GetCS() {return m_pFileBase->GetCS();}
protected:
	QcFileStorageBase* m_pFileBase;
	QsStorageData m_StorageData;
public:
	virtual void Write(QcFileWriter& writer);
	virtual void Read(QcFileReader& reader);
	virtual void ClearCurrent();
	virtual void ClearAll();
public:
	int GetGunLevel()							{	QmReadLock(GetCS());	return m_StorageData.iGunLevel; }
	void SetGunLevel(int val)					{	QmLocker(GetCS());	m_StorageData.iGunLevel = val; m_pFileBase->SetDirty(true);}

	//���ڼ�¼
	int GetLiRunLast()							{	QmReadLock(GetCS());	return m_StorageData.iLiRunLast;}
	void SetLiRunLast(int iScore)				{	QmLocker(GetCS());	m_StorageData.iLiRunLast = iScore;	m_pFileBase->SetDirty(true);	}

	//���ڼ�¼
	//64λ�Ʒ�
	INT64 GetCatchScore()							{	QmReadLock(GetCS());	return m_StorageData.iCatchScore;	}
	void SetCatchScore(INT64 iScore)				{	QmLocker(GetCS());	m_StorageData.iCatchScore = iScore;	m_pFileBase->SetDirty(true);	}
	//64λ�Ʒ�-end
	int GetTotalInScore()						{	QmReadLock(GetCS());	return m_StorageData.iTotalInScore;}
	void SetTotalInScore(int iScore)			{	QmLocker(GetCS());	m_StorageData.iTotalInScore = iScore;	m_pFileBase->SetDirty(true);	}

	int GetInScore()							{	QmReadLock(GetCS());	return m_StorageData.iInScore;}
	void SetInScore(int iScore)					{	QmLocker(GetCS());	m_StorageData.iInScore = iScore;	m_pFileBase->SetDirty(true);	}

	int GetInCoin()								{	QmReadLock(GetCS());	return m_StorageData.iInCoin;}
	void SetInCoin(int iScore)					{	QmLocker(GetCS());	m_StorageData.iInCoin = iScore;	m_pFileBase->SetDirty(true);	}

	int GetCurScore()							{	QmReadLock(GetCS());	return m_StorageData.iCurScore;	}
	void SetCurScore(int iScore)				{	QmLocker(GetCS());	m_StorageData.iCurScore = iScore;	m_pFileBase->SetDirty(true);	}

	//64λ�Ʒ�
	INT64 GetFireScore()						{	QmReadLock(GetCS());	return m_StorageData.iFireScore;	}
	void SetFireScore(INT64 iScore)				{	QmLocker(GetCS());	m_StorageData.iFireScore = iScore; m_pFileBase->SetDirty(true);}

	INT64 GetExplodeScore()						{	QmReadLock(GetCS());	return m_StorageData.iExplodeScore;	}
	void SetExplodeScore(INT64 iScore)				{	QmLocker(GetCS());	m_StorageData.iExplodeScore = iScore; m_pFileBase->SetDirty(true);}

	//64λ�Ʒ�-end
	int GetWinModeWinScore()					{	QmReadLock(GetCS());	return m_StorageData.iWinScoreModeWinScore;	}
	void SetWinModeWinScore(int iScore)			
	{	
		QmLocker(GetCS());	
		m_StorageData.iWinScoreModeWinScore = iScore;	
		QmAssert(m_StorageData.iWinScoreModeWinScore>=0);
		m_pFileBase->SetDirty(true);
	}
	void AddWinModeWinScore(int iScore)			
	{	
		QmLocker(GetCS());	
		m_StorageData.iWinScoreModeWinScore += iScore; 
		m_pFileBase->SetDirty(true); 
		QmAssert(m_StorageData.iWinScoreModeWinScore>=0);
	}

	//�˱�
	void AddOutScore(int iExitNumber)		
	{	
		QmLocker(GetCS());	
		m_StorageData.iOutScore += iExitNumber;
		m_StorageData.iTotalOutScore += iExitNumber;	
		m_pFileBase->SetDirty(true);
	}

	void AddOutCoin(int iExitNumber)		
	{	
		QmLocker(GetCS());
		m_StorageData.iOutCoin += iExitNumber;
		m_StorageData.iTotalOutScore += iExitNumber;	
		m_pFileBase->SetDirty(true);
	}

	void AddOutLottery(int iExitNumber)		
	{	
		QmLocker(GetCS());
		m_StorageData.iOutLottery += iExitNumber;

		m_StorageData.iTotalOutScore += iExitNumber;
		m_pFileBase->SetDirty(true);
	}

	int GetOutScore()					{	QmReadLock(GetCS());	return m_StorageData.iOutScore;	}
	void SetOutScore(int iScore)		{	QmLocker(GetCS());	m_StorageData.iOutScore = iScore;	m_pFileBase->SetDirty(true);	}

	int GetOutCoin()					{	QmReadLock(GetCS());	return m_StorageData.iOutCoin;	}
	void SetOutCoin(int iScore)			{	QmLocker(GetCS());	m_StorageData.iOutCoin = iScore;	m_pFileBase->SetDirty(true);	}

	int GetOutLottery()					{	QmReadLock(GetCS());	return m_StorageData.iOutLottery;	}

	int GetTotalOutScore()				{	QmReadLock(GetCS());	return m_StorageData.iTotalOutScore;	}
	void SetTotalOutScore(int iScore)	{	QmLocker(GetCS());	m_StorageData.iTotalOutScore = iScore;	m_pFileBase->SetDirty(true);	}


	void Fire(int iScore)
	{
		QmLocker(GetCS());

		m_StorageData.iFireScore += iScore;
		m_StorageData.iCurScore -= iScore;
		m_pFileBase->SetDirty(true);
	}
	void Gamble(int iScore)
	{
		QmLocker(GetCS());
		m_StorageData.iFireScore += iScore;
		m_StorageData.iExplodeScore += iScore;
		m_pFileBase->SetDirty(true);
	}
	void Hit(int iScore)
	{
		QmLocker(GetCS());

		m_StorageData.iExplodeScore += iScore;
		m_pFileBase->SetDirty(true);
	}

	void AddCatchScore(int iScore)
	{
		QmLocker(GetCS());
		m_StorageData.iCatchScore += iScore;
		m_pFileBase->SetDirty(true);
	}

	void AddScore(int iScore)
	{
		QmLocker(GetCS());
		m_StorageData.iCurScore += iScore;
		QmAssert(m_StorageData.iCurScore >= 0);
		m_pFileBase->SetDirty(true);
	}

	void AddInScore(int iCoinNumber)
	{
		QmLocker(GetCS());
		m_StorageData.iInScore += iCoinNumber;
		m_StorageData.iTotalInScore += iCoinNumber;
		m_pFileBase->SetDirty(true);

	}

	void AddInCoin(int iCoinNumber)
	{
		QmLocker(GetCS());
		m_StorageData.iInCoin += iCoinNumber;
		m_StorageData.iTotalInScore += iCoinNumber;

		m_pFileBase->SetDirty(true);
	}
};