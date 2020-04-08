#pragma once

#include "Res\QcResLoader.hpp"
class QcResPlayerLoader : public QcResLoader
{
public:
	explicit QcResPlayerLoader(QcResManager* pResMgr, QiResCallBack* pCallBack = NULL)
		: QcResLoader(pResMgr, pCallBack) {}

	virtual void Parse(const QcXmlNodeIterator& topNode);
};