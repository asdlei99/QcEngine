#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcResDialogDesc.hpp"
#include "QcDialog.hpp"

ImplRuntimeClass2(QcResDialogDesc, 20);

QcResDialogDesc::QcResDialogDesc()
: m_fSize(0.f, 0.f)
{
	m_pDialogClass = QmRuntimeClass(QcDialog);
}

bool QcResDialogDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	m_objType = node.Attribute("name");
	m_fSize = node.Attribute("size", m_fSize);

	const char* pChar = node.Attribute("DlgClass");
	if (strlen(pChar) > 0)
	{
		QcRuntimeClass* pRuntimeClass = QmClassFactoryInstance->Find(pChar);
		QmAssert(pRuntimeClass != NULL);
		m_pDialogClass = pRuntimeClass;
	}

	m_sAction = node.Attribute("Action");

	m_oBgImage = new QcResImageDesc();
	m_oBgImage->LoadDesc(node.Attribute("BackGround"));

	const char* psTemp = node.Attribute("SelectFlag");
	if (psTemp && strlen(psTemp) > 0)
	{
		m_oSelectFlagImage = new QcResImageDesc();
		m_oSelectFlagImage->LoadDesc(psTemp);
	}
	
	psTemp = node.Attribute("SelectBg");
	if (psTemp && strlen(psTemp) > 0)
	{
		m_oSelectBgImage = new QcResImageDesc();
		m_oSelectBgImage->LoadDesc(psTemp);
	}

	LoadType(node, "Static", m_arStaticList);
	LoadType(node, "TipArea", m_arTipList);
	LoadType(node, "Items", m_arItemList);

	QcResDesc::LoadDesc(node);
	return true;
}

void QcResDialogDesc::LoadTexture(QcResManager* pResMgr)
{
	m_oBgImage = pResMgr->LoadRes2(m_oBgImage);
	if ( (QcMath::equals(m_fSize.X, 0.f) || QcMath::equals(m_fSize.Y, 0.f)) && m_oBgImage)
	{
		m_fSize.X = m_oBgImage->GetWidth();
		m_fSize.Y = m_oBgImage->GetHeight();
	}

	if (m_oSelectFlagImage)
		m_oSelectFlagImage = pResMgr->LoadRes2(m_oSelectFlagImage);
	if (m_oSelectBgImage)
		m_oSelectBgImage = pResMgr->LoadRes2(m_oSelectBgImage);

	QcResDialogItemDescListIter iter(m_arStaticList.begin());
	for (; iter != m_arStaticList.end(); ++iter)
	{
		(*iter)->LoadTexture(pResMgr);
	}
	iter = m_arTipList.begin();
	for (; iter != m_arTipList.end(); ++iter)
	{
		(*iter)->LoadTexture(pResMgr);
	}
	iter = m_arItemList.begin();
	for (; iter != m_arItemList.end(); ++iter)
	{
		(*iter)->LoadTexture(pResMgr);
	}
}

void QcResDialogDesc::LoadType(const QcXmlNodeIterator& node, const char* psType, QcResDialogItemDescList& itemList)
{
	QcXmlNodeIterator item = node.FirstNode(psType);
	if (item)
	{
		item = item.FirstNode();
		while (item)
		{
			QcResDialogItemDesc* pItemDesc = new QcResDialogItemDesc();
			pItemDesc->LoadDesc(item);

			itemList.PushBack(pItemDesc);
			item = item.NextSibling();
		}
	}
}

QcDialog* QcResDialogDesc::MakeInstance()
{
	return m_pDialogClass->CreateObject<QcDialog>();
}