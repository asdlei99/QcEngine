#include "pch\pch.hpp"
#include "QcFileSystem.hpp"

 void QcFileSystem::GetFileList(const QcString& sPath, QcStringList* pFileList, QcStringList* pSubFolderPathList, const QsFileFilter& fileFilter)
{
	QcString destName(sPath);
	UnifyPathName(destName);

	WIN32_FIND_DATAA fileData = {0};
	HANDLE hFind = FindFirstFileA(QcString(destName + "*.*").c_str(), &fileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (fileData.cFileName[0] != '.' && fileData.cFileName[0] != '..')
				{
					if (PathMatchSpecA(fileData.cFileName, fileFilter.m_sfolderFilter.c_str()))
					{
						if (pSubFolderPathList)
							pSubFolderPathList->push_back(destName + fileData.cFileName +  '\\');

						if (fileFilter.m_bSubPath)
							GetFileList(destName + fileData.cFileName, pFileList, pSubFolderPathList, fileFilter);
					}
				}
			}
			else
			{
				if (pFileList && PathMatchSpecA(fileData.cFileName, fileFilter.m_sFileFilter.c_str()))
					pFileList->push_back(destName + fileData.cFileName);
			}
		}
		while(FindNextFileA(hFind, &fileData));
		FindClose(hFind);
	}
}

bool QcFileSystem::SearchUpFolder(const QcString& sBasePath, QcString& Folder)
{
	QcString sTempPath = sBasePath;
	QcFileSystem::UnifyPathName(sTempPath);

	char path[MAX_PATH] = {0};
	strcpy_s(path, sTempPath.c_str());
	PathRemoveFileSpecA(path);
	for (;;)
	{
		char buf[MAX_PATH];
		sprintf_s(buf, MAX_PATH, "%s\\%s", path, Folder.c_str());
		if (QcFileSystem::IsExist(buf))
		{
			Folder = buf;
			return true;
		}
		if (!PathRemoveFileSpecA(path))
		{
			break;
		}
	}
	return false;
}

bool QcFileSystem::SearchSubFolder(const QcString& sBasePath, QcString& Folder)
{
	QcStringList foldList;
	GetFileList(sBasePath, NULL, &foldList, QsFileFilter());

	size_t nSize = Folder.size();
	QcStringList::iterator iter(foldList.begin());
	QcStringList::iterator endIter(foldList.end());
	for (; iter != endIter; ++iter)
	{
		if (_strnicmp(PathFindFileNameA((*iter).c_str()), Folder.c_str(), nSize) == 0)
		{
			Folder = *iter;
			return true;
		}
	}
	return false;
}


void QcFileSystem::FilterFileList(const QcStringList& fileList, const QcString& sMatchStr, QcStringList* pMatchFileList, QcStringList* pNotMatchFileList)
{ 
	if (sMatchStr.size())
	{
		QcStringList sFilterList;
		QfSplitString(sMatchStr.c_str(), sFilterList, ';');

		QcStringListConstIter iter(fileList.begin());
		QcStringListConstIter endIter(fileList.end());
		for (; iter != endIter; ++iter)
		{
			if (MatchFile(*iter, sFilterList))
			{
				if (pMatchFileList)
					pMatchFileList->push_back(*iter);
			}
			else if (pNotMatchFileList)
			{
				pNotMatchFileList->push_back(*iter);
			}
		}
	}
}

bool QcFileSystem::MatchFile(const QcString& fileName, const QcString& sMatchStr)
{
	return PathMatchSpecA(PathFindFileNameA(fileName.c_str()), sMatchStr.c_str()) ? true : false;
}

bool QcFileSystem::MatchFile(const QcString& fileName, const QcStringList& sMatchList)
{
	QcStringListConstIter iter(sMatchList.begin());
	QcStringListConstIter endIter(sMatchList.end());
	for (; iter != endIter; ++iter)
	{
		if (MatchFile(fileName, *iter))
			return true;
	}
	return false;
}

void QcFileSystem::CopyFileList(const QcStringList& fileList, const QcString& sDestPath, const QcString& sBasePath)
{
	QcString destName(sDestPath);
	UnifyPathName(destName);

	int nLen = sBasePath.length();
	if (nLen)
	{
		QcStringListConstIter iter(fileList.begin());
		QcStringListConstIter endIter(fileList.end());
		for (; iter != endIter; ++iter)
		{
			if (_strnicmp(iter->c_str(), sBasePath.c_str(), nLen) == 0)
			{
				CopyFile(*iter, destName + (iter->c_str() + nLen));
			}
			else
			{
				CopyFile(*iter, destName + ::PathFindFileNameA(iter->c_str()));
			}
		}
	}
	else
	{
		QcStringListConstIter iter(fileList.begin());
		QcStringListConstIter endIter(fileList.end());
		for (; iter != endIter; ++iter)
		{
			CopyFile(*iter, destName + ::PathFindFileNameA(iter->c_str()));
		}
	}
}

