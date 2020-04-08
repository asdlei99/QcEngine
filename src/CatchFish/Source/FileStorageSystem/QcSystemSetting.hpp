#pragma once

#include "FileStorageSystem\QcFileStorageBase.hpp"
#include "FileStorageSystem\QsSysStorageData.hpp"

enum QeLockFishFlag
{
	eLockFishNone = 0,
	eLockFishWithSuperGun,
	eLockFishAnyTime,
};

class QcSystemSetting : public QcFileStorageBase
{
	friend class QcSettingExchanger;
public:
	u32 GetMaxGunLevelRange1() { return iMaxGunLevelRange1; }
	u32 GetMaxGunLevelRange2() { return iMaxGunLevelRange2; }
	u32 GetMinGunLevelRange1() { return iMinGunLevelRange1; }
	u32 GetMinGunLevelRange2() { return iMinGunLevelRange2; }
	u32 GetExitCoinNumOnce() const {return m_iExitCoinOnce;}
	u32 GetMaxGoldOneColumn() const {return m_nMaxGoldOneColumn;}
	f32 GetFrozenTime() const { return m_fFrozenTime;}
	int GetCoinCountPerAddScorceKey() const {return m_iCoinCountPerAddScorceKey;}
	int GetCoinCountMultiAddScorceKey() const {return m_iCoinCountMultiAddScorceKey;}
	u32 GetMaxLivingBullet() const {return m_nMaxLivingBullet;}
	f32 GetSceneTime() const {return m_fSceneTime;}
	f32 GetSuperGunLivingTime() const {return m_fSuperGunTime;}
	f32 GetGunRotateSpeed() const {return m_fGunRotateSpeed;}
	f32 GetKeyGapTime() const {return m_fKeyGap;}
	f32 GetCoinSpeed() const {return m_fCoinSpeed;}
	f32 GetGunInitRotation() const {return HALF_PI;}
	const QcRect& GetTrackedBox() const {return m_rcTrackedBox;}
	void AdjustTrackedBox(const QcRect& rect) 
	{ 
		m_rcTrackedBox._BottomLeftPt = rect._BottomLeftPt + GetRelativeGunPos(); 
		m_rcTrackedBox._TopRightPt = rect._TopRightPt - GetRelativeGunPos();
	}
	const QcPoint& GetRelativeGunPos() const {return m_vRelativeGunPos; }
	const QcPoint& GetRelativeFishFlagPos() const {return m_vRelativeFishFlagPos; }
	const QcPoint& GetRelativeTrackHelperPos() const { return m_vRelativeTrackHelper; }
	const QcPoint& GetRelativeCoinEndPt() const {return m_vRelativeCoinEndPt; }
	const QcPoint& GetRalativeCoinExiterPt() const {return m_vRalativeCoinExiterPt;}
	const QcPoint& GetCaiJinPos() const {return m_vCaiJinPos;}
	int GetScoreChaoChange() const {return iScoreChaoChang;}
	int GetMaxSend() const {return iMaxSend;}
	QeLockFishFlag GetLockFishFlag() const {return m_eLockFishFlag;}

	void TryToSendDaMaPasswordToIO();
private:
	QeLockFishFlag m_eLockFishFlag;
	u32 m_iExitCoinOnce;
	int iMaxGunLevelRange1;		//最大炮数变化范围
	int iMaxGunLevelRange2;		//最大炮数变化范围
	int iMinGunLevelRange1;		//最小炮数变化范围
	int iMinGunLevelRange2;		//最小炮数变化范围
	int iScoreChaoChang;		//炒场分数
	int iMaxSend;
	int m_iCoinCountPerAddScorceKey;
	int m_iCoinCountMultiAddScorceKey;
	u32 m_nMaxGoldOneColumn;
	u32 m_nMaxLivingBullet;
	f32 m_fFrozenTime;
	f32 m_fSceneTime;
	f32 m_fGunRotateSpeed;
	f32 m_fKeyGap;
	f32 m_fCoinSpeed;
	f32 m_fGunInitRotation;
	f32 m_fSuperGunTime;
	QcRect m_rcTrackedBox;
	QcPoint m_vRelativeGunPos;
	QcPoint m_vRelativeFishFlagPos;
	QcPoint m_vRelativeTrackHelper;
	QcPoint m_vRelativeTrackFishFlagPos;
	QcPoint m_vRelativeCoinEndPt;
	QcPoint m_vRalativeCoinExiterPt;
	QcPoint m_vCaiJinPos;
public:
	QcSystemSetting();

	virtual void Write(QcFileWriter& writer);
	virtual void Read(QcFileReader& reader);

	u32 GetVolume() const {QmReadLock(m_cs); return m_storageData.iVolume;}
	void SetVolume(u32 iValue) { waveOutSetVolume(0, iValue); QmLocker(m_cs); m_storageData.iVolume = iValue; SetDirty(true);}

