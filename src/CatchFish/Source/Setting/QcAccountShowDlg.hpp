#pragma once

#include "Setting\QcDialog.hpp"

class QcAccountShowDlg : public QcDialog
{
	DeclareRuntimeClass(QcAccountShowDlg)
public:
	QcAccountShowDlg();

	virtual void InitDialog(QcResDialogDesc* pDialogDesc, QcDialog* pParentDlg = NULL);
	virtual void SetActive(bool bActive);
	virtual void Update(bool bUpdate);
	virtual void OnTimer(u32 nTimer, void* pContext);
	virtual void OnKeyDown(QeKey key, const QsKeyInfo& info);
	virtual void OnKeyUp(QeKey key, const QsKeyInfo& info);
	/*virtual bool FindWidgetCallBacker(const QcString& sAction, QcWidgetCallBacker& cb);*/
protected:
	/*void OnTimeUpdate(Int32&, bool bUpdate);
	void OnInputCodeUpdate(BYTE*,Int32& nBit,bool bUpdate);
	void OnInputCode(BYTE*,Int32& nBit,bool bUpdate);*/
	void SetInputCodeDlgActive(bool bActive);
protected:
	QcDialogPtr m_pInputCodeDlg;
	QcWidgetPtr m_pInputCodeText;
	QcWidgetPtr m_pInputCode;
	QcWidgetPtr m_pWaiting;
	QcWidgetPtr m_pSuccText;
	QcWidgetPtr m_pErrorText;
	QcWidgetPtr m_pTimer;
	bool m_bSucc;
	u32 m_nBit;
	BYTE m_sInput[QmMaxDigital];
	s32 m_nTime;
	QcWidgetCallBacker m_pCheckCb;
};