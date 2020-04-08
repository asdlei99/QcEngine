#pragma once

#include "FileSystem\QcGameFileBase.hpp"
#include "Thread\QcLock.hpp"
class QcSystemSetting : public QcFileStorageBase
{
public:
	QcSystemSetting(const QsDefaultSetting& defaultSetting);

	virtual void Write(QcFileWriter& writer);
	virtual void Read(QcFileReader& reader);

	int GetCoinQtyOneKey()  { QcLock gameLock(m_cs);	return iFireQtyOneCoin * 10; }

	int GetDifficultLevel()  { QcLock gameLock(m_cs);	return iDifficultLevel; }
	void SetDifficultLevel(int val) {QcLock gameLock(m_cs);	SetDirty(true); iDifficultLevel = val; }

	int GetMaxGunLevel()  { QcLock gameLock(m_cs);	return iMaxGunLevel; }
	void SetMaxGunLevel(int val) {QcLock gameLock(m_cs);	SetDirty(true); iMaxGunLevel = val; }

	int GetMinGunLevel()  { QcLock gameLock(m_cs);	return iMinGunLevel; }
	void SetMinGunLevel(int val) {QcLock gameLock(m_cs);	SetDirty(true); iMinGunLevel = val; }

	int GetJiaPaoFuDu()  { QcLock gameLock(m_cs);	return iJiaPaoFuDu; }
	void SetJiaPaoFuDu(int val) {QcLock gameLock(m_cs);	SetDirty(true); iJiaPaoFuDu = val; }

	QeGameExitMode GetGameMode()  {QcLock gameLock(m_cs);	 return enGameMode; }
	void SetGameMode(QeGameExitMode val) { QcLock gameLock(m_cs);	SetDirty(true);enGameMode = val; }

	void GetCaiPiaoBiLi(int &iCoin, int &iLottery)  { QcLock gameLock(m_cs);	iCoin = iCaiPiaoBiLiCoin;  iLottery = iCaiPiaoBiLiCaiPiao;}
	void SetCaiPiaoBiLi(int iCoin, int iLottery) { QcLock gameLock(m_cs);	SetDirty(true);iCaiPiaoBiLiCoin = iCoin;  iCaiPiaoBiLiCaiPiao = iLottery; }

	int GetFireQtyOneCoin()  { QcLock gameLock(m_cs);	return iFireQtyOneCoin;}
	void SetFireQtyOneCoin(int iVal)  {QcLock gameLock(m_cs);	SetDirty(true);iFireQtyOneCoin = iVal; }

	int GetLotteryQtyOneCoin()
	{
		QcLock gameLock(m_cs);	
		return iCaiPiaoBiLiCaiPiao / iCaiPiaoBiLiCoin;
	}

	int GetDaMaCiShu()
	{
		QcLock gameLock(m_cs);	
		return iDaMaCiShu;
	}	
	void AddDaMaCiShu(int iVal = 1)
	{
		QcLock gameLock(m_cs);	
		SetDirty(true); 
		iDaMaCiShu += iVal;
	}

	int GetShenYuShiJian()
	{
		QcLock gameLock(m_cs);	
		return iShenYuShiJian;
	}
	void AddShenYuShiJian(int iVal)
	{
		QcLock gameLock(m_cs);	
		SetDirty(true); 
		iShenYuShiJian += iVal;
	}
	void SetShenYuShiJian(int iVal)
	{
		QcLock gameLock(m_cs);	
		SetDirty(true); 
		iShenYuShiJian = iVal;
	}


	int GetXianHao()  
	{
		QcLock gameLock(m_cs);	 
		return iXianHao; 
	}
	void SetXianHao(int val) {QcLock gameLock(m_cs);	 SetDirty(true);  iXianHao = val; }
	int GetJiTaiHao()  
	{
		QcLock gameLock(m_cs);	
		return iJiTaiHao; 
	}
	void SetJiTaiHao(int val) {QcLock gameLock(m_cs);	 SetDirty(true);  iJiTaiHao = val; }

	BOOL GetNeedDaMa()  {QcLock gameLock(m_cs);	 return bNeedDaMa; }
	void SetNeedDaMa(BOOL val) {QcLock gameLock(m_cs);	 SetDirty(true); bNeedDaMa = val; }

	void ComputeShenYuShiJian(float fElapsedTime)
	{
		QcLock gameLock(m_cs);	
		static float fTimeCount = 0.0f;
		fTimeCount += fElapsedTime;
		if (fTimeCount > 60.0f)
		{
			SetDirty(true); 
			iShenYuShiJian--;
			fTimeCount -= 60.0f;
		}
	}

	QeGameFieldMode GetChangDiMode()  {QcLock gameLock(m_cs);	 return eChangDiMode; }
	void SetChangDiMode(QeGameFieldMode val) {QcLock gameLock(m_cs);	 SetDirty(true); eChangDiMode = val; }


	QeMachineType GetPlayerNumber()  {QcLock gameLock(m_cs);	 return ePlayerNumber; }
	void SetPlayerNumber(QeMachineType eVal) 
	{
		QcLock gameLock(m_cs); 
		SetDirty(true); 
		ePlayerNumber = eVal; 
	}

	int GetBanBenHao1()  {QcLock gameLock(m_cs);	 return iBanBenHao1; }
	void SetBanBenHao1(int val) 
	{
		QcLock gameLock(m_cs); 
		SetDirty(true); 
		iBanBenHao1 = val; 
	}


	int GetBanBenHao2()  {QcLock gameLock(m_cs);	 return iBanBenHao2; }
	void SetBanBenHao2(int val) 
	{
		QcLock gameLock(m_cs); 
		SetDirty(true); 
		iBanBenHao2 = val; 
	}

	int GetDaMaTian()  {QcLock gameLock(m_cs);	 return iDaMaTian; }
	void SetDaMaTian(int val)  {QcLock gameLock(m_cs);	SetDirty(true);  iDaMaTian = val; }

	int GetShowExDiff(){QcLock gameLock(m_cs); return iShowExDifficult;}
	void SetShowExDiff(int iVal){QcLock gameLock(m_cs); SetDirty(true); iShowExDifficult = iVal;}
private:
	int			iXianHao;
	int			iJiTaiHao;

	int			iFireQtyOneCoin;

	int			iCaiPiaoBiLiCoin;		//彩票比例
	int			iCaiPiaoBiLiCaiPiao;	//彩票比例

	int			iDifficultLevel;		//游戏难度
	int			iMaxGunLevel;			//最大炮数
	int			iMinGunLevel;			//最小炮数
	int			iJiaPaoFuDu;			//加炮幅度
	QeGameExitMode	enGameMode;			//退币模式

	int			iDaMaTian;				//打码天数

	int			iDaMaCiShu;
	int			iShenYuShiJian;			//分钟

	BOOL		bNeedDaMa;

	int			iBanBenHao1;			//主版本号
	int			iBanBenHao2;			//副版本号
	int			iShowExDifficult;

	QeMachineType	ePlayerNumber;
	QeGameFieldMode eChangDiMode;
};