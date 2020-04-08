#pragma once

#include "Setting\QcWidgetCallBacker.hpp"
#include "FileStorageSystem\QsSysStorageData.hpp"
class QcPlayerInforList;
class QcSettingExchanger : public QcObject
{
public:
	QcSettingExchanger();

	void Init(QcSettingMgr* pSettingMgr, QcSystemSetting* pSystemSetting);
protected:
	//Main menu
	void OnShowParaSetting();
	void OnShowDecodeSetting();
	void OnShowCheckAccount();
	void OnShowAccount();
	void OnShowSystemSetting();
	void OnMainExit();
	void OnRebootTimeUpdate(s32& iValue, bool bUpdate);

	//ParameterSetting
	void OnParameterSetting(int iFlag);
	void OnExchangeMinGunLevel(s32& iValue, bool bUpdate);
	void OnExchangeMaxGunLevel(s32& iValue, bool bUpdate);
	void OnParasetRecover();
	void OnParasetNoSaveExit();
	void OnParasetSaveExit();

	//DecodeSetting
	void OnDecodeSetting(int iFlag);
	void OnDecodeSettingSysTime(BYTE*,s32& nBit,bool bUpdate);
	void OnDecodeSettingBtn();
	void OnDecodeSettingExitBtn();
	//DecodeInputDlg
	void OnDecodeInput(int iFlag);
	s32 OnDecodeInputCheck(u8 *pInData,  u32 &nNum,s32);

	//CheckAccount
	void OnCheckAccount(int iFlag);
	void OnCheckExit();

	//Account
	void OnAccount(int iFlag);
	s32 OnDaMa(u8 *pInData, s32 nInNum);
	//AccountMenu
	void OnAccountMenu(int iFlag);
	void OnAccountMenuDelay();
	void OnAccountMenuDaMa();
	void OnDelayTimeUpdate(s32& iValue, bool bUpdate);
	
	//Account Warning
	void OnAccountWarningYes();
	void OnAccountWarningNo();

	//SysSetting
	void OnPasswordInput(int iFlag);
	void OnEndLineNumEditor(int iFlag);
	void OnEndMatchineIDEditor(int iFlag);
	void OnEndDaMaPasswordEditor(int iFlag);
	void OnExDifficultCheck(int iFlag);
	void OnExDifficult(s32& iValue, bool bUpdate);
	void OnSysSettingExDiffOK();
protected:
	void OnExchangeSettingValue(bool bUpdate);
	void RegisterCb();
	void SetDaMaFlag(QeDaMaFlag eFlag);
	QcPlayerInforList* GetPlayerInfoList();
	QcSystemSetting* GetSysSeting() {return m_pSystemSetting;}
protected:
	QcSettingMgr* m_pSettingMgr;
	QcSystemSetting* m_pSystemSetting;
	QsSysStorageData m_sSettingData;

	int m_iTotalProfit;
	int m_iCurrentProfit;
	int m_iRandCode;
	s32 m_iRebootTimeCount;
	s32 m_iDelayTimeCount;

	u8	m_arDecodeCheckCode[8];
	u8  m_arDaMaCheckCode[4];
};