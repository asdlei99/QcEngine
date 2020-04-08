#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcDigitalWidget.hpp"
#include "Res\QcResDescParam.hpp"

ImplRuntimeClass(QcDigitalWidget);

QcDigitalWidget::QcDigitalWidget()
{
	m_pDigitalNode = new QcDigitalNode();
	m_iDigital = 0;
	m_iMin = 0;
	m_iMax = 0x7ffffffe;
	m_iStep = 0;
	m_iFractBit = 0;
}

QcDigitalWidget::~QcDigitalWidget()
{
	m_pDigitalNode = NULL;
}
void QcDigitalWidget::InitWidget(QcResDialogItemDesc* pWidgetDesc, QcDialog* pDialog)
{
	QcWidget::InitWidget(pWidgetDesc, pDialog);
	m_pDigitalNode->SetAlign(m_eAlign);

	QcResDescParam* pDescParam = pWidgetDesc->GetParam();
	if (pDescParam)
	{
		m_iFractBit = pDescParam->GetProperty("FractBit", 0);
		if (IsIntValue())
		{
			m_iMin = pDescParam->GetProperty("min", 0);
			m_iMax = pDescParam->GetProperty("max", 0x7ffffffe);
			m_iStep = pDescParam->GetProperty("step", 0);
			m_iDigital = 0;
			m_iDigital = QcMath::surround(m_iDigital, m_iMin, m_iMax + 1);
		}
		else
		{
			m_fMinValue = pDescParam->GetProperty("min", ROUNDING_ERROR_f64);
			m_fMaxValue = pDescParam->GetProperty("max", f64(gfMax) - ROUNDING_ERROR_f64);
			m_fStepValue = pDescParam->GetProperty("step", f64(0));
			if (m_fValue < m_fMinValue)
				m_fValue = m_fMinValue;
			if (m_fValue > m_fMaxValue)
				m_fValue = m_fMaxValue;
		}
		f32 fGapWidth = pDescParam->GetProperty("Gap");
		m_pDigitalNode->SetGapWidth(fGapWidth);
		QcString oRes = pDescParam->GetProperty("Res");
		m_pDigitalNode->SetDigitalType(oRes);
	}
	
}

void QcDigitalWidget::SetValue(const char* psKey, const QcVariant& value)
{
	QcNoCaseCompare compare(psKey);
	if (compare == "min")
	{
		if (IsIntValue())
			m_iMin = value;
		else
			m_fMinValue = value;
	}
	else if (compare == "max")
	{
		if (IsIntValue())
			m_iMax = value;
		else
			m_fMaxValue = value;
	}
	else if (compare == "step")
	{
		if (IsIntValue())
			m_iStep = value;
		else
			m_fStepValue = value;
	}
	else if (compare == "Res")
	{
		QcString oRes = value;
		m_pDigitalNode->SetDigitalType(oRes);
	}
}

bool QcDigitalWidget::OnLeftKey(const QsKeyInfo& info)
{
	if (IsIntValue())
	{
		if (m_iDigital == m_iMin)
		{
			m_iDigital = m_iMax;
		}
		else
		{
			m_iDigital = QcMath::max_(m_iMin, m_iDigital - m_iStep);
		}
		Update(true);
		if ((m_iMax - m_iMin)/m_iStep > 50)
		{
			if (info.fKeyDownTime >= 1.f)
			{
				info.fTimeCount = info.fGapTime * 0.7f;
			}
			else if (info.fKeyDownTime >= 2.f)
			{
				info.fTimeCount = info.fGapTime;
			}
		}
	}
	else
	{
		if (QcMath::equals(m_fValue, m_fMinValue))
		{
			m_fValue = m_fMaxValue;
		}
		else
		{
			m_fValue = QcMath::max_(m_fMinValue, m_fValue - m_fStepValue);
		}
		Update(true);
		if ((m_fMaxValue - m_fMinValue)/m_fStepValue > 50)
		{
			if (info.fKeyDownTime >= 1.f)
			{
				info.fTimeCount = info.fGapTime * 0.7f;
			}
			else if (info.fKeyDownTime >= 2.f)
			{
				info.fTimeCount = info.fGapTime;
			}
		}
	}
	return true;
}
bool QcDigitalWidget::OnRightKey(const QsKeyInfo& info)
{
	if (IsIntValue())
	{
		if (m_iDigital == m_iMax)
		{
			m_iDigital = m_iMin;
		}
		else
		{
			m_iDigital = QcMath::min_(m_iMax, m_iDigital + m_iStep);
		}
		Update(true);
		if ((m_iMax - m_iMin)/m_iStep > 50)
		{
			if (info.fKeyDownTime >= 1.f)
			{
				info.fTimeCount = info.fGapTime * 0.7f;
			}
			else if (info.fKeyDownTime >= 2.f)
			{
				info.fTimeCount = info.fGapTime;
			}
		}
	}
	else
	{
		if (QcMath::equals(m_fValue, m_fMaxValue))
		{
			m_fValue = m_fMinValue;
		}
		else
		{
			m_fValue = QcMath::min_(m_fMaxValue, m_fValue + m_fStepValue);
		}
		Update(true);
		if ((m_fMaxValue - m_fMinValue)/m_iStep > 50)
		{
			if (info.fKeyDownTime >= 1.f)
			{
				info.fTimeCount = info.fGapTime * 0.7f;
			}
			else if (info.fKeyDownTime >= 2.f)
			{
				info.fTimeCount = info.fGapTime;
			}
		}
	}
	return true;
}
void QcDigitalWidget::Update(bool bUpdateData)
{
	if (m_cb)
	{
		if (IsIntValue())
			m_cb(m_iDigital, bUpdateData);
		else
			QmAssert(false);
	}
	UpdateWidgetValue();
}

bool QcDigitalWidget::Update(f64& iValue, bool bUpdateData)
{
	if (bUpdateData)
	{
		iValue = m_fValue;
	}
	else
	{
		if (IsIntValue())
			m_iDigital = (int)iValue;
		else
			m_fValue = iValue;
		UpdateWidgetValue();
	}
	return true;
}

bool QcDigitalWidget::Update(s32& iValue, bool bUpdateData)
{ 
	if (bUpdateData)
	{
		iValue = m_iDigital;
	}
	else
	{
		if (IsIntValue())
			m_iDigital = iValue;
		else
			m_fValue = iValue;
		UpdateWidgetValue();
	}
	return true;
}

void QcDigitalWidget::RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	QcWidget::RenderSub(render, pWorldMatrix);

	m_pDigitalNode->Render(render, pWorldMatrix);
}

void QcDigitalWidget::UpdateWidgetValue()
{
	if (IsIntValue())
	{
		m_pDigitalNode->SetValue(m_iDigital);
	}
	else
	{
		char format[256];
		char valueBuf[256] = {0};
		sprintf_s(format, "%%.%dlf", m_iFractBit);
		sprintf_s(valueBuf, format, m_fValue);
		m_pDigitalNode->SetValue((BYTE*)valueBuf, strlen(valueBuf));
	}
}