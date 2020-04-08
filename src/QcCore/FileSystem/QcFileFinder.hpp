#pragma once

#include "Common\QcObject.hpp"

class QcFileFinder : public QcObject
{
public:
	QcFileFinder(){}
	QcFileFinder(const QcString& path, bool bSubFolder = false);

	void AddSearchPath(const QcString& path, bool bSubFolder = false);
	void AddSearchPath(const QcStringList& pathList, bool bSubFolder = false)
	{
		QcStringList::const_iterator iter(pathList.begin());
		QcStringList::const_iterator endIter(pathList.end());
		for (; iter != endIter; ++iter)
			AddSearchPath(*iter, bSubFolder);
	}
	void RemoveSearchPath(const QcString& path);
	bool FindFile(const QcString& fileName, QcString& sFullPathName)
	{
		return FindFile(fileName.c_str(), sFullPathName);
	}
	bool FindFile(const char* psFileName, QcString& sFullPathName)
	{
		if (psFileName != sFullPathName.c_str())
			sFullPathName = psFileName;
		return FindFile(sFullPathName);
	}
	bool FindFile(QcString& sFile);
	bool FindFileList(const char* psFilter, QcStringList& fileList);
protected:
	void UniquePath()
	{
		if (m_bNeedUniquePath)
		{
			QfMakeUnique(m_arSearchPath);
			m_bNeedUniquePath = false;
		}
	}
protected:
	std::vector<std::string> m_arSearchPath;
	bool m_bNeedUniquePath;
};