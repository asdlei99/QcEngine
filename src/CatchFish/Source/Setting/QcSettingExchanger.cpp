#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcSettingExchanger.hpp"
#include "QcSettingMgr.hpp"
#include "FileStorageSystem\QcSystemSetting.hpp"
#include "FileStorageSystem\QcPlayerInforList.hpp"
#include "Scene\QcPlayerMgr.hpp"

#define QmExchangerDlgCb(fun) m_pSettingMgr->QmRegisterDlgCb(#fun, &QcSettingExchanger::fun, this)
#define QmExchangerBtnCb(fun) m_pSettingMgr->QmRegisterBtnCb(#fun, &QcSettingExchanger::fun, this)
#define QmExchangerEnumCb(fun) m_pSettingMgr->QmRegisterEnumCb(#fun, &QcSettingExchanger::fun, this)
#define QmExchangerDigitalCb(fun) m_pSettingMgr->QmRegisterDigitalCb(#fun, &QcSettingExchanger::fun, this)
#define QmExchangerDigitalInputCb(fun) m_pSettingMgr->QmRegisterDigitalInputCb(#fun, &QcSettingExchanger::fun, this)
#define QmExchangerInputCheckCb(fun) m_pSettingMgr->QmRegisterInputCheckCb(#fun, &QcSettingExchanger::fun, this)

QcSettingExchanger::QcSettingExchanger()
: m_pSettingMgr(NULL), m_pSystemSetting(NULL)
{
}

void QcSettingExchanger::Init(QcSettingMgr* pSettingMgr, QcSystemSetting* pSystemSetting)
{
	m_pSettingMgr = pSettingMgr;
	m_pSystemSetting = pSystemSetting;

	m_sSettingData = m_pSystemSetting->m_storageData;

	int iLastProfit = 0;
	m_iTotalProfit = 0;
	QcPlayerInforList* pPlayerInforList = GetPlayerInfoList();
	for (int i=0; i<QmMaxPlayer; ++i)
	{
		QcPlayerInfo* pPlayerInfo = pPlayerInforList->GetPlayerInfo(i);
		iLastProfit += pPlayerInfo->GetLiRunLast();
		m_iTotalProfit += (pPlayerInfo->GetInScore() + pPlayerInfo->GetInCoin()) - (pPlayerInfo->GetOutCoin() + pPlayerInfo->GetOutScore() + pPlayerInfo->GetOutLottery());
	}
	m_iCurrentProfit = m_iTotalProfit - iLastProfit;

	m_iRebootTimeCount = 0;
	m_iDelayTimeCount = 0;

	RegisterCb();
}

void QcSettingExchanger::RegisterCb()
{
	QmExchangerBtnCb(OnShowParaSetting);
	QmExchangerBtnCb(OnShowDecodeSetting);
	QmExchangerBtnCb(OnShowCheckAccount);
	QmExchangerBtnCb(OnShowAccount);
	QmExchangerBtnCb(OnShowSystemSetting);
	QmExchangerBtnCb(OnMainExit);

	//参数设置
	QmExchangerDlgCb(OnParameterSetting);
	QmExchangerDigitalCb(OnExchangeMinGunLevel);
	QmExchangerDigitalCb(OnExchangeMaxGunLevel);
	QmExchangerBtnCb(OnParasetRecover);
	QmExchangerBtnCb(OnParasetNoSaveExit);
	QmExchangerBtnCb(OnParasetSaveExit);

	//解码设置
	QmExchangerDlgCb(OnDecodeSetting);
	QmExchangerDigitalInputCb(OnDecodeSettingSysTime);
	QmExchangerBtnCb(OnDecodeSettingBtn);
	QmExchangerBtnCb(OnDecodeSettingExitBtn);
	//解码输入框
	QmExchangerDlgCb(OnDecodeInput);

	//查账
	QmExchangerDlgCb(OnCheckAccount);
	QmExchangerBtnCb(OnCheckExit);

	//报账
	QmExchangerDlgCb(OnAccount);
	QmExchangerInputCheckCb(OnDaMa);
	QmExchangerDlgCb(OnAccountMenu);
	QmExchangerBtnCb(OnAccountMenuDelay);
	QmExchangerBtnCb(OnAccountMenuDaMa);
	QmExchangerDigitalCb(OnDelayTimeUpdate);
	//Account Warning
	QmExchangerBtnCb(OnAccountWarningYes);
	QmExchangerBtnCb(OnAccountWarningNo);

	//SysSetting
	QmExchangerDlgCb(OnPasswordInput);
	QmExchangerDlgCb(OnEndLineNumEditor);
	QmExchangerDlgCb(OnEndMatchineIDEditor);
	QmExchangerDlgCb(OnEndDaMaPasswordEditor);
	QmExchangerDlgCb(OnExDifficultCheck);
	QmExchangerEnumCb(OnExDifficult);
	QmExchangerBtnCb(OnSysSettingExDiffOK);

	//Reboot
	QmExchangerDigitalCb(OnRebootTimeUpdate);
}

