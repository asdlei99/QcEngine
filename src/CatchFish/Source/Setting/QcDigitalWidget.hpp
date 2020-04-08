#pragma once

#include "Setting\QcWidget.hpp"
class QcDigitalWidget : public QcWidget
{
	typedef boost::function<void(s32&, bool bUpdate)> QcFunDigitalCb;
	DeclareRuntimeClass(QcDigitalWidget);
public:
	QcDigitalWidget();
	~QcDigitalWidget();

	virtual void InitWidget(QcResDialogItemDesc* pWidgetDesc, QcDialog* pDialog);
	virtual bool OnLeftKey(const QsKeyInfo& info);
	virtual bool OnRightKey(const QsKeyInfo& info);
	virtual void Update(bool bUpdateData);
	virtual bool Update(s32& iValue, bool bUpdateData);
	virtual bool Update(f64& iValue, bool bUpdateData);
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix);
	virtual void SetValue(const char* psKey, const QcVariant& value);
protected:
	void UpdateWidgetValue();
	bool IsIntValue() const {return m_iFractBit == 0;}
	QcSharedPtr<QcDigitalNode> m_pDigitalNode;
	s32 m_iFractBit;
	union
	{
		s32 m_iDigital;
		f64 m_fValue;
	};
	union
	{
		s32 m_iMin;
		f64 m_fMinValue;
	};
	union
	{
		s32 m_iMax;
		f64 m_fMaxValue;
	};
	union
	{
		s32 m_iStep;
		f64 m_fStepValue;
	};
};