#include "pch.hpp"
#include "QcDecodeInputDlg.hpp"

ImplRuntimeClass(QcDecodeInputDlg);

QcDecodeInputDlg::QcDecodeInputDlg()
{
	
}

void QcDecodeInputDlg::InitDialog(QcResDialogDesc* pDialogDesc, QcDialog* pParentDlg)
{
	QcDialog::InitDialog(pDialogDesc, pParentDlg);

	m_pInputCode = GetWidget(InputID);
	m_pTipWidget = GetWidget(TipID);

	m_pCheckFun = QmSettingMgr->FindCallBacker("OnDecodeInputCheck");
}

void QcDecodeInputDlg::OnKeyDown(QeKey key, const QsKeyInfo& info)
{
	if (m_pTipWidget->IsVisible())
		m_pTipWidget->SetVisible(false);

	QcDialog::OnKeyDown(key, info);
}

void QcDecodeInputDlg::OnKeyUp(QeKey key, const QsKeyInfo& info)
{
	QcDialog::OnKeyUp(key, info);
	if (key == eSmallKeyOk)
	{
		UInt32 nBit;
		BYTE sBuf[QmMaxDigital];
		m_pInputCode->Update(sBuf, nBit, true);
		if (m_pCheckFun(sBuf, nBit))
		{
			QmSettingMgr->ToShowDialog(QmObjType(DecodeSetting));
		}
		else
		{
			m_pTipWidget->SetVisible(true);
		}
	}
	else if (key == eSmallKeyCancel)
	{
		QmSettingMgr->ToShowDialog(QmObjType(DecodeSetting));
	}
}