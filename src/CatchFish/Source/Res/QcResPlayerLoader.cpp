#include "pch.hpp"
#include "QcResPlayerLoader.hpp"
#include "QcResPlayerDesc.hpp"
#include "QiResCallBack.hpp"
#include "QcResImageListDesc.hpp"

void QcResPlayerLoader::Parse(const QcXmlNodeIterator& topNode)
{
	QcRuntimeClass* pRunClass = SGlobalData.goObjFactory.Find(topNode.Attribute("class"));
	if (pRunClass == NULL)
		pRunClass = QmRuntimeClass(QcResPlayerDesc);

	QcStringList sElementList;
	QcXmlNodeIterator elements = topNode.FirstNode("Elements");
	if (elements)
	{
		QcXmlNodeIterator element = elements.FirstNode();
		while (element)
		{
			QcRuntimeClass* pRuntimeClass = GetRunClass(element, QmRuntimeClass(QcResImageListDesc));
			QcSharedPointer<QcResImageListDesc>  pDesc(pRuntimeClass->CreateObject<QcResImageListDesc>());
			pDesc->SetRange(1, QmCountOf(QaPlayerList));
			if (pDesc->LoadDesc(element))
			{
				if (GetCallBacker())
					GetCallBacker()->CallBack(pDesc);
				GetResMgr()->LoadRes(pDesc);
				sElementList.push_back(pDesc->GetObjType());
			}
			element = element.NextSibling();
		}
	}
}