	u32 GetRunningTime() const {QmReadLock(m_cs); return m_storageData.iGameRunningTime;}
	void SetRunningTime(u32 iValue) {QmLocker(m_cs); m_storageData.iGameRunningTime = iValue; SetDirty(true);}

	int GetMaxGunLevel()  { QmReadLock(m_cs);	return m_storageData.iMaxGunLevel; }
	void SetMaxGunLevel(int val) {QmLocker(m_cs); m_storageData.iMaxGunLevel = val; SetDirty(true);}

	int GetMinGunLevel()  { QmReadLock(m_cs);	return m_storageData.iMinGunLevel; }
	void SetMinGunLevel(int val) {QmLocker(m_cs); m_storageData.iMinGunLevel = val; SetDirty(true);}

	int GetJiaPaoFuDu()  { QmReadLock(m_cs);	return m_storageData.iJiaPaoFuDu; }
	void SetJiaPaoFuDu(int val) {QmLocker(m_cs);	m_storageData.iJiaPaoFuDu = val; SetDirty(true); }

	QeGameExitMode GetGameMode()  {QmReadLock(m_cs);	 return m_storageData.enGameMode; }
	void SetGameMode(QeGameExitMode val) { QmLocker(m_cs);m_storageData.enGameMode = val; SetDirty(true);}

	int GetDifficultLevel()  { QmReadLock(m_cs);	return m_storageData.m_importantData.iDifficultLevel; }
	void SetDifficultLevel(int val) {QmLocker(m_cs);	m_storageData.m_importantData.iDifficultLevel = val; SetDirty(true); }

	void GetCaiPiaoBiLi(int &iCoin, int &iLottery)  { QmReadLock(m_cs); iCoin = m_storageData.m_importantData.iCaiPiaoBiLiCoin;  iLottery = m_storageData.m_importantData.iCaiPiaoBiLiCaiPiao;}
	void SetCaiPiaoBiLi(int iCoin, int iLottery) { QmLocker(m_cs); m_storageData.m_importantData.iCaiPiaoBiLiCoin = iCoin;  m_storageData.m_importantData.iCaiPiaoBiLiCaiPiao = iLottery; SetDirty(true);}

	int GetFireQtyOneCoin()  { QmReadLock(m_cs);	return abs(m_storageData.m_importantData.iFireQtyOneCoin);}
	void SetFireQtyOneCoin(int iVal)  {QmLocker(m_cs); m_storageData.m_importantData.iFireQtyOneCoin = iVal; SetDirty(true);}

	QeResolution GetResolution() { QmReadLock(m_cs); return m_storageData.iResolution; }
	void SetResolution(QeResolution ival) { QmLocker(m_cs); m_storageData.iResolution = ival; SetDirty(true); }

	int GetLotteryQtyOneCoin()
	{
		QmReadLock(m_cs);	
		return m_storageData.m_importantData.iCaiPiaoBiLiCaiPiao / m_storageData.m_importantData.iCaiPiaoBiLiCoin;
	}

	int GetDaMaCiShu()
	{
		QmReadLock(m_cs);	
		return m_storageData.iDaMaCiShu;
	}	
	void AddDaMaCiShu(int iVal = 1)
	{
		QmLocker(m_cs);	
		m_storageData.iDaMaCiShu += iVal;
		SetDirty(true); 
	}

	int GetXianHao()  
	{
		QmReadLock(m_cs);	 
		return m_storageData.iXianHao; 
	}
	void SetXianHao(int val) {QmLocker(m_cs);  m_storageData.iXianHao = val; SetDirty(true);}
	int GetJiTaiHao()  
	{
		QmReadLock(m_cs);	
		return m_storageData.iJiTaiHao; 
	}
	void SetJiTaiHao(int val) {QmLocker(m_cs); m_storageData.iJiTaiHao = val; SetDirty(true); }

	QeDaMaFlag GetDaMaFlag()  {QmReadLock(m_cs);	 return m_storageData.eDaMaFlag; }
	void SetDaMaFlag(QeDaMaFlag val) {QmLocker(m_cs);  m_storageData.eDaMaFlag = val; SetDirty(true);}

	int GetChangDiMode()  {QmReadLock(m_cs);	 return m_storageData.eChangDiMode; }
	void SetChangDiMode(int val) {QmLocker(m_cs); m_storageData.eChangDiMode = val; SetDirty(true);}

	bool IsTwoIOMode() const 
	{
		return (m_storageData.ePlayerNumber>=eMachine_UionScreen_6_Stand_2IO && m_storageData.ePlayerNumber<=eMachine_UionScreen_16_Lie_2IO)
			|| (m_storageData.ePlayerNumber>=eMachine_TwoScreen_6_Stand_2IO && m_storageData.ePlayerNumber<=eMachine_TwoScreen_20_lie_2IO);
	}
	bool IsUionScreenMode() const {return m_storageData.ePlayerNumber >= eMachine_UionScreen_6_Stand_1IO && m_storageData.ePlayerNumber <= eMachine_UionScreen_16_Lie_2IO;}
	bool IsTwoScreenMode() const {return m_storageData.ePlayerNumber >= eMachine_TwoScreen_4_Stand_1IO && m_storageData.ePlayerNumber <= eMachine_TwoScreen_20_lie_2IO;}

