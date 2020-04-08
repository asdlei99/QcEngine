#pragma once

#include "Setting\QcDialog.hpp"
#define InputID 4
#define TipID 5
class QcDecodeInputDlg : public QcDialog
{
	DeclareRuntimeClass(QcDecodeInputDlg);
public:
	QcDecodeInputDlg();

	virtual void InitDialog(QcResDialogDesc* pDialogDesc, QcDialog* pParentDlg = NULL);
	virtual void OnKeyDown(QeKey key, const QsKeyInfo& info);
	virtual void OnKeyUp(QeKey key, const QsKeyInfo& info);
protected:
	QcWidgetCallBacker m_pCheckFun;
	QcWidgetPtr m_pInputCode;
	QcWidgetPtr m_pTipWidget;
};