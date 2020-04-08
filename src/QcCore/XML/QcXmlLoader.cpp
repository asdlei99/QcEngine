#include "pch\pch.hpp"
#include "rapidxml_print.hpp"
#include "QcXmlLoader.hpp"
#include "FileSystem\QiFile.hpp"

QcXmlNodeIterator QcXmlLoader::LoadFromFile(const char* psFile)
{
	QcFile file;
	if (file.Open(psFile, std::ios::binary | std::ios::in))
	{
		int iLen = file.GetSize();
		char* psBuf = m_document.allocate_string(0, iLen + 1);
		file.Seek(0, std::ios::beg);
		file.Read(psBuf, iLen);
		file.Close();
		psBuf[iLen] = 0;

		return LoadFromBuf(psBuf, false);
	}
	return QcXmlNodeIterator();
}

QcXmlNodeIterator QcXmlLoader::LoadFromBuf(char* psBuf, bool bNeedCopyBuf, int nSize)
{
	if (bNeedCopyBuf)
		psBuf = m_document.allocate_string(psBuf, nSize);
	m_document.parse<parse_trim_whitespace>(const_cast<char*>(psBuf));

	return GetRootNode();
}

void QcXmlLoader::Save(const char* psFile)
{
	std::string text;    
	rapidxml::print(std::back_inserter(text), m_document, 0);
	QcFile file;
	if (file.Open(psFile, std::ios::binary | std::ios::out))
	{
		file.Write(text.c_str(), text.size());
	}
}