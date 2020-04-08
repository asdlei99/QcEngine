#pragma once

struct QsSysBackupData
{
	bool operator == (const QsSysBackupData& data) const
	{
		return data.iFireQtyOneCoin == iFireQtyOneCoin && 
			data.iCaiPiaoBiLiCoin == iCaiPiaoBiLiCoin &&
			data.iCaiPiaoBiLiCaiPiao == iCaiPiaoBiLiCaiPiao &&
			data.iDifficultLevel == iDifficultLevel;
	}
	bool operator != (const QsSysBackupData& data) const
	{
		return !(data == *this);
	}
	int			iFireQtyOneCoin;
	int			iCaiPiaoBiLiCoin;		//彩票比例
	int			iCaiPiaoBiLiCaiPiao;	//彩票比例
	int			iDifficultLevel;
};

enum QeResolution
{
	eDefaultNone = 0,
	e1920_1080,
	e1600_900,
	e1440_900,
	e1366_768,
	e1280_768,
};

enum QeDiffLevel
{
	eDifficult0 = 0,
	eDifficult1 = 1,
	eDifficult2 = 2,
	eDifficult3 = 3,
	eDifficult4 = 4,
	eDifficultPlus3 = 5,
	eDifficultPlus4 = 6,
};

struct QsSysStorageData
{
	int			iXianHao;
	int			iJiTaiHao;
	QeResolution			iResolution;
	QsSysBackupData m_importantData;

	int			iMaxGunLevel;			//最大炮数
	int			iMinGunLevel;			//最小炮数
	int			iJiaPaoFuDu;			//加炮幅度
	QeGameExitMode	enGameMode;			//退币模式

	int			bShowDaMaInfo;
	int			iDaMaCiShu;
	int			iDaMaTian;				//打码天数
	__time64_t  iCurrentTime;
	__time64_t  iLeaveTime;				//剩余时间

	QeDaMaFlag	eDaMaFlag;

	int			iBanBenHao1;			//主版本号
	int			iBanBenHao2;			//副版本号
	int			iShowExDifficult;
	int			iBaoJiFenShu;			//爆机分数	
    u32      iGambleOver;
	QeMachineType	ePlayerNumber;
	int			eChangDiMode;
	u32		iDaMaPassword;
	u32		iVolume;
	u32		iGameRunningTime;

    bool        bLotteryBullet;
};