#pragma once

#define QmWinFileOn
class QcFilePacker2
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
public:
	QcFilePacker2(const QcString& packetName, bool bPackerPixel);
	~QcFilePacker2();

	int PakFile(const QcStringList& fileList);
protected:
	long ToExcuteSize() const {return m_arFileList.size(); }
	void WriteData(char* pData, const QsPackInfo& info)
	{
#ifdef QmWinFileOn
		DWORD dwOut = 0;
		WriteFile(m_ifsData, pData, info.m_iLen, &dwOut, NULL);
		WriteFile(m_ifsFile, &info, sizeof(info), &dwOut, NULL);
#else
		m_ifsData.write(pData, info.m_iLen);
		m_ifsFile.write((char*)&info, sizeof(info));
#endif
	}
	char* PakFile(const QcString& file, QsPackInfo& info);
	static DWORD __stdcall ThreadFun(LPVOID lpParameter);
protected:
#ifdef QmWinFileOn
	HANDLE m_ifsData;
	HANDLE m_ifsFile;
#else
	std::fstream m_ifsData;
	std::fstream m_ifsFile;
#endif
	QcStringList m_arFileList;
	volatile long m_iIndex;
	volatile long m_iErrorFlag;
	QcCriticalSection m_cs;
	DWORD m_iCurrentPos;
	bool m_bPackerPixel;
};