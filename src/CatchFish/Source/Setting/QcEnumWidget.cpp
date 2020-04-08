#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcEnumWidget.hpp"
#include "QcResEnumDescParam.hpp"
#include "Animator\QcAnimatorFlash.hpp"

ImplRuntimeClass(QcEnumWidget);

QcEnumWidget::QcEnumWidget()
{
	m_pEnumParam = NULL;
	m_iCurrent = 0;
}

void QcEnumWidget::InitWidget(QcResDialogItemDesc* pWidgetDesc, QcDialog* pDialog)
{
	QcWidget::InitWidget(pWidgetDesc, pDialog);
	m_pEnumParam = pWidgetDesc->GetParam()->As<QcResEnumDescParam*>();
	QcList<QcResImageDescPtr>& tipList = m_pEnumParam->GetTipList();
	if (tipList.size())
	{
		m_pTipWidget = QmRuntimeClass(QcWidget)->CreateObject<QcWidget>();
		QcResDialogItemDescPtr pWidgetDesc(QmRuntimeClass(QcResDialogItemDesc)->CreateObject<QcResDialogItemDesc>());
		pWidgetDesc->SetAlign(m_pEnumParam->GetTipAlign());
		pWidgetDesc->SetAlignPos(m_pEnumParam->GetTipPos());
		m_pTipWidget->InitWidget(pWidgetDesc, pDialog);
	}
	m_iCount = (s32)m_pEnumParam->GetEnumList().size();
}

void QcEnumWidget::Animate(double dCurrentTime,f32 fElapsedTime)
{
	if (m_pTipWidget)
	{
		m_pTipWidget->Animate(dCurrentTime, fElapsedTime);
	}
	QcWidget::Animate(dCurrentTime, fElapsedTime);
}
void QcEnumWidget::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (m_pTipWidget)
		m_pTipWidget->Render(render, pWorldMatrix);
	QcWidget::Render(render, pWorldMatrix);
}

bool QcEnumWidget::OnLeftKey(const QsKeyInfo& info)
{
	m_iCurrent = QcMath::surround(m_iCurrent - 1, GetCount());
	Update(true);	
	return true;
}

bool QcEnumWidget::OnRightKey(const QsKeyInfo& info)
{
	m_iCurrent = QcMath::surround(m_iCurrent + 1, GetCount());
	Update(true);
	return true;
}

void QcEnumWidget::SetActive(bool bActive)
{
	if (bActive)
	{
		SetCurSel((s32)m_iCurrent);
	}
	else if (m_pTipWidget)
	{
		m_pTipWidget->SetVisible(false);
	}
	QcWidget::SetActive(bActive);
}

void QcEnumWidget::SetCurSel(s32 index)
{
	m_iCurrent = (u32)index;
	QcList<QcResImageDescPtr>& itemList = m_pEnumParam->GetEnumList();
	SetWidgetImage(itemList[index]);
	if (m_pTipWidget)
	{
		m_pTipWidget->SetWidgetImage(m_pEnumParam->GetTipList()[index]);
		m_pTipWidget->SetVisible(true);
	}
}

s32 QcEnumWidget::GetCount()
{
	return m_iCount;
}

void QcEnumWidget::Update(bool bUpdateData)
{
	if (m_cb)
		m_cb(m_iCurrent, bUpdateData);
	SetCurSel(m_iCurrent);
}

bool QcEnumWidget::Update(s32& iValue, bool bUpdateData)
{ 
	if (bUpdateData)
	{
		iValue = m_iCurrent;
	}
	else
	{
		m_iCurrent = iValue;
		SetCurSel(m_iCurrent);
	}
	return true;
}

void QcEnumWidget::SetValue(const char* psKey, const QcVariant& value)
{
	QcNoCaseCompare compare(psKey);
	if (compare == "Count")
	{
		m_iCount = value;
	}
	else if (compare == "Current")
	{
		m_iCurrent = value;
	}
}
