#pragma once

typedef int QcPackFileHandle;
#define QmInvaidHandle QcPackFileHandle(-1)

#include "Common\QcObject.hpp"

class QcFilePacker : public QcObject
{
	struct QsPackInfo
	{
		QcString m_fileName;
		UINT32 m_iPos;
		UINT32 m_iLen;
	};
	struct QsPackInfoSorter
	{
		bool operator ()(const QsPackInfo& left, const QsPackInfo& right) const
		{
			return QfStrNoCaseCmp(left.m_fileName.c_str(), right.m_fileName.c_str()) < 0;
		}
		bool operator ()(const QsPackInfo& left, const char* right) const
		{
			return QfStrNoCaseCmp(left.m_fileName.c_str(), right) < 0;
		}
		bool operator()(const char* left, const QsPackInfo& right) const
		{
			return QfStrNoCaseCmp(left, right.m_fileName.c_str()) < 0;
		}
	};
public:
	QcFilePacker(const QcString& packetName, QiCrypt* pCrytor = NULL);
	~QcFilePacker();

	void PackFile(const QcStringList& fileList, QiCrypt* pCrytor = NULL)
	{
		QcStringList::const_iterator iter(fileList.begin());
		QcStringList::const_iterator endIter(fileList.end());
		for (; iter != endIter; ++iter)
		{
			PackFile(*iter, pCrytor);
		}
	}
	void PackFile(const QcString& fileName, QiCrypt* pCrytor = NULL)
	{
		if (QcFileSystem::IsExist(fileName))
		{
			QcFileBuf fileBuf;
			if (fileBuf.readFile(fileName.c_str()))
			{
				PackData2(PathFindFileNameA(fileName.c_str()), fileBuf.pointer(), fileBuf.size(), pCrytor);
			}
		}
	}
	void PackData(const char* psName, const char* data, u32 nDataLen, QiCrypt* pCrytor = NULL);
	void PackData2(const char* psName, char* data, u32 nDataLen, QiCrypt* pCrytor = NULL);

	QcPackFileHandle FirstFile() const
	{		
		return GetFileCount() ? 0 : QmInvaidHandle;
	}
	QcPackFileHandle NextFile(QcPackFileHandle preFile) const
	{
		if (preFile != QmInvaidHandle)
		{
			++preFile;
			if (preFile < GetFileCount())
				return preFile;
		}
		return QmInvaidHandle;
	}
	QcPackFileHandle FindFile(const char *pFileName) const
	{
		std::vector<QsPackInfo>::const_iterator iter = binary_searchEx(m_arPackInfoList.begin(), m_arPackInfoList.end(), pFileName, QsPackInfoSorter());
		if (iter != m_arPackInfoList.end())
			return iter - m_arPackInfoList.begin();

		if (!m_arAppendPackInfoList.empty())
		{
			iter = m_arAppendPackInfoList.begin();
			std::vector<QsPackInfo>::const_iterator endIter(m_arAppendPackInfoList.end());
			for (; iter != endIter; ++iter)
			{
				if (QfStrNoCaseCmp(pFileName, iter->m_fileName.c_str()) == 0)
					return m_arPackInfoList.size() + iter - m_arAppendPackInfoList.begin();
			}
		}
		return QmInvaidHandle;
	}
	const char* GetPackInfo(QcPackFileHandle hHandle, u32& iPos, u32& iLen)
	{
		if (hHandle != QmInvaidHandle && hHandle < GetFileCount())
		{
			const QsPackInfo& info = GetInfo(hHandle);
			iPos = info.m_iPos;
			iLen = info.m_iLen;
			return info.m_fileName.c_str();
		}
		return "";
	}
	char* ReadData(u32 iPos, u32 iLen, char* data, QiCrypt* pCrytor = NULL);
protected:
	QsPackInfo& GetInfo(QcPackFileHandle hHandle)
	{
		return (hHandle < (int)m_arPackInfoList.size()) ? m_arPackInfoList[hHandle] : m_arAppendPackInfoList[hHandle - m_arPackInfoList.size()];
	}
	int GetFileCount() const {return m_arPackInfoList.size() + m_arAppendPackInfoList.size(); }
	void DecryptData(char* pData, u32 nDataSize, QiCrypt* pCrytor = NULL);
	void EncryptData(char* pData, u32 nDataSize, QiCrypt* pCrytor = NULL);
	void WriteData(u32 iPos, char* data, u32 iDataLen, QiCrypt* pCrytor = NULL);
protected:
	QcFile m_fFile;
	QiCrypt* m_pDefaultCrytor;
	std::vector<QsPackInfo> m_arPackInfoList;
	std::vector<QsPackInfo> m_arAppendPackInfoList;
	u32 m_iCurrentPos;
	bool m_bDirty;
};