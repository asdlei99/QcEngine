#pragma once

#include "Setting\QcWidget.hpp"
class QcResEnumDescParam;
class QcAnimatorFlash;
class QcEnumWidget : public QcWidget
{
	DeclareRuntimeClass(QcEnumWidget);
public:
	QcEnumWidget();

	virtual void InitWidget(QcResDialogItemDesc* pWidgetDesc, QcDialog* pDialog);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);

	virtual bool OnLeftKey(const QsKeyInfo& info);
	virtual bool OnRightKey(const QsKeyInfo& info);
	virtual void SetActive(bool bActive);
	virtual void Update(bool bUpdateData);
	virtual bool Update(s32& iValue, bool bUpdateData);
	virtual void SetValue(const char* psKey, const QcVariant& value);
protected:
	void SetCurSel(s32 index);
	s32 GetCount();
protected:
	s32 m_iCurrent;
	s32 m_iCount;
	QcResEnumDescParam* m_pEnumParam;
	QcWidgetPtr m_pTipWidget;
};