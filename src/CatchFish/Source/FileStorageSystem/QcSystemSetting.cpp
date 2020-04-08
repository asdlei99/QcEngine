#include "pch.hpp"
#include "QcSystemSetting.hpp"
#include "Common\QcMediaFileFinder.hpp"

class ProfileFileReader
{
public:
	ProfileFileReader(const char* psFile) : m_sFile(psFile)
	{}

	int ReadProfileValue(const char* psKey, int iDefault, int iMin, int iMax)
	{
		if (iDefault < iMin || iDefault > iMax)
			iDefault = iMin;
		int iRet = GetPrivateProfileIntA("YLWQ_SETTING", psKey, iDefault, m_sFile.c_str());
		if (iRet > iMax)
			iRet = iMax;
		else if (iRet < iMin)
			iRet = iMin;
		return iRet;
	}
protected:
	QcString m_sFile;
};

QcSystemSetting::QcSystemSetting()
{
	m_iExitCoinOnce = 20;
	m_nMaxGoldOneColumn = 30;
	m_fFrozenTime = 10.f;
	m_iCoinCountPerAddScorceKey = 10;
	m_iCoinCountMultiAddScorceKey = 100;
	m_nMaxLivingBullet = 15;
	m_fSceneTime = 60.f * 5.f;
#ifdef QmTestVersion
	//if (IsDebuggerPresent())
	//	m_fSceneTime = gfMax;
#endif
	m_fSuperGunTime = 20.f;
	m_fGunRotateSpeed = 1.76f;
	m_fKeyGap = 0.15f;
	m_fCoinSpeed = 200.f;
	m_vRelativeGunPos = QcPoint(0.f, 75.f);
	m_vRelativeFishFlagPos = QcPoint(260.f, 120.f);
	m_vRelativeTrackHelper = QcPoint(160.f,120.f);
	m_vRelativeCoinEndPt = QcPoint(0.f, 0.f);
	m_vRalativeCoinExiterPt = QcPoint(170.f, 100.f);
	m_vCaiJinPos = QcPoint(0, 250.f);

	RecoverDefault(m_storageData,true);
	m_BackupImportantData = m_storageData.m_importantData;
}
	

