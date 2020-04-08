#pragma once
#include <ShlObj.h>

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

class QcFileSystem
{
public:
	static void GetFileList(const QcString& sPath, QcStringList* pFileList, QcStringList* pSubFolderPathList, const QsFileFilter& fileFilter = QsFileFilter());
	static void GetFileList(const QcString& sPath, QcStringList& pFileList, const QsFileFilter& fileFilter = QsFileFilter())
	{
		QcFileSystem::GetFileList(sPath, &pFileList, NULL, fileFilter);
	}
	static void GetSubFolderList(const QcString& sPath, QcStringList& pSubFolderPathList, const QsFileFilter& fileFilter = QsFileFilter())
	{
		QcFileSystem::GetFileList(sPath, NULL, &pSubFolderPathList, fileFilter);
	}
	static bool SearchUpFolder(const QcString& sBasePath, QcString& Folder);
	static bool SearchSubFolder(const QcString& sBasePath, QcString& Folder);

	static void FilterFileList(const QcStringList& fileList, const QcString& sMatchStr, QcStringList* pMatchFileList, QcStringList* pNotMatchFileList = NULL);
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
	
	static void UnifyPathName(QcString& str)
	{
		if (!str.empty())
		{
			char last = str.at(str.length() - 1);
			if (last != '\\' && last != '/')
				str += "\\";
		}
	}
	static inline bool MakePath(const QcString& sPath)
	{
		return MakePath(sPath.c_str());
	}
	static inline bool MakePath(const char* psFile)
	{
		if (!PathFileExistsA(psFile))
		{
			return SHCreateDirectoryExA(NULL, psFile, NULL) == ERROR_SUCCESS;
		}
		return true;
	}

	static bool GetFileTime(const char* psFile, LPFILETIME lpLastWriteTime, LPFILETIME lpLastAccessTime = NULL, LPFILETIME lpCreationTime = NULL, DWORD* pFileSize = NULL)
	{
		HANDLE hFile = CreateFileA(psFile, GENERIC_READ | GENERIC_EXECUTE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_READONLY, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;

		::GetFileTime(hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime);
		if (pFileSize)
			GetFileSize(hFile, pFileSize);

		CloseHandle(hFile);
		return true;
	}

	static bool IsFullPath(const char* szPath)
	{
		//Full path len >= 2
		if (szPath[0] == '\0' || szPath[1] == '\0')
		{
			return false;
		}
		if (*szPath == '"') //"<path>" - skip the quote.
		{
			szPath++;
		}
		if (szPath[1]==':')		// drive: case
		{
			return true;
		}
		if (szPath[0]=='\\' && szPath[1]=='\\' && 
			szPath[2] != '\0' ) // unc path name
		{		
			return true;
		}
		return false;
	}
	static void SplitPath(const QcString& filePath, QcString& path, QcString& file)
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
	
	static bool IsExist(const QcString& file)
	{
		return IsExist(file.c_str());
	}
	static bool IsExist(const char* psFile)
	{
		return GetFileAttributesA(psFile) != INVALID_FILE_ATTRIBUTES;
	}
	static const char* FindFileName(const char* psPath)
	{
		return ::PathFindFileNameA(psPath);
	}
};