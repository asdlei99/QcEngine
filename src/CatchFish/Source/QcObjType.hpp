#pragma once

typedef QcString QcObjType;
#define QmDefineObjType(name) const QcObjType ID_##name(#name)
#define QmObjType(name) ID_##name
#define QmObjName(name) QcObjType(#name)

//Empty Type
const QcObjType ID_NULL;

//Player
const QcObjType QaPlayerList[] = 
{
	QmObjName(Player1),
	QmObjName(Player2),
	QmObjName(Player3),
	QmObjName(Player4),
	QmObjName(Player5),
	QmObjName(Player6),
	QmObjName(Player7),
	QmObjName(Player8),
	QmObjName(Player9),
	QmObjName(Player10),
	QmObjName(Player11),
	QmObjName(Player12),
	QmObjName(Player13),
	QmObjName(Player14),
	QmObjName(Player15),
	QmObjName(Player16),
	QmObjName(Player17),
	QmObjName(Player18),
	QmObjName(Player19),
	QmObjName(Player20),
};
QmDefineObjType(BaoJi);
QmDefineObjType(ExitCoin);
QmDefineObjType(ExitCoinError);
QmDefineObjType(ExitLottery);
QmDefineObjType(ExitLotteryError);

//Fish
const QcObjType QaFishList[] =
{
	QmObjName(fish19),
	QmObjName(fish18),
	QmObjName(fish17),
	QmObjName(fish16),
	QmObjName(fish15),
	QmObjName(fish14),
	QmObjName(fish13),
	QmObjName(fish12),
	QmObjName(fish11),
	QmObjName(fish10),
	QmObjName(fish09),
	QmObjName(fish08),
	QmObjName(fish07),
	QmObjName(fish06),
	QmObjName(fish05),
	QmObjName(fish04),
	QmObjName(fish03),
	QmObjName(fish02),
	QmObjName(fish01),

	QmObjName(SameKindFish),
	QmObjName(ScopeBomb),
	QmObjName(DelayScopeBomb),
    QmObjName(ChainLinkFish),
    QmObjName(GambleFish),
	QmObjName(RemoteBomberFish),
};

//Gun
const QcObjType QaGunList[] = 
{
	QmObjName(Gun1),
	QmObjName(Gun4),
	QmObjName(Gun5),
	//QmObjName(Gun4),
	//QmObjName(Gun5),
	QmObjName(EnergyGun1),
	QmObjName(EnergyGun4),
	QmObjName(EnergyGun5),
	//QmObjName(EnergyGun4),
	//QmObjName(EnergyGun5),
};
QmDefineObjType(SuperFort);
QmDefineObjType(Fort);
QmDefineObjType(GunLevelFort);
QmDefineObjType(EnergyGunEffect);

//Coin
QmDefineObjType(RedGem);
QmDefineObjType(SilveryGem);
QmDefineObjType(GoldCoin);
QmDefineObjType(SilveryCoin);
QmDefineObjType(DigitalCoin);
QmDefineObjType(GoldColumn);
QmDefineObjType(GoldColumnBag);


//Start Image
QmDefineObjType(StartLogo);
QmDefineObjType(StartLogo2);
QmDefineObjType(warning_ch);
QmDefineObjType(warning_en);
QmDefineObjType(LoadingText);

//Backgound
const QcObjType QaSceneList[] = 
{
    QmObjName(SceneB),
    QmObjName(SceneC),
	QmObjName(SceneA),
};
QmDefineObjType(SceneLogo);
QmDefineObjType(Stencil);
QmDefineObjType(Wave);
QmDefineObjType(shuiwen);

//Util
QmDefineObjType(DaMaError);
QmDefineObjType(CardError);
QmDefineObjType(ConnectError);
QmDefineObjType(ErrorBg);
QmDefineObjType(SuperGunFlag);
QmDefineObjType(CaiJinQuan);
QmDefineObjType(SetttingReady);
QmDefineObjType(QuickAccountBg);
QmDefineObjType(QuickAccountWin);
QmDefineObjType(QuickAccountWin2);
QmDefineObjType(QuickDeepSlip);
QmDefineObjType(QuickLightSlip);
QmDefineObjType(suodingkuang);
QmDefineObjType(TotalProfit);
QmDefineObjType(CurrentProfit);
QmDefineObjType(InScore);
QmDefineObjType(ExitScore);
QmDefineObjType(InCoin);
QmDefineObjType(OutCoin);
QmDefineObjType(Lottery);
QmDefineObjType(LeaveTime);
QmDefineObjType(QuickAccountCoinUnit);
QmDefineObjType(QuickAccountTimeUnit);
QmDefineObjType(VolumnBG);
QmDefineObjType(VolumnBlock);
//////////////////////////////////////////////////////////////////////////
QmDefineObjType(xuanfeng);
QmDefineObjType(getscore);
QmDefineObjType(BulletFire);
QmDefineObjType(CaiJinDigital);
QmDefineObjType(LzpQuan);
QmDefineObjType(TrackLockDigital);
QmDefineObjType(LogoForAuto);
//////////////////////////////////////////////////////////////////////////


