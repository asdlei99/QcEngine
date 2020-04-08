#include "pch.hpp"
#include "QcResLoader.hpp"
#include "QiResCallBack.hpp"
#include "Res\QcResDesc.hpp"
#include "Res\QcResManager.hpp"

void QcResLoader::Parse(const QcXmlNodeIterator& rootNode, void* pParam)
{
	 QcXmlNodeIterator topNode = rootNode.FirstNode();
	 if (!topNode)
		 return;

	QcXmlNodeIterator child = topNode.FirstNode();
	QcRuntimeClass* pBaseRunClass = QmClassFactoryInstance->Find(topNode.Attribute("class"));
	while(child) 
	{
		QcRuntimeClass* pRunClass = GetRunClass(child, pBaseRunClass);
		if (pRunClass)
		{
			QcSharedPtr<QcResDesc> pDesc(pRunClass->CreateObject<QcResDesc>());
			if (pDesc->LoadDesc(child))
			{
				if (GetCallBacker())
					GetCallBacker()->CallBack(pDesc);
				if (GetResMgr()->LoadRes(pDesc) != pDesc)
				{
					QmOutputString("%s\n", pDesc->GetObjType().c_str());
				}
			}
		}
		else
		{
			LoadRes(child);
		}
		child = child.NextSibling();
	};
}

QcRuntimeClass* QcResLoader::GetRunClass(const QcXmlNodeIterator& node, QcRuntimeClass* pDefaultRunClass)
{
	const char* psChildClass = node.Attribute("class");
	if (strlen(psChildClass) > 0)
		pDefaultRunClass = QmClassFactoryInstance->Find(psChildClass);

	const char* psListClass = node.Attribute("ListClass");
	if (strlen(psListClass) > 0)
		pDefaultRunClass = QmClassFactoryInstance->Find(psListClass);

	return pDefaultRunClass;
}