	QeMachineType GetPlayerNumber()  {QmReadLock(m_cs); return m_storageData.ePlayerNumber; }
	void SetPlayerNumber(QeMachineType eVal) 
	{
		QmLocker(m_cs); 
		m_storageData.ePlayerNumber = eVal;
		SetDirty(true); 
	}

	int GetBanBenHao1()  {QmReadLock(m_cs);	 return m_storageData.iBanBenHao1; }
	void SetBanBenHao1(int val) 
	{
		QmLocker(m_cs); 
		m_storageData.iBanBenHao1 = val;
		SetDirty(true); 
	}


	int GetBanBenHao2()  {QmReadLock(m_cs);	return m_storageData.iBanBenHao2; }
	void SetBanBenHao2(int val) 
	{
		QmLocker(m_cs); 
		m_storageData.iBanBenHao2 = val;
		SetDirty(true); 
	}

	u32 GetDaMaPassword()
	{
		QmReadLock(m_cs);
		return m_storageData.iDaMaPassword;
	}

	void SetDaMaPassword(u32 iPassword)
	{
		QmLocker(m_cs);
		m_storageData.iDaMaPassword = iPassword;
	}

	int GetbShowDaMaInfo() {QmReadLock(m_cs);	 return m_storageData.bShowDaMaInfo; }
	void SetBShowDaMaInfo(int val) { QmLocker(m_cs); m_storageData.bShowDaMaInfo = val; SetDirty(true); }

	int GetDaMaTian()  {QmReadLock(m_cs);	 return m_storageData.iDaMaTian; }
	__time64_t GetLeaveTime() {return m_storageData.iLeaveTime;}
	void SetLeaveTime(__time64_t t) {QmLocker(m_cs); m_storageData.iLeaveTime = t;}
	void RepairLeaveTime()
	{
		QmLocker(m_cs);
		__time64_t iSpanTime = 0;
		if (m_storageData.iCurrentTime != 0)
			iSpanTime = GetLocalTime64() - m_storageData.iCurrentTime;
		if (iSpanTime > 0/*&& iSpanTime < SpanTime(0, m_storageData.iDaMaTian) */)
		{
			SpanLeaveTime(iSpanTime);
		}
	}
	bool SpanLeaveTime(__time64_t t) 
	{
		QmLocker(m_cs); 
		if (m_storageData.iLeaveTime > t) 
		{
			m_storageData.iLeaveTime -= t;
		}
		else
		{
			m_storageData.iLeaveTime = 0;
		}
		m_storageData.iCurrentTime = GetLocalTime64();
		SetDirty(true);
		return m_storageData.iLeaveTime == 0;
	} 

	int GetShowExDiff(){QmReadLock(m_cs); return m_storageData.iShowExDifficult;}
	void SetShowExDiff(int iVal){QmLocker(m_cs);  m_storageData.iShowExDifficult = iVal; SetDirty(true);}

	int GetBaoJiScore(){QmReadLock(m_cs); return m_storageData.iBaoJiFenShu;}
	void SetBaoJiScore(int iVal){QmLocker(m_cs); m_storageData.iBaoJiFenShu = iVal; SetDirty(true);}

	void ApplyBackupData() {QmLocker(m_cs); m_storageData.m_importantData = m_BackupImportantData; SetDirty(true);}
	void SaveBackupData(const QsSysBackupData& data){ QmLocker(m_cs); m_BackupImportantData = data; SetDirty(true);}

	const QsSysBackupData& GetBackupData() const {return m_BackupImportantData;}
	void SetBackupDataDifficultLevel(int val) {QmLocker(m_cs);	m_BackupImportantData.iDifficultLevel = val; SetDirty(true); }

	bool IsNeedClearAccount(const QsSysBackupData& nowData) {QmReadLock(m_cs); return nowData != m_storageData.m_importantData;}

	void RecoverDefault(QsSysStorageData& defaultData,bool bRecoverParaset);

    u32  GetGambleOver() {QmReadLock(m_cs); return m_storageData.iGambleOver;}
    void SetGambleOver(u32 val) {QmLocker(m_cs); m_storageData.iGambleOver=val; SetDirty(true);}

    bool HasLotteryBullet(){QmReadLock(m_cs); return m_storageData.bLotteryBullet;}
    void SetLotteryBullet(bool enable) {QmLocker(m_cs); m_storageData.bLotteryBullet=enable; SetDirty(true);}
private:
	QsSysStorageData m_storageData;
	QsSysBackupData m_BackupImportantData;
};