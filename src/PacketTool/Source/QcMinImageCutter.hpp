#pragma once

class QcMinImageCutter
{
	struct QcOffsetInfo
	{
		bool operator==(const QcOffsetInfo& info)
		{
			return sName == info.sName;
		}
		QcString sName;
		QcString souceFile;
		QcString destFile;
		float X;
		float Y;
		FILETIME lastWriteTime;
	};
	struct QcInfoSortor
	{
		//小于比较符号
		bool operator ()(const QcOffsetInfo& first, const QcOffsetInfo& second) const
		{
			const QcString& left = first.sName;
			const QcString& right = second.sName;
			return left < right;
		}
		bool operator ()(const QcOffsetInfo& first, const QcString& second) const
		{
			const QcString& left = first.sName;
			const QcString& right = second;
			return left < right;
		}
		bool operator ()(const QcString& first, const QcOffsetInfo& second) const
		{
			const QcString& left = first;
			const QcString& right = second.sName;
			return left < right;
		}
		bool operator ()(const QcOffsetInfo& first, const char* psName) const
		{
			return strcmp(first.sName.c_str(), psName) < 0;
		}
		bool operator ()(const char* psName, const QcOffsetInfo& second) const
		{
			return strcmp(psName, second.sName.c_str()) < 0;
		}
	};
public:
	QcMinImageCutter(): m_iIndex(-1), m_iErrorFlag(0), m_readPngTime(0), m_writePngTime(0){}

	int DoExcute(const QcString& sSourcePath, const QcString& file, const QcStringList& fileFilter, int iAphlaRef = 0, int iiAphlaEdgeWidth = 0, bool bSamePath = true);
protected:
	static DWORD __stdcall ThreadFun(LPVOID lpParameter);
	void Excute(QcOffsetInfo& infor);
	bool Excute(const QcString& sSourceFile, const QcString& sDestFile, float& offsetX, float& offsetY);
	long ToExcuteSize() const {return m_arOffsetList.size();}
	bool FillInfoList(const QcString& sSourcePath, const QcString& destPath, bool bSamePath);
	QcOffsetInfo* FindInfo(const char* psFile)
	{
		std::vector<QcOffsetInfo>::iterator iter = std::binary_searchEx(m_arOffsetList.begin(), m_arOffsetList.end(), psFile, QcInfoSortor());
		if (iter != m_arOffsetList.end())
			return &(*iter);
		return NULL;
	}
protected:
	std::vector<QcOffsetInfo> m_arOffsetList;
	QcStringList m_arFilterList;
	int m_iAphlaRef;
	int m_iAphlaEdgeWidth;
	__int64 m_readPngTime;
	__int64 m_writePngTime;
	volatile long m_iIndex;
	long m_iErrorFlag;
};