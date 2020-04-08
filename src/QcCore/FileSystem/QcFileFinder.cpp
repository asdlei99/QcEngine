#include "pch\pch.hpp"
#include "QcFileFinder.hpp"
#include "QcFileSystem.hpp"

QcFileFinder::QcFileFinder(const QcString& path, bool bSubFolder) : m_bNeedUniquePath(true)
{
	AddSearchPath(path, bSubFolder);
}

bool QcFileFinder::FindFileList(const char* psFilter, QcStringList& fileList)
{
	UniquePath();
	QcStringListIter iter(m_arSearchPath.begin());
	QcStringListIter endIter(m_arSearchPath.end());
	for (; iter != endIter; ++iter)
	{
		QcFileSystem::GetFileList(*iter, &fileList, NULL, QsFileFilter(psFilter, false));
	}
	return fileList.size() != 0;
}

void QcFileFinder::AddSearchPath(const QcString& path, bool bSubFolder)
{
	QcString str = path;
	QcFileSystem::UnifyPathName(str);
	m_arSearchPath.push_back(str);
	m_bNeedUniquePath = true;

	if (bSubFolder)
	{
		QcStringList folderList;
		QcFileSystem::GetFileList(str, NULL, &folderList);
		AddSearchPath(folderList, false);
	}
}
void QcFileFinder::RemoveSearchPath(const QcString& path)
{
	QfErase(m_arSearchPath, path);
}

bool QcFileFinder::FindFile(QcString& sFile)
{
	UniquePath();
	if (QcFileSystem::IsExist(sFile.c_str()))
	{
		return true;
	}
	QcString tempFile;
	static QcString sLastFind;

	if (sLastFind.size())
	{
		tempFile = sLastFind + sFile;
		if (QcFileSystem::IsExist(tempFile))
		{
			sFile = tempFile;
			return true;
		}
	}

	QcStringListIter iter = m_arSearchPath.begin();
	QcStringListIter endIter = m_arSearchPath.end();
	for (; iter != endIter; ++iter)
	{
		tempFile = *iter + sFile;
		if (QcFileSystem::IsExist(tempFile))
		{
			sFile = tempFile;
			sLastFind = *iter;
			return true;
		}
	}
	return false;
}