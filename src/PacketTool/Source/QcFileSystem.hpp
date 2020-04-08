#pragma once


struct QsFileFilter
{
public:
	explicit QsFileFilter(const char* psFilter = "*.*", bool bSubPath = true, const char* psFolderFilter="*")
		: m_sFileFilter(psFilter)
		, m_sfolderFilter(psFolderFilter)
		, m_bSubPath(bSubPath)
	{}

	QcString m_sfolderFilter;
	QcString m_sFileFilter;
	bool m_bSubPath;
};

class QcBaseFileAction
{
	friend class QcFileSystem;
public:
	virtual bool EnterFolder(const QcString& folderName){return true;};
	virtual bool DoFolder(const QcString& folderName) {return true;}
	virtual bool DoFile(const QcString& fileName) = 0;
	virtual void LeaveFolder(const QcString& folderName){};
protected:
	QcString m_sCurrentPath;
};

class QcFileSystem
{
public:
	static void ForEachFile(const QcString& sPath, QcBaseFileAction& action, const QsFileFilter& fileFilter = QsFileFilter());

	static int GetFileList(const QcString& sPath, QcStringList& fileList, const QsFileFilter& fileFilter = QsFileFilter());
	static int GetSubFolderList(const QcString& sPath, QcStringList& subFolderList, const QsFileFilter& foldFilter = QsFileFilter());
	static bool SearchFolder(const QcString& sBasePath, QcString& Folder, bool bUp);

	static int FilterFileList(const QcStringList& fileList, const QcString& sMatchStr, QcStringList* pMatchFileList, QcStringList* pNotMatchFileList = NULL);
	static bool MatchFile(const QcString& fileName, const QcString& sMatchStr);
	static bool MatchFile(const QcString& fileName, const QcStringList& sMatchList);

	static void CopyFileList(const QcStringList& fileList, const QcString& sDestPath, const QcString& sBasePath = "");
	static void CopyFileList(const QcString& sSourcePath, const QcString& sDestPath, const QsFileFilter& fileFilter = QsFileFilter(), bool bSamePath = true);
	static void CopyFile(const QcString& sourceFile, const QcString& destFile);

	static void DeleteFileList(const QcStringList& fileList);
	static void DeleteDir(const QcString& sPath);
	static void DeleteFile(const QcString& sFile);
	static void DeleteFile(const QcString& sPath, const QsFileFilter& fileFilter);
	static void RemoveFile(QcStringList& pathFileList, const QcStringList& removeFile);
	static void RemoveFile(QcStringList& pathFileList, const QcString& file);

	static void UnifyPathName(QcString& str, bool RemoveBackslash = false);
	static void MakePath(const QcString& sPath);

	static bool GetFileTime(const char* psFile, LPFILETIME lpLastWriteTime, LPFILETIME lpCreationTime = NULL, LPFILETIME lpLastAccessTime = NULL)
	{
		HANDLE hFile = CreateFileA(psFile, GENERIC_READ | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_READONLY, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		::GetFileTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
		CloseHandle(hFile);
		return true;
	}

	static void SplitPath(const QcString& filePath, QcString& pathList, QcString& fileName);

	static bool IsExist(const QcString& file)
	{
		return GetFileAttributesA(file.c_str()) != 0xFFFFFFFF;
	}
	static bool IsExist(const char* psFile)
	{
		return GetFileAttributesA(psFile) != 0xFFFFFFFF;
	}

	void AddSearchPath(const QcString& path)
	{
		m_arSearchPath.push_back(path);
	}
	void AddSearchPath(const QcStringList& pathList)
	{
		m_arSearchPath.insert(m_arSearchPath.end(), pathList.begin(), pathList.end());
	}
	bool FindFile(const char* psFileName, QcString& sFullPathName)
	{
		if (psFileName != sFullPathName.c_str())
			sFullPathName = psFileName;
		return FindFile(sFullPathName);
	}
	bool FindFile(QcString& sFile)
	{
		if (QcFileSystem::IsExist(sFile.c_str()))
		{
			if (PathIsRelativeA(sFile.c_str()))
			{
				char buf[MAX_PATH] = {0};
				char buf2[MAX_PATH] = {0};
				GetCurrentDirectoryA(MAX_PATH, buf);
				sprintf_s(buf2, "%s\\%s", buf, sFile.c_str());
				sFile = buf2;
			}
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
		for (; iter != m_arSearchPath.end(); ++iter)
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
protected:
	QcStringList m_arSearchPath;
};