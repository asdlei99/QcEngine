#include "pch\Pch.hpp"
#include "QcApp.hpp"

#pragma warning(disable:4073)
#pragma init_seg(lib)

QcApp* QcApp::gsApp = NULL;
char gsLogPath[MAX_PATH] = {0};
void SetLogPath(const char* psPath)
{
	if (QcFileSystem::MakePath(psPath))
	{
		strcpy_s(gsLogPath, psPath);
		int nLen = strlen(gsLogPath);
		if (nLen)
		{
			char lastCh = gsLogPath[nLen-1];
			if (lastCh == '\\' && lastCh == '/')
				gsLogPath[nLen-1] = 0;
		}
	}
}

const char* GetLogPath()
{
	return gsLogPath;
}

QcApp::QcApp()
{
	gsApp = this;

	std::locale::global(std::locale(""));

	char szBuff[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szBuff, MAX_PATH);
	PathRemoveFileSpecA(szBuff);
	SetCurrentDirectoryA(szBuff);

	strcat_s(szBuff, "\\Log");
	SetLogPath(szBuff);

	srand((unsigned int)time(0));
}



