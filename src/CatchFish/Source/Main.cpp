#include "Pch.hpp"
#include "QcGlobalData.hpp"
#include "App\QcGameApp.hpp"
#include "Common\QcMediaFileFinder.hpp"

#pragma fenv_access(on)

int DealWithPara(const char* psPara);
#ifdef _WINDLL
int Start(LPWSTR lp, int nCount)
#else
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR lp, int nCount)
#endif
{
	__try
	{
		unsigned int control_word = 0;
		_controlfp_s(&control_word, 0, 0);
		if (control_word & PC_24 || control_word & PC_53)
			_controlfp_s(&control_word, PC_53, MCW_PC);

		int iRet = 0;
		if (nCount != 0)
		{
			USES_CONVERSION;
			iRet = DealWithPara(W2A(lp));
		}
		if (iRet == 0 || iRet == 1)
		{
#ifdef QmLogOn
			GetInstance<QcLogEngine>();
#endif
			QmGlobalData.goApp->Initialize(iRet == 0);
		}
	}
	__except(MakeDumpFile("Crash", GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER)
	{
		QmApp->ExitApp();
	}
	return 0;
}

int DealWithPara(const char* psPara)
{
	QcString sPara(psPara);
	if (sPara.size() == 0)
		return 0;

	if (sPara[0] == '\"')
	{
		sPara.erase(sPara.begin());
		sPara.erase(sPara.end() - 1);
	}
	psPara = sPara.c_str();

	//Ë«ÆÁÒÑ¾­Æô¶¯
	if (QfStrNoCaseCmp(psPara, "WaitProgram.exe") == 0)
	{
		return 1;
	}
	else
	{
		QcString sExeFilePath = "TargetPath=";
		char exeFilePath[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, exeFilePath, MAX_PATH);
		sExeFilePath += exeFilePath;

		//Find Packet Tool.
		PathRemoveFileSpecA(exeFilePath);
		QcString AssistToolsPath = "AssistTools";
		QcFileSystem::SearchUpFolder(QcString(exeFilePath), AssistToolsPath);
		QcStringList folderList;
		QcFileSystem::GetFileList(AssistToolsPath, NULL, &folderList);
		folderList.push_back(AssistToolsPath);
		QmGlobalData.gpMediaFinder->AddSearchPath(folderList);

		QcString sPacketTool = "PacketTool.exe";
		if (QmGlobalData.gpMediaFinder->FindFile(sPacketTool))
		{
			QcString moduleList = "ModuleList=";
			HMODULE hMods[1024];
			HANDLE hProcess = GetCurrentProcess();
			DWORD cbNeeded;
			if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
			{
				char szModName[MAX_PATH];
				for (u32 i = 0; i < (cbNeeded / sizeof(HMODULE)); ++i)
				{
					if ( GetModuleFileNameExA( hProcess, hMods[i], szModName,
						sizeof(szModName) / sizeof(TCHAR)))
					{
						if (sExeFilePath != szModName)
						{
							moduleList += szModName;
							moduleList += "|";
						}
					}
				}
			}

			QcString sEnKey;

			if (InitCom())
			{
				DWORD key[4] = {0xc54b84b2, 0x63183b3f, 0x451c314f, 0x4030201};
				QmGlobalData.SetEncryptKey(key, sizeof(key));
				GetEnKey(1, key);
				ShutDownIOThread();
				QmGlobalData.EncryptData((char*)key, sizeof(key));

				char sBuf[256];
				sprintf_s(sBuf, "EnKey=%u_%u_%u_%u",key[0], key[1], key[2], key[3]);
				sEnKey = sBuf;
			}
			else
			{
				QmLog("fail to InitCom");
			}
			sPara = psPara;
			sPara += QcString(";") + moduleList;
			sPara += QcString(";") + sExeFilePath;
			sPara += QcString(";") + sEnKey;

			QfExcuteShell(sPacketTool.c_str(), sPara.c_str(), 0);
		}
	}
	return 3;
}

