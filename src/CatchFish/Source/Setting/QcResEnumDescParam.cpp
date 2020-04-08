#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcResEnumDescParam.hpp"

ImplRuntimeClass(QcResEnumDescParam);

QcResEnumDescParam::QcResEnumDescParam()
{

}

void QcResEnumDescParam::LoadParam(const QcXmlNodeIterator& node)
{	
	{
		const char* contentList = node.Attribute("ContentList");
		if (contentList && strlen(contentList) != 0)
		{
			char buf[MAX_PATH] = {0};
			for (u32 i=0; i<=99; ++i)
			{
				sprintf_s(buf, MAX_PATH, "%s_%02d_000_000.png", contentList, i);
				if (QmResMgr->FindImageFile(buf) == 0)
					break;

				sprintf_s(buf, MAX_PATH, "%s_%02d", contentList, i);
				QcResImageDesc* pImageDesc = QmRuntimeClass(QcResImageDesc)->CreateObject<QcResImageDesc>();
				pImageDesc->LoadDesc(buf);
				m_enumObjList.PushBack(pImageDesc);
			}
		}
		else
		{
			QcStringList content;
			QfSplitString(node.Attribute("Content"), content, ';');
			QcStringList::iterator iter(content.begin());
			QcStringList::iterator endIter(content.end());
			for (; iter != endIter; ++iter)
			{
				QcResImageDesc* pImageDesc = QmRuntimeClass(QcResImageDesc)->CreateObject<QcResImageDesc>();
				pImageDesc->LoadDesc(iter->c_str());
				m_enumObjList.PushBack(pImageDesc);
			}
		}
	}
	
	QcXmlNodeIterator tipNode = node.FirstNode("Tip");
	if (tipNode)
	{
		m_vPos = tipNode.Attribute("Pos", m_vPos);
		m_eAlign = QfStringToAlign(tipNode.Attribute("Align"));

		const char* tipList = tipNode.Attribute("ContentList");
		if (tipList && strlen(tipList) != 0)
		{
			char buf[MAX_PATH] = {0};
			for (u32 i=0; i<=99; ++i)
			{
				sprintf_s(buf, MAX_PATH, "%s_%02d_000_000.png", tipList, i);
				if (QmResMgr->FindImageFile(buf) == 0)
					break;

				sprintf_s(buf, MAX_PATH, "%s_%02d", tipList, i);
				QcResImageDesc* pImageDesc = QmRuntimeClass(QcResImageDesc)->CreateObject<QcResImageDesc>();
				pImageDesc->LoadDesc(buf);
				m_tipObjList.PushBack(pImageDesc);
			}
		}
		else
		{
			QcStringList content;
			QfSplitString(tipNode.Attribute("Content"), content, ';');
			QcStringList::iterator iter(content.begin());
			QcStringList::iterator endIter(content.end());
			for (; iter != endIter; ++iter)
			{
				QcResImageDesc* pImageDesc = QmRuntimeClass(QcResImageDesc)->CreateObject<QcResImageDesc>();
				pImageDesc->LoadDesc(iter->c_str());
				m_tipObjList.PushBack(pImageDesc);
			}
		}
	}
}