void QcSettingExchanger::SetDaMaFlag(QeDaMaFlag eFlag) 
{
	m_sSettingData.eDaMaFlag = eFlag; 
	m_pSystemSetting->SetDaMaFlag(eFlag);
	if (eFlag == eDaMaClear)
	{
		GetSysSeting()->SaveBackupData(m_sSettingData.m_importantData);
	}
}

QcPlayerInforList* QcSettingExchanger::GetPlayerInfoList()
{
	return QmSceneMgr->GetPlayerMgr()->GetPlayerInfoList();
}

void QcSettingExchanger::OnExchangeSettingValue(bool bUpdate)
{
	if (!bUpdate)
	{
		QmLocker(GetSysSeting()->GetCS());
		m_sSettingData = GetSysSeting()->m_storageData;
	}
	else
	{
		//GetSysSeting()->m_storageData.m_importantData;
		//不能将m_sSettingData.m_importantData 的数据直接更新GetSysSeting()->m_storageData.m_importantData, 哪些数据需要打码后才能应用
		QcLock lock(GetSysSeting()->GetCS());
		QsSysBackupData save = GetSysSeting()->m_storageData.m_importantData;
		GetSysSeting()->m_storageData = m_sSettingData;
		GetSysSeting()->m_storageData.m_importantData = save;
		GetSysSeting()->SetDirty(true);
	}
}


//Main menu
void QcSettingExchanger::OnShowParaSetting()
{
	m_pSettingMgr->ToShowDialog(QmObjType(ParameterSetting));
}
void QcSettingExchanger::OnShowDecodeSetting()
{
	m_pSettingMgr->ToShowDialog(QmObjType(DecodeSetting));
}
void QcSettingExchanger::OnShowCheckAccount()
{
	m_pSettingMgr->ToShowDialog(QmObjType(CheckAccount));
}
void QcSettingExchanger::OnShowAccount()
{
	if (m_iCurrentProfit <= 0)
	{
		m_pSettingMgr->ToShowDialog(QmObjType(AccountMenu));
	}
	else
	{
		m_pSettingMgr->ToShowDialog(QmObjType(AccountWarning));
	}
	
}
void QcSettingExchanger::OnShowSystemSetting()
{
	m_pSettingMgr->ToShowDialog(QmObjType(PasswordDlg), false);
}
void QcSettingExchanger::OnMainExit()
{
	m_iRebootTimeCount = 4;
	m_pSettingMgr->ToShowDialog(QmObjType(Reboot));
}

void QcSettingExchanger::OnRebootTimeUpdate(s32& iValue, bool bUpdate)
{
	if (m_iRebootTimeCount > 0 && !bUpdate)
	{
		--m_iRebootTimeCount;
		iValue = m_iRebootTimeCount;
		if (m_iRebootTimeCount == 0)
		{
			QmSceneMgr->PostEvent(eEvtSceneStateChange, QcSceneMgr::eSceneSettingEnd);
		}
	}
}


