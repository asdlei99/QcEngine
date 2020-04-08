#include "pch.hpp"
#include "QcMediaFileFinder.hpp"

QcMediaFileFinder::QcMediaFileFinder()
{
	Reset();
}

void QcMediaFileFinder::Reset()
{
	m_arSearchPath.clear();

	char strExePath[MAX_PATH] = {0};
	GetModuleFileNameA( NULL, strExePath, MAX_PATH );
	strExePath[MAX_PATH - 1] = 0;
	char* strLastSlash = strrchr( strExePath, '\\');
	if (strLastSlash)
	{
		*strLastSlash = 0;
	}
	QcString exePath(strExePath);
	m_arSearchPath.push_back(exePath);
#ifdef QmTestVersion
	QcString PakPath("PakTest");
	if (SearchMediaPath(exePath, PakPath))
	{
		AddSearchPath(PakPath);
	}
	QcString MergeImagePath("MergeMedia");
	if (SearchMediaPath(exePath, MergeImagePath))
	{
		AddSearchPath(MergeImagePath, true);
		//QcStringList subFolderList;
		//QcFileSystem::GetSubFolderList(MergeImagePath, subFolderList);
		//m_arSearchPath.insert(m_arSearchPath.end(), subFolderList.begin(), subFolderList.end());
	}
	QcString AssistImagePath("AssistImage");
	bool bFindAssistImage = false; 
	if (SearchMediaPath(exePath, AssistImagePath))
	{
		AddSearchPath(AssistImagePath, true);
		//QcStringList subFolderList;
		//QcFileSystem::GetSubFolderList(AssistImagePath, subFolderList);
		//m_arSearchPath.insert(m_arSearchPath.end(), subFolderList.begin(), subFolderList.end());
		bFindAssistImage = true;
	}
	QcString mediaPath("Media");
	if (SearchMediaPath(exePath, mediaPath))
	{
		AddSearchPath(mediaPath);
		QcStringList subFolderList;
		QcFileSystem::GetSubFolderList(mediaPath, subFolderList, QsFileFilter("*.*", false));
		QcFileSystem::UnifyPathName(mediaPath);
		if (bFindAssistImage)
			QfErase(subFolderList, mediaPath + "Image\\");

		QcStringListIter iter(subFolderList.begin());
		for (; iter != subFolderList.end(); ++iter)
		{
			AddSearchPath(*iter, true);
		}
	}
#endif
	if (!IsDebuggerPresent())
		m_arSearchPath.push_back(QcString("C:\\"));

	//m_arSearchPath.erase(std::unique(m_arSearchPath.begin(), m_arSearchPath.end()), m_arSearchPath.end());
}

bool QcMediaFileFinder::SearchMediaPath(const QcString& modulePath, QcString& folder)
{
	char path[MAX_PATH] = {0};
	strcpy_s(path, modulePath.c_str());
	for (;;)
	{
		char buf[MAX_PATH] = {0};
		sprintf_s(buf, MAX_PATH, "%s\\%s", path, folder.c_str());
		if (QcFileSystem::IsExist(buf))
		{
			folder = buf;
			return true;
		}
		if (!PathRemoveFileSpecA(path))
		{
			break;
		}
	}
	return false;
}

