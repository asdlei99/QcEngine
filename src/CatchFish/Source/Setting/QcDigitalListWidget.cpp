#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcDigitalListWidget.hpp"
#include "Scene\QcDigitalNode.hpp"
#include "Res\QcResDescParam.hpp"

ImplRuntimeClass(QcDigitalListWidget);

QcDigitalListWidget::QcDigitalListWidget()
{
	m_iCurrentBit = 0;
	m_nBit = 0;
	m_pDigitalNode = new QcDigitalNode();
	memset(m_sBuf, 0, QmMaxDigital);
}

QcDigitalListWidget::~QcDigitalListWidget()
{
	m_pDigitalNode = NULL;
}

void QcDigitalListWidget::InitWidget(QcResDialogItemDesc* pWidgetDesc, QcDialog* pDialog)
{
	QcWidget::InitWidget(pWidgetDesc, pDialog);
	m_pDigitalNode->SetAlign(m_eAlign);

	QcResDescParam* pDescParam = pWidgetDesc->GetParam();
	if (pDescParam)
	{
		QcString oRes = pDescParam->GetProperty("Res");
		m_nBit = pDescParam->GetProperty("Bits", 0);
		f32 fGapWidth = pDescParam->GetProperty("Gap");
		m_pDigitalNode->SetGapWidth(fGapWidth);
		m_pDigitalNode->SetDigitalType(QcObjType(oRes));
		m_pDigitalNode->SetValue(m_sBuf, m_nBit);
		m_pDigitalNode->SetFlashTime(0.8f, 0.8f);
	}
}

void QcDigitalListWidget::SetFocus(bool bFocus)
{
	QcWidget::SetFocus(bFocus);
	if (bFocus)
		m_pDigitalNode->SetFlashBit(m_iCurrentBit);
	else
		m_pDigitalNode->SetFlashBit(-1);
}

void QcDigitalListWidget::SetActive(bool bActive)
{
	if (bActive && !IsMoreThanOneInputWidget())
	{
		SetFocus(true);
	}
}

bool QcDigitalListWidget::OnUpKey(const QsKeyInfo& info)
{
	m_sBuf[m_iCurrentBit] = (BYTE)QcMath::surround(m_sBuf[m_iCurrentBit] + 1, 10);
	Update(true);
	m_pDigitalNode->SetFlashBit(m_iCurrentBit);
	return true;
}
bool QcDigitalListWidget::OnDownKey(const QsKeyInfo& info)
{
	m_sBuf[m_iCurrentBit] = (BYTE)QcMath::surround(m_sBuf[m_iCurrentBit] - 1, 10);
	Update(true);
	m_pDigitalNode->SetFlashBit(m_iCurrentBit);
	return true;
}
bool QcDigitalListWidget::OnLeftKey(const QsKeyInfo& info)
{
	if (m_iCurrentBit == 0)
	{
		m_pDigitalNode->SetFlashBit(-1);
		return false;
	}
	--m_iCurrentBit;
	m_pDigitalNode->SetFlashBit(m_iCurrentBit);
	return true;
}
bool QcDigitalListWidget::OnRightKey(const QsKeyInfo& info)
{
	if (m_iCurrentBit == m_nBit-1)
	{
		m_pDigitalNode->SetFlashBit(-1);
		return false;
	}
	++m_iCurrentBit;
	m_pDigitalNode->SetFlashBit(m_iCurrentBit);
	return true;
}

void QcDigitalListWidget::ActiveAgain(bool bForward)
{
	m_iCurrentBit = bForward ? 0 : m_nBit-1;
	m_pDigitalNode->SetFlashBit(m_iCurrentBit);
};

void QcDigitalListWidget::Update(bool bUpdateData)
{
	if (m_cb)
		m_cb(m_sBuf, m_nBit, bUpdateData);
	m_pDigitalNode->SetValue(m_sBuf, m_nBit);
}

bool QcDigitalListWidget::Update(BYTE* pValue,u32& nBit, bool bUpdateData)
{
	if (bUpdateData)
	{
		memcpy_s(pValue, QmMaxDigital, m_sBuf, m_nBit);
		nBit = m_nBit;
	}
	else
	{
		memcpy_s(m_sBuf, QmMaxDigital, pValue, m_nBit);
		m_pDigitalNode->SetValue(m_sBuf, m_nBit);
	}
	return true;
}

void QcDigitalListWidget::RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	QcWidget::RenderSub(render, pWorldMatrix);

	m_pDigitalNode->Render(render, pWorldMatrix);
}