void QcFileSystem::CopyFileList(const QcString& sSourcePath, const QcString& sDestPath, const QsFileFilter& pFileFilter, bool bSamePath)
{
	QcStringList fileList;
	GetFileList(sSourcePath, &fileList, NULL, pFileFilter);
	CopyFileList(fileList, sDestPath, pFileFilter.m_bSubPath && bSamePath ? sSourcePath : QcString());
}

void QcFileSystem::CopyFile(const QcString& sourceFile, const QcString& destFile)
{
	if (!::CopyFileExA(sourceFile.c_str(), destFile.c_str(), NULL, NULL, FALSE, 
		COPY_FILE_OPEN_SOURCE_FOR_WRITE | COPY_FILE_RESTARTABLE| COPY_FILE_ALLOW_DECRYPTED_DESTINATION) )
	{
		if (ERROR_PATH_NOT_FOUND == GetLastError() && IsExist(sourceFile.c_str()))
		{
			char* lpszName = ::PathFindFileNameA(destFile.c_str());
			QcString destPath(destFile.c_str(), lpszName - destFile.c_str());
			QcString destName(lpszName);
			//SplitPath(destFile, destPath, destName);
			MakePath(destPath);
			if (::CopyFileExA(sourceFile.c_str(), destFile.c_str(), NULL, NULL, FALSE, 
				COPY_FILE_OPEN_SOURCE_FOR_WRITE | COPY_FILE_RESTARTABLE| COPY_FILE_ALLOW_DECRYPTED_DESTINATION))
				return;
		}
#ifndef QmLogOn
		LPVOID lpMsgBuf;
		FormatMessageA( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPSTR) &lpMsgBuf,
			0,
			NULL 
			);
		printf("¿½±´ÎÄ¼þÊ§°Ü%s:\n", sourceFile.c_str());
		printf("%s:\n", (LPCTSTR)lpMsgBuf);
		LocalFree( lpMsgBuf );
#else
		QmLogSysError();
#endif
	}
}

void QcFileSystem::DeleteFileList(const QcStringList& fileList)
{
	QcStringListConstIter iter(fileList.begin());
	QcStringListConstIter endIter(fileList.end());
	for (; iter != endIter; ++iter)
	{
		DeleteFile(*iter);
	}
}

void QcFileSystem::DeleteDir(const QcString& sDelPath)
{
	QcStringList fileList;
	QcStringList folderList;
	GetFileList(sDelPath, &fileList, &folderList);
	DeleteFileList(fileList);

	QcStringListConstIter iter(folderList.begin());
	for (; iter != folderList.end(); ++iter)
		RemoveDirectoryA(iter->c_str());
	RemoveDirectoryA(sDelPath.c_str());
}

void QcFileSystem::DeleteFile(const QcString& sPath, const QsFileFilter& fileFilter)
{
	QcStringList fileList;
	GetFileList(sPath, &fileList, NULL, fileFilter);
	DeleteFileList(fileList);
}

void QcFileSystem::DeleteFile(const QcString& sFile)
{ 
	if (IsExist(sFile.c_str()))
	{
		SetFileAttributesA(sFile.c_str(), ~FILE_ATTRIBUTE_READONLY & GetFileAttributesA(sFile.c_str()));
		if (!::DeleteFileA(sFile.c_str()))
		{
#ifndef QmLogSysError
			LPVOID lpMsgBuf;
			FormatMessageA( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				(LPSTR) &lpMsgBuf,
				0,
				NULL 
				);
			QmLog("É¾³ýÎÄ¼þÊ§°Ü:\n");
			QmLog("%s:\n", (LPCTSTR)lpMsgBuf);
			LocalFree( lpMsgBuf );
#else
			QmLogSysError();
#endif
		}
	}
}

void QcFileSystem::RemoveFile(QcStringList& pathFileList, const QcStringList& removeFile)
{
	QcStringList::const_iterator iter(removeFile.begin());
	QcStringList::const_iterator endIter(removeFile.end());
	for (; iter != endIter; ++iter)
	{
		RemoveFile(pathFileList, *iter);
	}
}

void QcFileSystem::RemoveFile(QcStringList& pathFileList, const QcString& file)
{
	const char* psName = PathFindFileNameA(file.c_str());
	QcStringList::iterator iter(pathFileList.begin());
	QcStringList::iterator endIter(pathFileList.end());
	for (; iter != endIter; ++iter)
	{
		const char* pBegin = iter->c_str();
		char* lpszName = ::PathFindFileNameA(pBegin);
		if (_stricmp(psName, lpszName) == 0)
		{
			pathFileList.erase(iter);
			break;
		}
	}
}
