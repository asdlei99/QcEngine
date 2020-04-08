#pragma once

class QcXmlNodeIterator;
class QcResDescParam : public QcObject
{
	DeclareRuntimeClass(QcResDescParam);
public:
	QcResDescParam()
	{
		m_pVariantMap = new QcVariantMap();
	}
	
	virtual void LoadParam(const QcXmlNodeIterator& node);
};