#pragma once


class QcXmlLoader
{
public:
	QcXmlLoader() {}
	QcXmlNodeIterator LoadFromFile(const char* psFile);
	QcXmlNodeIterator LoadFromBuf(char* psBuf, bool bNeedCopyBuf = false, int nSize = 0);
	QcXmlNodeIterator GetRootNode() {return QcXmlNodeIterator(m_document.document());}
	void Save(const char* psFile);
protected:
	xml_document<char> m_document;
};