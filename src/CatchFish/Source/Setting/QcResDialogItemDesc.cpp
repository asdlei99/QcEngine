#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcResDialogItemDesc.hpp"
#include "QcWidget.hpp"
#include "QcButtonWidget.hpp"
#include "QcDigitalListWidget.hpp"
#include "QcEnumWidget.hpp"
#include "QcDigitalWidget.hpp"
#include "QcResEnumDescParam.hpp"

ImplRuntimeClass2(QcResDialogItemDesc, 300);

QcResDialogItemDesc::QcResDialogItemDesc()
{
	m_eAlign = eAlignCenter;
	m_bAcceptInput = false;
	m_nID = 0;
	m_fUpdateTime = 0.f;
	m_pWidgetClass = NULL;
}	

bool QcResDialogItemDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	m_nID = node.Attribute("ID", 0);
	const char* psImage = node.Attribute("name");
	if (psImage && strlen(psImage) > 0)
	{
		m_pImage = new QcResImageDesc();
		m_pImage->LoadDesc(psImage);
	}
	m_pWidgetClass = TypeToRuntimeClass(node.Attribute("type"));

	const char* psTip = node.Attribute("Tip");
	if (psTip && *psTip != 0)
	{
		m_pTip = QmRuntimeClass(QcResImageDesc)->CreateObject<QcResImageDesc>();
		m_pTip->LoadDesc(psTip);
	}
	m_sAction = node.Attribute("Action");
	m_fUpdateTime = node.Attribute("UpdateTime", 0.f);
	m_vPos = node.Attribute("Pos", m_vPos);
	m_eAlign = QfStringToAlign(node.Attribute("Align"));
	m_size = node.Attribute("size", m_size);
	int iFlag = -1;
	iFlag = node.Attribute("AcceptInput", -1);
	if (iFlag == -1)
	{
		iFlag = (m_pWidgetClass == QmRuntimeClass(QcWidget)) ? 0 : 1;
	}
	m_bAcceptInput = iFlag != 0;

	LoadParam(node, m_pWidgetClass);
	LoadSubItem(node);
	return true;
}

bool QcResDialogItemDesc::LoadParam(const QcXmlNodeIterator& node, QcRuntimeClass* pRunClass)
{
	if (pRunClass == QmRuntimeClass(QcButtonWidget))
	{
	}
	else if (pRunClass == QmRuntimeClass(QcEnumWidget))
	{
		m_pParam = QmRuntimeClass(QcResEnumDescParam)->CreateObject<QcResEnumDescParam>();
		m_pParam->LoadParam(node);
	}
	else if (pRunClass == QmRuntimeClass(QcDigitalWidget))
	{
		m_pParam = QmRuntimeClass(QcResDescParam)->CreateObject<QcResDescParam>();

		int iFractBit = node.Attribute("FractBit", 0);
		m_pParam->SetProperty("FractBit", iFractBit);
		if (iFractBit == 0)
		{
			int iMin = node.Attribute("min", 0);
			int iMax = node.Attribute("max", 0);
			int iGap = node.Attribute("step", 0);

			m_pParam->SetProperty("min", iMin);
			m_pParam->SetProperty("max", iMax);
			m_pParam->SetProperty("step", iGap);
		}
		else
		{
			f64 iMin = node.Attribute("min", ROUNDING_ERROR_f64);
			f64 iMax = node.Attribute("max", f64(gfMax) - ROUNDING_ERROR_f64);
			f64 iGap = node.Attribute("step", 0.);

			m_pParam->SetProperty("min", iMin);
			m_pParam->SetProperty("max", iMax);
			m_pParam->SetProperty("step", iGap);
		}
		f32 fGapWidth = node.Attribute("Gap", 0.f);
		QcString objType = node.Attribute("Res");
		m_pParam->SetProperty("Gap", fGapWidth);
		m_pParam->SetProperty("Res", objType);
		
	}
	else if (pRunClass == QmRuntimeClass(QcDigitalListWidget))
	{
		m_pParam = QmRuntimeClass(QcResDescParam)->CreateObject<QcResDescParam>();
		int iBits = node.Attribute("Bits", 0);
		f32 fGapWidth = node.Attribute("Gap", 0.f);
		QcString objType = node.Attribute("Res");

		m_pParam->SetProperty("Gap", fGapWidth);
		m_pParam->SetProperty("Bits", iBits);
		m_pParam->SetProperty("Res", objType);
	}

	return true;
}

bool QcResDialogItemDesc::LoadSubItem(const QcXmlNodeIterator& node)
{
	QcXmlNodeIterator subItem = node.FirstNode("Item");
	while (subItem)
	{
		QcResDialogItemDesc* pSubDesc = QmRuntimeClass(QcResDialogItemDesc)->CreateObject<QcResDialogItemDesc>();
		pSubDesc->LoadDesc(subItem);
		m_arWidgetItemDesc.PushBack(pSubDesc);

		subItem = subItem.NextSibling("Item");
	}
	return true;
}

void QcResDialogItemDesc::LoadTexture(QcResManager* pResMgr)
{
	if (m_pTip)
		m_pTip = pResMgr->LoadRes2(m_pTip);
	if (m_pImage)
		m_pImage = pResMgr->LoadRes2(m_pImage);

	if (m_pImage && (QcMath::equals(m_size.X, 0.f) || QcMath::equals(m_size.Y, 0.f)))
	{
		m_size.X = m_pImage->GetWidth();
		m_size.Y = m_pImage->GetHeight();
	}

	if (GetWidgetClass() == QmRuntimeClass(QcEnumWidget))
	{
		QcResEnumDescParam* pParam = GetParam()->As<QcResEnumDescParam*>();

		QcList<QcResImageDescPtr>& enumList = pParam->GetEnumList();
		QcList<QcResImageDescPtr>::iterator iter(enumList.begin());
		QcList<QcResImageDescPtr>::iterator endIter(enumList.end());
		for (; iter != endIter; ++iter)
		{
			(*iter) = pResMgr->LoadRes2(*iter);
		}

		QcList<QcResImageDescPtr>& tipList = pParam->GetTipList();
		iter = tipList.begin();
		endIter = tipList.end();
		for (; iter != endIter; ++iter)
		{
			*iter = pResMgr->LoadRes2(*iter);
		}
	}

	QcResDialogItemDescListIter iter(m_arWidgetItemDesc.begin());
	QcResDialogItemDescListIter endIter(m_arWidgetItemDesc.end());
	for (; iter != endIter; ++iter)
	{
		if (*iter)
		{
			(*iter)->LoadTexture(pResMgr);
		}
	}
}

QcRuntimeClass* QcResDialogItemDesc::TypeToRuntimeClass(const char* psName)
{
	if (QfStrNoCaseCmp(psName, "Button") == 0)
		return QmRuntimeClass(QcButtonWidget);
	else if (QfStrNoCaseCmp(psName, "Enum") == 0)
		return QmRuntimeClass(QcEnumWidget);
	else if (QfStrNoCaseCmp(psName, "Digital") == 0)
		return QmRuntimeClass(QcDigitalWidget);
	else if (QfStrNoCaseCmp(psName, "DigitalList") == 0)
		return QmRuntimeClass(QcDigitalListWidget);
	return QmRuntimeClass(QcWidget);
}