//ParameterSetting
void QcSettingExchanger::OnParameterSetting(int iFlag)
{
	switch(iFlag)
	{
	case eDlgInit:
		{
			QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();

			int iDifficultLevel = m_sSettingData.m_importantData.iDifficultLevel;
			if (iDifficultLevel == eDifficultPlus3)
			{
				iDifficultLevel = eDifficult3;
			}
			else if (iDifficultLevel == eDifficultPlus4)
			{
				iDifficultLevel = eDifficult4;
			}

			pDlg->SetValue(1, iDifficultLevel);
			pDlg->SetValue(2, m_sSettingData.m_importantData.iCaiPiaoBiLiCoin);
			pDlg->SetValue(3, m_sSettingData.m_importantData.iCaiPiaoBiLiCaiPiao);
			pDlg->SetValue(4, m_sSettingData.enGameMode);
			pDlg->SetValue(5, m_sSettingData.ePlayerNumber);
			pDlg->SetValue(6, m_sSettingData.iJiaPaoFuDu);
			pDlg->SetValue(7, m_sSettingData.iMinGunLevel);
			pDlg->SetValue(8, m_sSettingData.iMaxGunLevel);
            pDlg->SetValue(9, (m_sSettingData.iBaoJiFenShu)/10000);
            pDlg->SetValue(10, m_sSettingData.iResolution);
            pDlg->SetValue(11,(m_sSettingData.iGambleOver)/10000);
			pDlg->SetValue(7, "min", (s32)GetSysSeting()->GetMinGunLevelRange1());
			pDlg->SetValue(7, "max", (s32)GetSysSeting()->GetMinGunLevelRange2());
			pDlg->SetValue(8, "min", (s32)GetSysSeting()->GetMaxGunLevelRange1());
			pDlg->SetValue(8, "max", (s32)GetSysSeting()->GetMaxGunLevelRange2());

			break;
		}
	case eDlgOK:
		{
			QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();
			int iDiff = m_sSettingData.m_importantData.iDifficultLevel;
			pDlg->GetValue(1, iDiff);
			if (iDiff != m_sSettingData.m_importantData.iDifficultLevel)
			{
				if (! ((m_sSettingData.m_importantData.iDifficultLevel == eDifficultPlus3 && iDiff == eDifficult3)
					|| (m_sSettingData.m_importantData.iDifficultLevel == eDifficultPlus4 && iDiff == eDifficult4)) )
				{
					m_sSettingData.m_importantData.iDifficultLevel = iDiff; 
				}
			}

			pDlg->GetValue(2, m_sSettingData.m_importantData.iCaiPiaoBiLiCoin);
			pDlg->GetValue(3, m_sSettingData.m_importantData.iCaiPiaoBiLiCaiPiao);
			pDlg->GetValue(4, m_sSettingData.enGameMode);
			pDlg->GetValue(5, m_sSettingData.ePlayerNumber);
			pDlg->GetValue(6, m_sSettingData.iJiaPaoFuDu);
			pDlg->GetValue(7, m_sSettingData.iMinGunLevel);
			pDlg->GetValue(8, m_sSettingData.iMaxGunLevel);
			pDlg->GetValue(9, m_sSettingData.iBaoJiFenShu);
            pDlg->GetValue(10, m_sSettingData.iResolution);
            m_sSettingData.iBaoJiFenShu *= 10000;
            pDlg->GetValue(11,m_sSettingData.iGambleOver);
            m_sSettingData.iGambleOver *= 10000;
			break;
		}
	}
}
void QcSettingExchanger::OnExchangeMinGunLevel(s32& iValue, bool bUpdate)
{
	if (bUpdate)
	{
		m_sSettingData.iMinGunLevel = iValue;

		QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();
		pDlg->SetValue(8, "min", m_sSettingData.iMinGunLevel);		//限定最大炮数的最小值
	}
	else
	{
		iValue = m_sSettingData.iMinGunLevel;
	}
}
void QcSettingExchanger::OnExchangeMaxGunLevel(s32& iValue, bool bUpdate)
{
	if (bUpdate)
	{
		m_sSettingData.iMaxGunLevel = iValue;

		QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();
		pDlg->SetValue(7, "max", m_sSettingData.iMaxGunLevel);		//限定最小炮数的最大值
	}
	else
	{
		iValue = m_sSettingData.iMaxGunLevel;
	}
}
void QcSettingExchanger::OnParasetRecover()
{
	GetSysSeting()->RecoverDefault(m_sSettingData,false);
	OnParameterSetting(eDlgInit);
}
void QcSettingExchanger::OnParasetNoSaveExit()
{
	OnExchangeSettingValue(false);
	m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
}
void QcSettingExchanger::OnParasetSaveExit()
{
	OnParameterSetting(eDlgOK);
	OnExchangeSettingValue(true);

	if (GetSysSeting()->IsNeedClearAccount(m_sSettingData.m_importantData))
	{
		SetDaMaFlag(eDaMaClear); 
		m_pSettingMgr->ToShowDialog(QmObjType(Account));
	}
	else
	{
		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
}


//DecodeSetting
void QcSettingExchanger::OnDecodeSetting(int iFlag)
{
	switch(iFlag)
	{
	case eDlgInit:
		{
			QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();

			pDlg->SetValue(1, 1);
			pDlg->SetValue(2, m_sSettingData.m_importantData.iFireQtyOneCoin);
			pDlg->SetValue(3, m_sSettingData.eChangDiMode);
			pDlg->SetValue(4, m_sSettingData.iDaMaTian);
			pDlg->SetValue(6, m_sSettingData.bShowDaMaInfo ? 1 : 0);
			pDlg->SetTip(NULL, 1);

			break;
		}
	}
}
void QcSettingExchanger::OnDecodeSettingSysTime(BYTE* pValue,s32& nBit,bool bUpdate)
{
	if (!bUpdate)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		char buf[QmFormatBuffer];
		sprintf_s(buf, "%04d-%02d-%02d %02d:%02d:%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		nBit = strlen(buf);
		memcpy_s(pValue, QmMaxDigital, buf, nBit);
	}
}
void QcSettingExchanger::OnDecodeSettingBtn()
{
	m_pSettingMgr->ToShowDialog(QmObjType(DecodeInputDlg), false);
}
void QcSettingExchanger::OnDecodeSettingExitBtn()
{
	QcDialog* pDialog = m_pSettingMgr->FindDialogEx(QmObjName(DecodeSetting));
	pDialog->SetTip(NULL, 1);
	OnExchangeSettingValue(true);

	if (GetSysSeting()->IsNeedClearAccount(m_sSettingData.m_importantData))
	{
		SetDaMaFlag(eDaMaClear); 
		m_pSettingMgr->ToShowDialog(QmObjType(Account));
	}
	else
	{
		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
}

//DecodeInputDlg
void QcSettingExchanger::OnDecodeInput(int iFlag)
{
	switch(iFlag)
	{
	case eDlgInit:
		{
			u8 bits[8] = {0};
			u8 inputBits[12] = {0};
			UInt32ToA(m_sSettingData.iJiTaiHao, bits, QmCountOf(bits));
			get_JMTZM(m_sSettingData.iJiTaiHao, m_arDecodeCheckCode);

			QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();
			pDlg->SetValue(1, bits, QmCountOf(bits));
			pDlg->SetValue(2, m_arDecodeCheckCode, QmCountOf(m_arDecodeCheckCode));
			pDlg->SetValue(4, inputBits, QmCountOf(inputBits));
			pDlg->WidgetVisible(5, false);
			break;
		}
	case eDlgCancel:
		{
			QmSettingMgr->ToShowDialog(QmObjType(DecodeSetting));
			break;
		}
	case eDlgOK:
		{
			u32 nBit;
			u32 nNum;
			BYTE inputBits[QmMaxDigital];
			QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();
			pDlg->GetValue(4, inputBits, nBit);
			QcString str;
			if (OnDecodeInputCheck(inputBits, nNum, nBit))
			{
				QcDialog* pDialog = m_pSettingMgr->FindDialogEx(QmObjName(DecodeSetting));
				QmSettingMgr->ShowDialog(QmObjType(DecodeSetting));
				if (QmSysSetting->GetbShowDaMaInfo())
				{
					QcResManager* pResMgr = QmResMgr;
					switch(nNum)
					{
					case 0:str = QmObjName(DecodeSettingCoinTip);
						break;
					case 1:str = QmObjName(DecodeSettingChangDiTip);
						break;
					case 2:str = QmObjName(DecodeSettingDayCountTip);
						break;
					case 3:str = QmObjName(DecodeSettingSysTimeTip);
						break;
					case 4:str = QmObjName(DecodeSettingShowTip);
						break;
					}
					QcResImageDesc* QImage = pResMgr->GetImageDesc(str);
					pDialog->SetTip(QImage,1);
				}
			}
			else
			{
				pDlg->WidgetVisible(5, true);
			}
			break;
		}
	}
}
s32 QcSettingExchanger::OnDecodeInputCheck(u8 *pInData,  u32 &nNum,s32)
{
	QmSysSetting->TryToSendDaMaPasswordToIO();

	s32 iRet = 0;
	
	u8 uState1 = 0;
	u8 uState2 = 0;
	u8 uYear = 0;
	u8 uMonth = 0;
	u8 uDate = 0;
	u8 uHour = 0;
	u8 uMin = 0;
#ifdef QmIOUsbOn
	u32 iCheckValue = U8ToUInt32(m_arDecodeCheckCode, QmCountOf(m_arDecodeCheckCode));
	if (JiSuan_TZM_decrypt(iCheckValue, pInData, &uState1, &uState2, &uYear, &uMonth, &uDate, &uHour, &uMin))
#endif
	{
		iRet = 1;
		nNum = uState1;
		switch(uState1)
		{
		case 0:
			if (uState2 <= 9)
			{
				m_sSettingData.m_importantData.iFireQtyOneCoin = uState2;
			}
			else if (uState2 <= 19)
			{
				m_sSettingData.m_importantData.iFireQtyOneCoin = 10 + (uState2-10)*10;
			}
			else
			{
				m_sSettingData.m_importantData.iFireQtyOneCoin = 100 + (uState2-19) * 100;
			}
			break;
		case 1:
			m_sSettingData.eChangDiMode = (uState2-1);
			break;
		case 2:
			m_sSettingData.iDaMaTian = uState2;
			m_sSettingData.iLeaveTime = SpanTime(0, m_sSettingData.iDaMaTian);
			break;
		case 3:
			{
				SYSTEMTIME tm;
				GetLocalTime(&tm);

				tm.wYear = (WORD)(2000+uYear);
				tm.wMonth = (WORD)uMonth;
				tm.wDay = (WORD)uDate;
				tm.wHour = (WORD)uHour;
				tm.wMinute = (WORD)uMin;
				if (!SetLocalTime(&tm))
				{
					QmLog("fail to SetLocalTime");
				}
				break;
			}
		case 4:
			m_sSettingData.bShowDaMaInfo = (uState2==1);
			break;
		default:
			QmAssert(false);
			iRet = 0;
		}
		OnExchangeSettingValue(true);
	}
	return iRet;
}


//CheckAccount
void QcSettingExchanger::OnCheckAccount(int iFlag)
{
	switch(iFlag)
	{
	case eDlgInit:
		{
			int iLastProfit = 0;
			int iInScore = 0;
			int iOutScore = 0;
			int iInCoin = 0;
			int iOutCoin = 0;
			int iOutLottery = 0;
			int iLeaveTime = QcMath::ceil32(QmSysSetting->GetLeaveTime()/60.f);

			QcPlayerInforList* pInforList = GetPlayerInfoList();
			for (int i=0; i<QmMaxPlayer; ++i)
			{
				iLastProfit += pInforList->GetPlayerInfo(i)->GetLiRunLast();
				iInScore += pInforList->GetPlayerInfo(i)->GetInScore();
				iOutScore += pInforList->GetPlayerInfo(i)->GetOutScore();
				iInCoin += pInforList->GetPlayerInfo(i)->GetInCoin();
				iOutCoin += pInforList->GetPlayerInfo(i)->GetOutCoin();
				iOutLottery += pInforList->GetPlayerInfo(i)->GetOutLottery();
			}
			m_iTotalProfit = iInScore + iInCoin - (iOutScore + iOutCoin + iOutLottery);
			m_iCurrentProfit = m_iTotalProfit - iLastProfit;

			QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();

			pDlg->SetValue(1, m_iTotalProfit);
			pDlg->SetValue(2, iLastProfit);
			pDlg->SetValue(3, m_iCurrentProfit);
			pDlg->SetValue(4, iInScore);
			pDlg->SetValue(5, iOutScore);
			pDlg->SetValue(6, iInCoin);
			pDlg->SetValue(7, iOutCoin);
			pDlg->SetValue(8, iOutLottery);

			u8 arNum1[3] = {0};
			UInt32ToA(m_sSettingData.iXianHao, arNum1, QmCountOf(arNum1));
			pDlg->SetValue(9, arNum1, QmCountOf(arNum1));

			u8 arNum2[8] = {0};
			UInt32ToA(m_sSettingData.iJiTaiHao, arNum2, QmCountOf(arNum2));
			pDlg->SetValue(10, arNum2, QmCountOf(arNum2));

			pDlg->SetValue(11, iLeaveTime);
			pDlg->SetValue(12, m_sSettingData.iDaMaCiShu);
			break;
		}
	}
}
void QcSettingExchanger::OnCheckExit()
{
	m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
}

//Account
void QcSettingExchanger::OnAccount(int iFlag)
{
	switch(iFlag)
	{
	case eDlgInit:
		{
			JiaoYan_JYM_encipher(m_iTotalProfit, m_iCurrentProfit, m_sSettingData.iJiTaiHao, m_sSettingData.iDaMaCiShu, m_arDaMaCheckCode);
			u8 temp[5] = {0};
			CheckNumChanged((u8)QmFishAlg->GetDiff(), m_arDaMaCheckCode, temp);


			QcDialog* pDlg = m_pSettingMgr->GetActiveDialog();

			pDlg->SetValue(1, m_iTotalProfit);
			pDlg->SetValue(2, m_iCurrentProfit);

			u8 arNum2[8] = {0};
			UInt32ToA(m_sSettingData.iJiTaiHao, arNum2, QmCountOf(arNum2));
			pDlg->SetValue(3, arNum2, QmCountOf(arNum2));

			pDlg->SetValue(4, m_sSettingData.iDaMaCiShu);
			pDlg->SetValue(5, temp, QmCountOf(temp));
			break;
		}
	}
}
s32 QcSettingExchanger::OnDaMa(u8 *pInData, s32)
{
    QmSysSetting->TryToSendDaMaPasswordToIO();
    u8 out = 0;
#ifdef QmIOUsbOn
    WORD wLine = 1000;
    DWORD dwMechine = 100000000;
    GetClassGroupNum(&wLine, &dwMechine);
    Sleep(5);
    if (int(wLine) != m_sSettingData.iXianHao || int(dwMechine) != m_sSettingData.iJiTaiHao)
        return 0;

	int iResult = JiSuan_DKXZM_decrypt(m_iTotalProfit, m_iCurrentProfit, m_sSettingData.iJiTaiHao, m_sSettingData.iDaMaCiShu, m_arDaMaCheckCode, pInData, &out);
#else
	int iResult = 1;
#endif
	if ( iResult == 1 || iResult == 2)
	{
		if (m_sSettingData.eDaMaFlag != eDaMaNone)
		{
			if (m_iCurrentProfit)
			{
				QcPlayerInforList* pPlayerInforList = GetPlayerInfoList();
				for (int i=0; i<QmMaxPlayer; ++i)
				{
					pPlayerInforList->GetPlayerInfo(i)->ClearCurrent();
				}
				m_iCurrentProfit = 0;
			}
			else
			{
				QcPlayerInforList* pPlayerInforList = GetPlayerInfoList();
				for (int i=0; i<QmMaxPlayer; ++i)
				{
					pPlayerInforList->GetPlayerInfo(i)->ClearAll();
				}
				m_iTotalProfit = 0;
				QmFishAlg->Reset();

				int iCurrentLevel = GetSysSeting()->GetBackupData().iDifficultLevel;
				int iDestLevel = iCurrentLevel;
				if (iResult == 2)
				{
					if (iCurrentLevel == eDifficult3)
						iDestLevel = eDifficultPlus3;
					else if (iCurrentLevel == eDifficult4)
						iDestLevel = eDifficultPlus4;
				}
				else if (iResult == 1)
				{	
					if (iCurrentLevel == eDifficultPlus3)
						iDestLevel = eDifficult3;
					else if (iCurrentLevel == eDifficultPlus4)
						iDestLevel = eDifficult4;
				}
				if (m_sSettingData.eDaMaFlag == eDaMaClear || iCurrentLevel != iDestLevel)
				{
					GetSysSeting()->SetBackupDataDifficultLevel(iDestLevel);
					GetSysSeting()->ApplyBackupData();
					m_sSettingData.m_importantData = GetSysSeting()->GetBackupData();
					SetDaMaFlag(eDaMaNone);
				}
				QmClearLog();
			}
			if (m_sSettingData.eDaMaFlag == eDaMaNormal)
				SetDaMaFlag(eDaMaNone);

			GetSysSeting()->AddDaMaCiShu();
			GetSysSeting()->SetLeaveTime(SpanTime(0, GetSysSeting()->GetDaMaTian()));
			m_sSettingData.iDaMaCiShu = GetSysSeting()->GetDaMaCiShu();

            if (iResult == 2)
            {
                QmFishAlg->SetAlgDiff(out);
                if (out == 24)
                    QmSysSetting->SetLotteryBullet(false);
            }
			return 1;
		}
	}
	return 0;
}
//AccountMenu
void QcSettingExchanger::OnAccountMenu(int iFlag)
{
	switch(iFlag)
	{
	case eDlgInit:
		{
			m_pSettingMgr->GetActiveDialog()->WidgetVisible(3, false);
			m_pSettingMgr->GetActiveDialog()->WidgetVisible(4, false);
			break;
		}
	}
}

void QcSettingExchanger::OnDelayTimeUpdate(s32& iValue, bool bUpdate)
{
	if (!bUpdate && m_iDelayTimeCount > 0)
	{
		--m_iDelayTimeCount;
		if (m_iDelayTimeCount == 0)
		{
			m_pSettingMgr->GetActiveDialog()->Enable(true);
			m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
		}
		else
			iValue = m_iDelayTimeCount;
	}
}

void QcSettingExchanger::OnAccountMenuDelay()
{
	m_sSettingData.iLeaveTime = SpanTime(0, GetSysSeting()->GetDaMaTian());

	OnExchangeSettingValue(true);
	m_iDelayTimeCount = 4;
	m_pSettingMgr->GetActiveDialog()->Update(false);
	m_pSettingMgr->GetActiveDialog()->Enable(false);

	m_pSettingMgr->GetActiveDialog()->WidgetVisible(3, true);
	m_pSettingMgr->GetActiveDialog()->WidgetVisible(4, true);
	m_pSettingMgr->GetActiveDialog()->GetWidget(4)->ResetUpdateTime();

}
void QcSettingExchanger::OnAccountMenuDaMa()
{
	SetDaMaFlag(eDaMaNormal);
	m_pSettingMgr->ToShowDialog(QmObjType(Account));
}

void QcSettingExchanger::OnAccountWarningYes()
{
	SetDaMaFlag(eDaMaNormal);
	m_pSettingMgr->HideDialog(QmObjType(SettingMain));
	m_pSettingMgr->ToShowDialog(QmObjType(Account));
}
void QcSettingExchanger::OnAccountWarningNo()
{
	m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
}


//SysSetting
void QcSettingExchanger::OnPasswordInput(int iFlag)
{
	if (iFlag == eDlgOK)
    {
        DWORD dwMechine = 1000;
        WORD wLine = 1000;
        if (GetClassGroupNum(&wLine, &dwMechine))
            return;

		u8 uMiMa[8] = {0};
		u32 nBit = 0;
		m_pSettingMgr->GetActiveDialog()->GetValue(1, uMiMa, nBit);
		u8 lastCh = uMiMa[7];
		u8 secondLastCh = uMiMa[6];
		if (lastCh == 0 && secondLastCh == 0)
		{
			if (verify_modify_xh_jth_pwd(wLine, dwMechine, 0, uMiMa))
			{
				m_pSettingMgr->HideDialog(QmObjType(SettingMain));
				m_pSettingMgr->ToShowDialog(QmObjType(SysLineNumEditor));
			}
		}
		else if (lastCh == 1 && secondLastCh == 0)
		{
			if (verify_modify_xh_jth_pwd(wLine, dwMechine, 1, uMiMa))
			{
				m_pSettingMgr->HideDialog(QmObjType(SettingMain));
				m_pSettingMgr->ToShowDialog(QmObjType(SysMatchineIDEditor));
			}
		}
		else if (lastCh == 0 && secondLastCh == 1)
		{
			m_pSettingMgr->HideDialog(QmObjType(SettingMain));
			m_pSettingMgr->ToShowDialog(QmObjType(SysExDifficultCheck));
		}
		else if (lastCh == 2 && secondLastCh == 0)
		{
			if (verify_modify_xh_jth_pwd(wLine, dwMechine, 2, uMiMa))
			{
				m_pSettingMgr->HideDialog(QmObjType(SettingMain));
				m_pSettingMgr->ToShowDialog(QmObjName(SysDaMaPasswordEditor));
			}
		}
	}
	else if (iFlag == eDlgInit)
	{
		u8 uMiMa[8] = {0};
		u32 nBit = QmCountOf(uMiMa);
		m_pSettingMgr->GetActiveDialog()->SetValue(1, uMiMa, nBit);
	}
	else if (iFlag == eDlgCancel)
	{
		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
}

void QcSettingExchanger::OnEndLineNumEditor(int iFlag)
{
	u8 lineNum[3] = {0};
	u8 version1Num[3] = {0};
	u8 version2Num[3] = {0};
	u32 nLineNumCount=QmCountOf(lineNum);
	u32 nVersion1Num=QmCountOf(version1Num);
	u32 nVersion2Num=QmCountOf(version2Num);

	if (iFlag == eDlgOK)
	{
		m_pSettingMgr->GetActiveDialog()->GetValue(1, lineNum, nLineNumCount);
		m_pSettingMgr->GetActiveDialog()->GetValue(2, version1Num, nVersion1Num);
		m_pSettingMgr->GetActiveDialog()->GetValue(3, version2Num, nVersion2Num);
		m_sSettingData.iXianHao = U8ToUInt32(lineNum, nLineNumCount);
		m_sSettingData.iBanBenHao1 = U8ToUInt32(version1Num, nVersion1Num);
        m_sSettingData.iBanBenHao2 = U8ToUInt32(version2Num, nVersion2Num);
        if (!ModifyClassGroupNum(WORD(m_sSettingData.iXianHao), m_sSettingData.iJiTaiHao))
            OnExchangeSettingValue(true);
        else OnExchangeSettingValue(false);

		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
	else if (iFlag == eDlgInit)
	{
		UInt32ToA((u32)m_sSettingData.iXianHao, lineNum, nLineNumCount);
		UInt32ToA((u32)m_sSettingData.iBanBenHao1, version1Num, nVersion1Num);
		UInt32ToA((u32)m_sSettingData.iBanBenHao2, version2Num, nVersion2Num);

		m_pSettingMgr->GetActiveDialog()->SetValue(1, lineNum, nLineNumCount);
		m_pSettingMgr->GetActiveDialog()->SetValue(2, version1Num, nVersion1Num);
		m_pSettingMgr->GetActiveDialog()->SetValue(3, version2Num, nVersion2Num);
	}
	else if (iFlag == eDlgCancel)
	{
		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
}

void QcSettingExchanger::OnEndMatchineIDEditor(int iFlag)
{
	u8 mathineId[8] = {0};
	u32 nMatchineIdCount = QmCountOf(mathineId);
	if (iFlag == eDlgOK)
	{
		m_pSettingMgr->GetActiveDialog()->GetValue(1, mathineId, nMatchineIdCount);
		m_sSettingData.iJiTaiHao = U8ToUInt32(mathineId, nMatchineIdCount);

        if (!ModifyClassGroupNum(WORD(m_sSettingData.iXianHao), m_sSettingData.iJiTaiHao))
            OnExchangeSettingValue(true);
        else OnExchangeSettingValue(false);

		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
	else if (iFlag == eDlgInit)
	{
		UInt32ToA((u32)m_sSettingData.iJiTaiHao, mathineId, nMatchineIdCount);
		m_pSettingMgr->GetActiveDialog()->SetValue(1, mathineId, nMatchineIdCount);
	}
	else if (iFlag == eDlgCancel)
	{
		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
}

void QcSettingExchanger::OnEndDaMaPasswordEditor(int iFlag)
{
	u8 password[8] = {0};
	u32 nMatchineIdCount = QmCountOf(password);
	if (iFlag == eDlgOK)
	{
		m_pSettingMgr->GetActiveDialog()->GetValue(1, password, nMatchineIdCount);
		m_sSettingData.iDaMaPassword = U8ToUInt32(password, nMatchineIdCount);
		OnExchangeSettingValue(true);

		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
	else if (iFlag == eDlgInit)
	{
		UInt32ToA(m_sSettingData.iDaMaPassword, password, nMatchineIdCount);
		m_pSettingMgr->GetActiveDialog()->SetValue(1, password, nMatchineIdCount);
	}
	else if (iFlag == eDlgCancel)
	{
		m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
	}
}

void QcSettingExchanger::OnExDifficultCheck(int iFlag)
{
	u8 checkCode[8] = {0};
	u32 nCheckCount = QmCountOf(checkCode);
	switch(iFlag)
	{
	case eDlgInit:
		{
			m_pSettingMgr->GetActiveDialog()->SetValue(2, checkCode, nCheckCount);

			m_iRandCode = (rand()%8+1)*10000000+rand()%10000000;
			UInt32ToA(m_iRandCode, checkCode, nCheckCount);
			m_pSettingMgr->GetActiveDialog()->SetValue(1, checkCode, nCheckCount);
			break;
		}
	case eDlgCancel:
		{
			m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
			break;
		}
	case eDlgOK:
		{
			m_pSettingMgr->GetActiveDialog()->GetValue(2, checkCode, nCheckCount);
			if (verify_modify_ExShowDiff2(m_sSettingData.iXianHao, m_sSettingData.iJiTaiHao, m_iRandCode, checkCode) == 1)
			{
				m_pSettingMgr->ToShowDialog(QmObjType(SysExDifficult));
			}
			else
			{
				m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
			}
			break;
		}
	}
}

void QcSettingExchanger::OnExDifficult(s32& iValue, bool bUpdate)
{
	if (bUpdate)
	{
		m_sSettingData.iShowExDifficult = iValue;
	}
	else
	{
		iValue = m_sSettingData.iShowExDifficult;
	}
}

void QcSettingExchanger::OnSysSettingExDiffOK()
{
	OnExchangeSettingValue(true);
	m_pSettingMgr->ToShowDialog(QmObjType(SettingMain));
}

