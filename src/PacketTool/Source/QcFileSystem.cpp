#include "pch.hpp"
#include "QcFileSystem.hpp"


class FileListGetter : public QcBaseFileAction
{
public:
	explicit FileListGetter(QcStringList& fileList)
		: m_sFileList(fileList)
	{}

	virtual bool DoFile(const QcString& fileName)
	{
		m_sFileList.push_back(m_sCurrentPath + fileName);
		return true;
	}
private:
	QcStringList& m_sFileList;
};

class FolderListGetter : public QcBaseFileAction
{
public:
	explicit FolderListGetter(QcStringList& folderList)
		: m_arFolderList(folderList)
	{}

	virtual bool DoFolder(const QcString& folderName)
	{
		m_arFolderList.push_back(m_sCurrentPath + folderName + "\\");
		return true;
	};
	virtual bool DoFile(const QcString& fileName) {return true;}
private:
	QcStringList& m_arFolderList;
};

class FileDeleter : public QcBaseFileAction
{
public:
	FileDeleter(){}
	virtual bool DoFile(const QcString& fileName)
	{
		BOOL bSuccee = ::DeleteFileA(QcString(m_sCurrentPath + fileName).c_str());
		if (!bSuccee)
		{
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
			printf("删除文件失败:\n");
			printf("%s:\n", (LPCTSTR)lpMsgBuf);
			LocalFree( lpMsgBuf );
		}
		return true;
	}
	virtual void LeaveFolder(const QcString& sPath)
	{
		::RemoveDirectoryA(sPath.c_str());
		QcBaseFileAction::LeaveFolder(sPath);
	}
};


void QcFileSystem::ForEachFile(const QcString& sPath, QcBaseFileAction& action, const QsFileFilter& fileFilter)
{
	QcString destName(sPath);
	UnifyPathName(destName);

	WIN32_FIND_DATAA fileData = {0};
	HANDLE hFind = FindFirstFileA(QcString(destName + "\\*.*").c_str(), &fileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		action.m_sCurrentPath = destName;
		if (action.EnterFolder(destName))
		{
			do
			{
				if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (fileData.cFileName[0] != '.' && fileData.cFileName[0] != '..')
					{
						if (PathMatchSpecA(fileData.cFileName, fileFilter.m_sfolderFilter.c_str()) && action.DoFolder(fileData.cFileName) && fileFilter.m_bSubPath)
							ForEachFile(destName + fileData.cFileName, action, fileFilter);
						action.m_sCurrentPath = destName;
					}
				}
				else
				{
					if (PathMatchSpecA(fileData.cFileName, fileFilter.m_sFileFilter.c_str()))
						action.DoFile(fileData.cFileName);
				}
			}
			while(FindNextFileA(hFind, &fileData));
			FindClose(hFind);
			action.m_sCurrentPath = destName;
			action.LeaveFolder(destName);
		}
	}
}

int QcFileSystem::GetFileList(const QcString& sPath, QcStringList& fileList, const QsFileFilter& fileFilter)
{
	FileListGetter fileGetter(fileList);
	ForEachFile(sPath, fileGetter, fileFilter);
	return fileList.size();
}

int QcFileSystem::GetSubFolderList(const QcString& sPath, QcStringList& subFolderList, const QsFileFilter& fileFilter)
{
	FolderListGetter folderGetter(subFolderList);
	ForEachFile(sPath, folderGetter, fileFilter);
	
	QcStringList::iterator iter(std::find(subFolderList.begin(), subFolderList.end(), sPath));
	if (iter != subFolderList.end())
		subFolderList.erase(iter);
	return subFolderList.size();
}

