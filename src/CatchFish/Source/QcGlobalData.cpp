#include "pch.hpp"
#include "QcGlobalData.hpp"
#include "App\QcGameApp.hpp"
#include "Scene\QcScene.hpp"
#include "Common\QcMediaFileFinder.hpp"
#include "Res\QcResPackLoader.hpp"
#include "FileStorageSystem\QcSystemSetting.hpp"

#pragma warning(disable:4073)
#pragma init_seg(lib)
QsGlobalData QsGlobalData::gGlobalData;
s32 giExitFlag = 0;
u64 giFrameID = 0;
s32 giScreenHandleing = eScreen1;

QsGlobalData::QsGlobalData()
: gScreenRect(-1920.f * 0.5f, -1080.f * 0.5, 1920.f * 0.5f, 1080.f * 0.5)
{
	giExitFlag = 0;
	giForceTexInVideoMem = 0;
	
	std::locale::global(std::locale(""));

	char szBuff[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szBuff, MAX_PATH);
	PathRemoveFileSpecA(szBuff);
	SetCurrentDirectoryA(szBuff);

	QcString gsLogPath;
#ifdef QmTestVersion
	QcString sBasePath("Output");
	if (!QcFileSystem::SearchUpFolder(szBuff, sBasePath))
	{
		sBasePath = szBuff;
	}
	gsLogPath = sBasePath + "\\Log";
	gsFileStoragePath = sBasePath + "\\TempLog";
	if (IsDebuggerPresent())
		QcFileSystem::DeleteDir(gsLogPath);
#else
	gsLogPath = "D:\\Log";
	gsFileStoragePath = "D:\\TempLog";
	if (!QcFileSystem::IsExist("D:\\"))
	{
		gsLogPath = "C:\\Log";
		gsFileStoragePath = "C:\\TempLog";
	}
#endif
	QcFileSystem::MakePath(gsFileStoragePath);
	QcFileSystem::MakePath(gsLogPath);
	
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	unsigned int timeValue = MAKELONG(sysTime.wMilliseconds * sysTime.wHour, sysTime.wSecond * sysTime.wMinute);
	srand(timeValue);
	PlantSeeds(timeValue);

	goApp = new QcGameApp();
	gpMediaFinder = new QcMediaFileFinder();
	gpSysSetting = new QcSystemSetting();
	m_pFilePacker = new QcResPackLoader();
	gpSoundMgr = new QcSoundMgr();
	gpAlgorithm = new QcFishAlgorithm();
	gpAnimateCreator = new QcAnimatorCreator();
	gpCrypt = new QcAESCrypt();

	QmSetLogPath(gsLogPath.c_str());
}

QsGlobalData::~QsGlobalData()
{
	delete gpCrypt;
	delete gpAnimateCreator;
	delete gpAlgorithm;
	delete gpSoundMgr;
	delete gpSysSetting;
	delete gpMediaFinder;
	delete goApp;
}

