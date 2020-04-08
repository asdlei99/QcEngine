#pragma once

#include "Res\QcResImageDesc.hpp"

class QcResDigitalDesc : public QcResImageDesc
{
	struct QsSysSymbol
	{
		QsSysSymbol(char ch, const char* psFileName, QcTexture* pTexture = NULL) : m_ch(ch), m_sFileName(psFileName), m_pTexture(pTexture)
		{}

		char m_ch;
		QcString m_sFileName;
		QcTexture* m_pTexture;
	};
	DeclareRuntimeClass(QcResDigitalDesc);
public:
	QcResDigitalDesc();

	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual void PostLoad(QcResManager* pResMgr);

	const QcTexture* GetTexture(char ch) const;
protected:
	QcList<QsSysSymbol> m_arSymbolList;
};