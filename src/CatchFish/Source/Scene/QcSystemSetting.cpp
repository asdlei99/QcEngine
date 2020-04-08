#include "pch.hpp"
#include "QcSystemSetting.hpp"

QcSystemSetting::QcSystemSetting(const QsDefaultSetting& defaultSetting)
{
	iFireQtyOneCoin		= defaultSetting.iFireQtyOneCoin;
	iCaiPiaoBiLiCoin	= defaultSetting.iCaiPiaoBiLiCoin;		//彩票比例
	iCaiPiaoBiLiCaiPiao	= defaultSetting.iCaiPiaoBiLiCaiPiao;	//彩票比例
	iDifficultLevel		= defaultSetting.iDifficultLevel;			//游戏难度

	iMaxGunLevel		= defaultSetting.iMaxGunLevelRange2;			//最大炮数
	iMinGunLevel		= defaultSetting.iMinGunLevelRange1;			//最小炮数
	iJiaPaoFuDu			= defaultSetting.iJiaPaoFuDu;				//加炮幅度

	iDaMaTian			= defaultSetting.iDaMaTian;
	iDaMaCiShu			= 0;
	iShenYuShiJian		= iDaMaTian * 24 * 60;

	iXianHao			= defaultSetting.iXianHao;
	iJiTaiHao			= defaultSetting.iJiTaiHao;

	bNeedDaMa			= FALSE;

	eChangDiMode		= defaultSetting.eChangDiMode;

	iBanBenHao1			= defaultSetting.iBanBenHao1;
	iBanBenHao2			= defaultSetting.iBanBenHao2;	

	iShowExDifficult	= 0;
	ePlayerNumber = QcUtil::MachineType(defaultSetting.iPlayerNumber);
}

void QcSystemSetting::Write(QcFileWriter& writer)
{
	writer.Write(iXianHao);
	writer.Write(iJiTaiHao);
	writer.Write(iFireQtyOneCoin);
	writer.Write(iCaiPiaoBiLiCoin);
	writer.Write(iCaiPiaoBiLiCaiPiao);
	writer.Write(iDifficultLevel);
	writer.Write(iMaxGunLevel);
	writer.Write(iMinGunLevel);
	writer.Write(iJiaPaoFuDu);
	writer.Write(enGameMode);
	writer.Write(iDaMaTian);
	writer.Write(iDaMaCiShu);
	writer.Write(iShenYuShiJian);
	writer.Write(bNeedDaMa);
	writer.Write(iBanBenHao1);
	writer.Write(iBanBenHao2);
	writer.Write(iShowExDifficult);
	writer.Write(ePlayerNumber);
	writer.Write(eChangDiMode);
}

void QcSystemSetting::Read(QcFileReader& reader)
{
	reader.Read(iXianHao);
	reader.Read(iJiTaiHao);
	reader.Read(iFireQtyOneCoin);
	reader.Read(iCaiPiaoBiLiCoin);
	reader.Read(iCaiPiaoBiLiCaiPiao);
	reader.Read(iDifficultLevel);
	reader.Read(iMaxGunLevel);
	reader.Read(iMinGunLevel);
	reader.Read(iJiaPaoFuDu);
	reader.Read(enGameMode);
	reader.Read(iDaMaTian);
	reader.Read(iDaMaCiShu);
	reader.Read(iShenYuShiJian);
	reader.Read(bNeedDaMa);
	reader.Read(iBanBenHao1);
	reader.Read(iBanBenHao2);
	reader.Read(iShowExDifficult);
	reader.Read(ePlayerNumber);
	reader.Read(eChangDiMode);
}