bool QcFileSystem::SearchFolder(const QcString& sBasePath, QcString& Folder, bool bUp)
{
	QcString sTempPath = sBasePath;
	//向上搜索
	if (bUp)
	{
		QcFileSystem::UnifyPathName(sTempPath);

		char path[MAX_PATH] = {0};
		strcpy_s(path, sTempPath.c_str());
		PathRemoveFileSpecA(path);
		for (;;)
		{
			char buf[MAX_PATH] = {0};
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
	}
	else
	{
		QcStringList foldList;
		GetSubFolderList(sTempPath, foldList, QsFileFilter());

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
	}
	return false;
}


int QcFileSystem::FilterFileList(const QcStringList& fileList, const QcString& sMatchStr, QcStringList* pMatchFileList, QcStringList* pNotMatchFileList)
{ 
	if (sMatchStr.size())
	{
		QcStringList sFilterList;
		SplitString(sMatchStr.c_str(), sFilterList, ';');

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
	return 0;
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
	GetFileList(sSourcePath, fileList, pFileFilter);
	CopyFileList(fileList, sDestPath, pFileFilter.m_bSubPath && bSamePath ? sSourcePath : QcString());
}

void QcFileSystem::CopyFile(const QcString& sourceFile, const QcString& destFile)
{
TryAgain:
	if (!::CopyFileExA(sourceFile.c_str(), destFile.c_str(), NULL, NULL, FALSE, 
		COPY_FILE_OPEN_SOURCE_FOR_WRITE | COPY_FILE_RESTARTABLE| COPY_FILE_ALLOW_DECRYPTED_DESTINATION) )
	{
		if (ERROR_PATH_NOT_FOUND == GetLastError() && IsExist(sourceFile.c_str()))
		{
			QcString destPath;
			QcString destName;
			SplitPath(destFile, destPath, destName);
			MakePath(destPath);
			goto TryAgain;
		}
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
		printf("拷贝文件失败%s:\n", sourceFile.c_str());
		printf("%s:\n", (LPCTSTR)lpMsgBuf);
		LocalFree( lpMsgBuf );
		printf("任意键继续:\n");
		getchar();
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
	FileDeleter fileDeleter;
	ForEachFile(sDelPath, fileDeleter);
}

void QcFileSystem::DeleteFile(const QcString& sPath, const QsFileFilter& fileFilter)
{
	QcStringList fileList;
	GetFileList(sPath, fileList, fileFilter);
	DeleteFileList(fileList);
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


void QcFileSystem::DeleteFile(const QcString& sFile)
{ 
	if (IsExist(sFile.c_str()))
	{
		SetFileAttributesA(sFile.c_str(), ~FILE_ATTRIBUTE_READONLY & GetFileAttributesA(sFile.c_str()));
		if (unlink(sFile.c_str()) == -1)
		{
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
			printf("删除文件失败:\n");
			printf("%s:\n", (LPCTSTR)lpMsgBuf);
			LocalFree( lpMsgBuf );
		}
	}
}

void QcFileSystem::UnifyPathName(QcString& str, bool RemoveBackslash)
{
	if (!str.empty())
	{
		char last = str.at(str.length() - 1);
		if (last != '\\' && last != '/')
		{
			if (!RemoveBackslash)
				str += "\\";
		}
		else if (RemoveBackslash)
		{
			str.erase(str.length() - 1);
		}
	}
}


void QcFileSystem::MakePath(const QcString& sPath)
{
	if (!PathFileExistsA(sPath.c_str()))
	{
		SHCreateDirectoryExA(NULL, sPath.c_str(), NULL);
		// 		char buf[2048] = {0};
		// 		sprintf_s(buf, 2048, "cmd /c mkdir \"%s\"", sPath.c_str());
		// 		WinExec(buf, SW_HIDE);
	}
}

void QcFileSystem::SplitPath(const QcString& filePath, QcString& path, QcString& file)
{
	const char* pBegin = filePath.c_str();
	char* lpszName = ::PathFindFileNameA(pBegin);
	if (pBegin == lpszName)
	{
		path = filePath;
	}
	else
	{
		path.assign(pBegin, lpszName - pBegin);
		file.assign(lpszName);
	}
}