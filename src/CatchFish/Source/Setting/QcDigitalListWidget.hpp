#pragma once

#include "Setting\QcWidget.hpp"

class QcDigitalListWidget : public QcWidget
{
	DeclareRuntimeClass(QcDigitalListWidget);
public:
	QcDigitalListWidget();
	~QcDigitalListWidget();

	virtual void InitWidget(QcResDialogItemDesc* pWidgetDesc, QcDialog* pDialog);
	virtual bool OnUpKey(const QsKeyInfo& info);
	virtual bool OnDownKey(const QsKeyInfo& info);
	virtual bool OnLeftKey(const QsKeyInfo& info);
	virtual bool OnRightKey(const QsKeyInfo& info);
	virtual void SetFocus(bool bFocus);
	virtual void Update(bool bUpdateData);
	virtual bool Update(BYTE* pValue,u32& nBit, bool bUpdateData);
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix);
	virtual void SetActive(bool bActive);
protected:
	virtual void ActiveAgain(bool bForward);
protected:
	s32 m_iCurrentBit;
	s32 m_nBit;
	BYTE m_sBuf[QmMaxDigital];
	QcSharedPtr<QcDigitalNode> m_pDigitalNode;
};