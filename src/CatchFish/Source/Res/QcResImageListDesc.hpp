#pragma once

#include "Res\QcResImageDesc.hpp"
class QcResImageListDesc : public QcResImageDesc
{
	DeclareRuntimeClass(QcResImageListDesc);
public:
	QcResImageListDesc();

	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual bool LoadSubDesc(const QcXmlNodeIterator& node);
	void SetRange(u32 iBegin, u32 iEnd)
	{
		m_iBegin = iBegin;
		m_iEnd = iEnd;
	}
	void GetRange(u32& iBegin, u32& iEnd)
	{
		iBegin = m_iBegin;
		iEnd = m_iEnd;
	}
protected:
	bool CheckFile(const char* psPrefix,const char* psFileExt, s32 index = -1);
protected:
	u32 m_iBegin;
	u32 m_iEnd;
};