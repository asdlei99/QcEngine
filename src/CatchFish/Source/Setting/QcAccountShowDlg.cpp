#include "pch.hpp"
#include "QcAccountShowDlg.hpp"
#include "QcSettingExchanger.hpp"
#include "QcSettingMgr.hpp"
#include "Res\QcResDescParam.hpp"

ImplRuntimeClass(QcAccountShowDlg);

QcAccountShowDlg::QcAccountShowDlg()
{
	memset(m_sInput, 0, sizeof(m_sInput));
	m_nBit = 0;
}

void QcAccountShowDlg::InitDialog(QcResDialogDesc* pDialogDesc, QcDialog* pParentDlg)
{
	QcDialog::InitDialog(pDialogDesc, pParentDlg);

	m_pInputCodeText = GetWidget(100);
	m_pInputCode = GetWidget(101);
	m_pWaiting = GetWidget(102);
	m_pSuccText = GetWidget(103);
	m_pErrorText = GetWidget(104);
	m_pTimer = GetWidget(105);

	QcResDescParam* pParam = pDialogDesc->GetParam();
	if (pParam)
	{
		QcString sCheckCodeDlg;
		QcPoint vCheckDlgPos;
		sCheckCodeDlg = pParam->GetProperty("CheckCodeDlg", sCheckCodeDlg);
		vCheckDlgPos = pParam->GetProperty("CheckCodeDlgPos", vCheckDlgPos);

		m_pInputCodeDlg = QmRuntimeClass(QcDialog)->CreateObject<QcDialog>();
		m_pInputCodeDlg->InitDialog(QmSettingMgr->FindDialogDesc(sCheckCodeDlg), this);
		m_pInputCodeDlg->SetPosition(vCheckDlgPos);
		AddSubSceneNode(m_pInputCodeDlg);
	}
	SetInputCodeDlgActive(true);
	
	m_pCheckCb = QmSettingMgr->FindCallBacker("OnDaMa");
}

void QcAccountShowDlg::SetActive(bool bActive)
{
	if (bActive)
	{
		SetInputCodeDlgActive(true);
	}
}

void QcAccountShowDlg::Update(bool bUpdate)
{
	QcDialog::Update(bUpdate);
	if (m_pInputCodeDlg)
		m_pInputCodeDlg->Update(bUpdate);
	if (m_cb && !bUpdate)
		m_cb(eDlgInit);
}

void QcAccountShowDlg::SetInputCodeDlgActive(bool bActive)
{
	if (bActive)
	{
		BYTE bits[8] = {0};
		m_pInputCodeDlg->SetValue(100, bits, QmCountOf(bits));
	}
	m_pInputCodeDlg->SetActive(bActive);
	m_pInputCodeDlg->SetVisible(bActive);
	m_pInputCodeText->SetVisible(!bActive);
	m_pInputCode->SetVisible(!bActive);
	m_pWaiting->SetVisible(!bActive);
	m_pSuccText->SetVisible(!bActive);
	m_pErrorText->SetVisible(!bActive);
	m_pTimer->SetVisible(!bActive);
}

void QcAccountShowDlg::OnKeyDown(QeKey key, const QsKeyInfo& info)
{
	if (m_pInputCodeDlg->IsVisible())
		m_pInputCodeDlg->OnKeyDown(key, info);
}
void QcAccountShowDlg::OnKeyUp(QeKey key, const QsKeyInfo& info)
{
	if (m_pInputCodeDlg->IsVisible())
	{
		m_pInputCodeDlg->OnKeyUp(key, info);
		if (key == eSmallKeyOk)
		{
			m_pInputCodeDlg->GetValue(100, m_sInput, m_nBit);
			SetInputCodeDlgActive(false);
			if (m_pCheckCb(m_sInput, m_nBit) == 1)
			{
				m_pErrorText->SetVisible(false);
			}
			else
			{
				m_pSuccText->SetVisible(false);
			}
			m_nTime = 10;
			SetTimer(eCountDownTimer, 1.f);
			m_pInputCode->Update(m_sInput, m_nBit, false);
			m_pTimer->Update(m_nTime, false);
		}
#ifdef QmTestVersion
		else if (key == eSmallKeyCancel)
		{
			QmSettingMgr->ToShowDialog(QmObjType(SettingMain));
		}
#endif
	}
	else
	{
		QcDialog::OnKeyUp(key, info);
	}
}

void QcAccountShowDlg::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eCountDownTimer)
	{
		--m_nTime;
		if (m_nTime != 0)
			m_pTimer->Update(m_nTime, false);
		else
		{
			KillTimer(eCountDownTimer);
			if (m_pErrorText->IsVisible() || QmSysSetting->GetDaMaFlag() == eDaMaClear)
			{
				SetInputCodeDlgActive(true);
				Update(false);
				m_cb(eDlgInit);
			}
			else
			{
				QmSettingMgr->ToShowDialog(QmObjType(SettingMain));
			}
		}
	}
}