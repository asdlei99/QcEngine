#pragma once

#include "Common\QcEncryptXmlLoader.hpp"

class QcXmlNodeIterator;
class QcResManager;
class QiResCallBack;
class QcResLoader : public QcEncryptXmlLoader
{
public:
	explicit QcResLoader(QcResManager* pResMgr, QiResCallBack* pCallBack = NULL)
		: m_pResMgr(pResMgr), m_pCallBack(pCallBack) {}

	virtual void Parse(const QcXmlNodeIterator& rootNode, void* pParam);
	virtual bool LoadRes(const QcXmlNodeIterator& pNode){return true;};
	void SetResCallBack(QiResCallBack* pCallBack) { m_pCallBack = pCallBack;}
protected:
	QcRuntimeClass* GetRunClass(const QcXmlNodeIterator& node, QcRuntimeClass* pDefaultRunClass);
protected:
	QcResManager* GetResMgr() { return m_pResMgr; }
	QiResCallBack* GetCallBacker() {return m_pCallBack; }
private:
	QiResCallBack* m_pCallBack;
	QcResManager* m_pResMgr;
};