void QcSystemSetting::RecoverDefault(QsSysStorageData& defaultData,bool bRecoverAll)
{
	QcString sProfileName;
	char buf[MAX_PATH] = {0};
	GetCurrentDirectoryA(MAX_PATH, buf);
	sProfileName = buf;
	sProfileName += "\\ylwq_update.ini";
	if (!QcFileSystem::IsExist(sProfileName))
	{
		sProfileName = "ylwq_update.ini";
		if (!QcMediaFileFinderInstance->FindFile(sProfileName))
			QmLog("can't find ini file");
	}
	
	ProfileFileReader reader(sProfileName.c_str());

	int iMaxInt = 0x7fffffff;
	if (bRecoverAll)
	{
		defaultData.m_importantData.iFireQtyOneCoin		= reader.ReadProfileValue("SCOREONECOIN", 100, 1, iMaxInt);
		defaultData.iDaMaTian			= reader.ReadProfileValue("DAMATIAN", 10, 1, 13);
		defaultData.iDaMaCiShu			= 0;
		defaultData.iCurrentTime		= 0;
		defaultData.iLeaveTime			= SpanTime(0, defaultData.iDaMaTian);
		defaultData.iXianHao			= reader.ReadProfileValue("XIANHAO", 100, 1, 999);
		defaultData.iJiTaiHao			= reader.ReadProfileValue("JITAIHAO", 10000000, 1, 99999999);
		defaultData.eDaMaFlag			= eDaMaNone;
		defaultData.eChangDiMode		= reader.ReadProfileValue("CHANGDI", 1, 0, 2);
		defaultData.iBanBenHao1			= reader.ReadProfileValue("BANBENHAO1", 101, 100, 999);
		defaultData.iBanBenHao2			= reader.ReadProfileValue("BANBENHAO2", 104, 100, 999);
		iScoreChaoChang		= reader.ReadProfileValue("SCORECHAOCHANG", 0, 0, 60000);
		iMaxSend = reader.ReadProfileValue("ABC", 10000, 0, 60000);
		defaultData.iShowExDifficult	= 0;
		defaultData.iGameRunningTime = 0;
		defaultData.bShowDaMaInfo = 1;

		DWORD dwWord = 0;
		waveOutGetVolume(0, &dwWord);
		defaultData.iVolume = dwWord;

        defaultData.bLotteryBullet = true;
	}

	defaultData.m_importantData.iDifficultLevel		= reader.ReadProfileValue("DIFFICULTLEVEL", 2, 0, 4);			//游戏难度

	defaultData.m_importantData.iCaiPiaoBiLiCoin	= reader.ReadProfileValue("LOTTERYCOIN", 1, 1, 100);		//彩票比例
	defaultData.m_importantData.iCaiPiaoBiLiCaiPiao	= reader.ReadProfileValue("LOTTERY", 10, 1, 100);	//彩票比例
	defaultData.enGameMode = eOutCoin;
	defaultData.ePlayerNumber = MachineType(reader.ReadProfileValue("PLAYERNUMBER", 10, 3, 10));
	m_eLockFishFlag = (QeLockFishFlag)reader.ReadProfileValue("LockFishFlag", eLockFishAnyTime, eLockFishNone, eLockFishAnyTime);
	
	iMaxGunLevelRange1  = reader.ReadProfileValue("MAXGUNRANGE1", 10, 1, 10000);
	iMaxGunLevelRange2  = reader.ReadProfileValue("MAXGUNRANGE2", 9999, 1, 10000);
	iMinGunLevelRange1	= reader.ReadProfileValue("MINGUNRANGE1", 1, 1, 10000);
	iMinGunLevelRange2	= reader.ReadProfileValue("MINGUNRANGE2", 10, 1, 10000);
	if (iMaxGunLevelRange1 > iMaxGunLevelRange2)
		QcMath::swap(iMaxGunLevelRange1, iMaxGunLevelRange2);
	if (iMinGunLevelRange1 > iMinGunLevelRange2)
		QcMath::swap(iMinGunLevelRange1, iMinGunLevelRange2);
	defaultData.iMaxGunLevel		= iMaxGunLevelRange2;
	defaultData.iMinGunLevel		= iMinGunLevelRange1;
	defaultData.iBaoJiFenShu		= reader.ReadProfileValue("BAOJIFENSHU", 100000, 10000, 90000000);
    defaultData.iGambleOver         = reader.ReadProfileValue("BIBEIBAOJI", 100000, 20000, 1000000);
	defaultData.iResolution = eDefaultNone;
	defaultData.iJiaPaoFuDu			= reader.ReadProfileValue("JIAPAOFUDU", 1, 1, QcMath::min_(m_storageData.iMaxGunLevel, 1000));						//加炮幅度

	defaultData.iDaMaPassword = 1118481;
}

void QcSystemSetting::TryToSendDaMaPasswordToIO()
{
#ifdef QmIOUsbOn
	{
		u8 password[8] = {0};
		u32 nMatchineIdCount = QmCountOf(password);
		UInt32ToA(GetDaMaPassword(), password, nMatchineIdCount);
		BYTE iRet = TransmitComputationalFormula(password);
		if (iRet)
			QmLog("Fail to send DaMaPassword %d", iRet);
	}
#endif
}

void QcSystemSetting::Write(QcFileWriter& writer)
{
	writer.Write(m_storageData);
	writer.Write(m_BackupImportantData);
}

void QcSystemSetting::Read(QcFileReader& reader)
{
	reader.Read(m_storageData);
	reader.Read(m_BackupImportantData);
}