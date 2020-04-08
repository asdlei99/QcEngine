#include "pch.hpp"
#include "QcResDesc.hpp"
#include "QcResDescParam.hpp"
#include "RuntimeSystem\QcRuntimeClass.hpp"

QcResDesc::~QcResDesc()
{

}

bool QcResDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	QcXmlNodeIterator paraNode = node.FirstNode("Parameter");
	if (paraNode)
	{
		QcRuntimeClass* pRuntimeClass = QmClassFactoryInstance->Find(paraNode.Attribute("class"));
		if (pRuntimeClass == NULL)
			pRuntimeClass = QmRuntimeClass(QcResDescParam);

		QcResDescParam* pParam = static_cast<QcResDescParam*>(pRuntimeClass->CreateObject());
		m_pParam.Set(pParam);
		pParam->LoadParam(paraNode);
	}
	return true;
}