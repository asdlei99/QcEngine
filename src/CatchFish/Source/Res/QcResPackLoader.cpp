#include "Pch.hpp"
#include "QcResPackLoader.hpp"
#include "Common\QcMediaFileFinder.hpp"
#include "Common\QcEncryptFileBuf.hpp"

QcResPackLoader::QcResPackLoader()
{
#ifdef QmWinFileOn
	m_ifs = NULL;
#endif
}

QcResPackLoader::~QcResPackLoader()
{
#ifdef QmWinFileOn
	if (m_ifs)
		CloseHandle(m_ifs);
#else
	m_ifs.close();
#endif
}

bool QcResPackLoader::Open(const char* psFile)
{
	QcEncryptFileBuf descFile;
	if (descFile.Load(psFile))
	{
		QsPackInfo* pData = (QsPackInfo*)descFile.pointer();
		int nCount = descFile.size()/sizeof(QsPackInfo);
		m_arPackInfoList.resize(nCount);
		std::copy(pData, pData + nCount, m_arPackInfoList.begin());
		std::sort(m_arPackInfoList.begin(), m_arPackInfoList.end(),  QsPackInfoSorter());
		

		char dataFile[MAX_PATH] = {0};
		strcpy_s(dataFile, psFile);
		PathRemoveExtensionA(dataFile);
		strcat_s(dataFile, PackDataExtension);

		QcString fullFileName;
		if (QcMediaFileFinderInstance->FindFile(dataFile, fullFileName))
		{
#ifdef QmWinFileOn
			m_ifs = CreateFileA(fullFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
			m_ifs.open(fullFileName.c_str(), std::ios::in | std::ios::binary);
#endif
		}
		else
		{
			QmAssert(false);
		}

		return true;
	}
	return false;
}