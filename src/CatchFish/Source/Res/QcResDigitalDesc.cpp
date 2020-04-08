#include "pch.hpp"
#include "QcResDigitalDesc.hpp"
#include "Scene\QcSceneDigital.hpp"
#include "QcTexture.hpp"

ImplRuntimeClass(QcResDigitalDesc);

QcResDigitalDesc::QcResDigitalDesc()
{
	m_pRuntimeClass = QmRuntimeClass(QcDigitalNode);
}

bool QcResDigitalDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	QcResImageDesc::LoadDesc(node);

	char buf[MAX_PATH] = {0};
	for (char i=0; i<10; ++i)
	{
		sprintf_s(buf, MAX_PATH, "%s_%02d.%s", m_sPrefix.c_str(), i, m_sFileExt.c_str());
		m_arSymbolList.PushBack(QsSysSymbol('0' + i, buf));
	}

	QcXmlNodeIterator sysbolNode = node.FirstNode("SymbolList");
	if (sysbolNode)
	{
		xml_attribute<char>* pAttribute = sysbolNode.AttributeNode();
		while (pAttribute)
		{
			sprintf_s(buf, MAX_PATH, "%s.%s", pAttribute->name(), m_sFileExt.c_str());
			m_arSymbolList.PushBack(QsSysSymbol(*(pAttribute->value()), buf));

			pAttribute = pAttribute->next_attribute();
		}
	}
	return true;
}

const QcTexture* QcResDigitalDesc::GetTexture(char ch) const
{
	if (ch >= '0' && ch <='9')
		return m_arSymbolList[ch - '0'].m_pTexture;
	else if (ch >=0 && ch <=9)
	{
		return m_arSymbolList[ch].m_pTexture;
	}
	else
	{
		int nSize = m_arSymbolList.size();
		for (int i=10; i<nSize; ++i)
		{
			if (m_arSymbolList[i].m_ch == ch)
				return m_arSymbolList[i].m_pTexture;
		}
	}
	return NULL;
}

void QcResDigitalDesc::PostLoad(QcResManager* pResMgr)
{
	QmAutoRevert(s32, QmGlobalData.giForceTexInVideoMem);
	if (QmGlobalData.giForceTexInVideoMem == 0)
		QmGlobalData.giForceTexInVideoMem = IsForceInVideo();

	QcList<QsSysSymbol>::iterator iter(m_arSymbolList.begin());
	for (; iter != m_arSymbolList.end(); ++iter)
	{
		iter->m_pTexture = pResMgr->LoadTexture(iter->m_sFileName);
	}

	if ((m_fWidth < ROUNDING_ERROR_f32 || m_fHeight < ROUNDING_ERROR_f32))
	{
		m_fHeight = m_arSymbolList[0].m_pTexture->GetHeight();
		m_fWidth = m_arSymbolList[0].m_pTexture->GetWidth();
	}
}