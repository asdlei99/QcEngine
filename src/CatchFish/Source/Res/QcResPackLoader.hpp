#pragma once

class QcResPackLoader : public QcObject
{
	struct QsPackInfo
	{
		char m_fileName[64];
		UINT32 m_iPos;
		UINT32 m_iLen;
		UINT32 m_width;
		UINT32 m_height;
		int m_iEncryFlag;
	};
	struct QsPackInfoSorter
	{
		bool operator ()(const QsPackInfo& left, const QsPackInfo& right) const
		{
			return QfStrNoCaseCmp(left.m_fileName, right.m_fileName) < 0;
		}
		bool operator ()(const QsPackInfo& left, const char* right) const
		{
			return QfStrNoCaseCmp(left.m_fileName, right) < 0;
		}
		bool operator()(const char* left, const QsPackInfo& right) const
		{
			return QfStrNoCaseCmp(left, right.m_fileName) < 0;
		}
	};
public:
	QcResPackLoader();
	~QcResPackLoader();

	bool Open(const char* psFile);
	QcPackFileHandle FindFile(const char *pFileName)
	{
		std::vector<QsPackInfo>::iterator iter = binary_searchEx(m_arPackInfoList.begin(), m_arPackInfoList.end(), pFileName, QsPackInfoSorter());
		if (iter != m_arPackInfoList.end())
			return iter - m_arPackInfoList.begin();
		return QmInvaidHandle;
	}
	void GetPackInfo(QcPackFileHandle hHandle, u32& iPos, u32& iLen, int& iEncry, u32* pWidth = NULL, u32* pHeight = NULL)
	{
		if (hHandle != QmInvaidHandle)
		{
			QsPackInfo& info = m_arPackInfoList[hHandle];
			iPos = info.m_iPos;
			iLen = info.m_iLen;
			iEncry = info.m_iEncryFlag;

			if (pWidth)
				*pWidth = info.m_width;
			if (pHeight)
				*pHeight = info.m_height;
		}
	}
	char* ReadData(u32 iPos, u32 iLen, char* data)
	{
#ifdef QmWinFileOn
		DWORD dwOut;
		SetFilePointer(m_ifs, iPos, 0, FILE_BEGIN);
		ReadFile(m_ifs, data, iLen, &dwOut, 0);
#else
		m_ifs.seekg(iPos, ios::beg);
		m_ifs.read(data, iLen);
#endif
		return data;
	}
protected:
#ifdef QmWinFileOn
	HANDLE m_ifs;
#else
	std::ifstream m_ifs;
#endif
	std::vector<QsPackInfo> m_arPackInfoList;
};