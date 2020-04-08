#include "pch.hpp"
#include "QcResImageListDesc.hpp"

ImplRuntimeClass(QcResImageListDesc);

QcResImageListDesc::QcResImageListDesc()
{
	m_iBegin = 0;
	m_iEnd = 0;
}

bool QcResImageListDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	QcResImageDesc::LoadDesc(node);

	u32 iBegin = node.Attribute("Begin", 0);
	u32 iEnd = node.Attribute("End", 0);
	if (iBegin != 0)
		m_iBegin = iBegin;
	if (iEnd != 0)
		m_iEnd = iEnd;

	bool bChecked = false;
	QcString sBackupPrefix;
	char buf[MAX_PATH] = {0};
	if (m_iEnd == 0)
	{
		m_iEnd = m_iBegin;
		bChecked = true;
		for (u32 i=m_iBegin; i<100; ++i)
		{
			if (!CheckFile(m_sPrefix.c_str(), m_sFileExt.c_str(), (s32)i))
				break;
			++m_iEnd;
		}
		if (m_iEnd == 0)
		{
			if (CheckFile(m_sPrefix.c_str(), m_sFileExt.c_str(), -1))
			{
				sBackupPrefix = m_sPrefix;
				++m_iEnd;
			}
		}
		QmAssert(m_iEnd > m_iBegin);
	}
	
	const char* pChar = node.Attribute("class");
	QcRuntimeClass* pRuntimeClass = QmRuntimeClass(QcResImageDesc);
	if (strlen(pChar) >= 1)
	{
		pRuntimeClass = QmClassFactoryInstance->Find(pChar);
	}
	for (u32 i=m_iBegin; i<=m_iEnd; ++i)
	{
		QcResImageDesc* pSubDesc = pRuntimeClass->CreateObject()->As<QcResImageDesc*>();
		if (pSubDesc->Is<QcResImageListDesc*>())
			QmLogError("QcResImageListDesc");

		sprintf_s(buf, MAX_PATH, "%s_%02d", m_objType.c_str(), i);
		pSubDesc->SetObjType(buf);

		sprintf_s(buf, MAX_PATH, "%s_%02d", m_sPrefix.c_str(), i);
		pSubDesc->SetPrefix(buf);
		if (!bChecked)
		{
			if (CheckFile(buf, m_sFileExt.c_str(), -1))
			{
				if (sBackupPrefix.empty())
					sBackupPrefix = buf;
			}
			else
			{
				if (sBackupPrefix.empty())
				{
					if (CheckFile(m_sPrefix.c_str(), m_sFileExt.c_str(), -1))
						sBackupPrefix = m_sPrefix;
					else
						assert(false);
				}
				pSubDesc->SetPrefix(sBackupPrefix.c_str());
			}
		}
		pSubDesc->LoadDesc(node);
		m_arSubList.PushBack(pSubDesc);
	}
	return true;
}

bool QcResImageListDesc::CheckFile(const char* psPrefix, const char* psFileExt, s32 index)
{
	char buf[MAX_PATH];
	if (index != -1)
		sprintf_s(buf, MAX_PATH, "%s_%02d_000_000.%s", psPrefix, index, psFileExt);
	else
		sprintf_s(buf, MAX_PATH, "%s_000_000.%s", psPrefix, psFileExt);

	return QmResMgr->FindImageFile(buf) != 0;
}

bool QcResImageListDesc::LoadSubDesc(const QcXmlNodeIterator& node)
{
	return true;
}