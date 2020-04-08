#include "pch.hpp"
#include "QcPacketPerformer.hpp"
#include "QcMinImageCutter.hpp"
#include "QcFilePacker2.hpp"
#include <iostream>

#define NEWLINE_CHAR "\n"

QcPacketPerformer::QcPacketPerformer()
{
	gpCrypt = new QcAESCrypt();
}

bool QcPacketPerformer::Excute(const char* psPara)
{
	if (m_packetSetting.ReadSetting("PacketSetting.xml", psPara))
	{
		QcFileSystem::UnifyPathName(m_packetSetting.m_sSourceTargetDir);
		QcFileSystem::UnifyPathName(m_packetSetting.m_mediaPath);
		QcFileSystem::UnifyPathName(m_packetSetting.m_imageMediaPath);
		QcFileSystem::UnifyPathName(m_packetSetting.m_imageMergePath);
		QcFileSystem::UnifyPathName(m_packetSetting.m_assistImagePath);
		QcFileSystem::UnifyPathName(m_packetSetting.m_pakPath);
		QcFileSystem::UnifyPathName(m_packetSetting.m_sOutPath);

		switch(m_packetSetting.m_iExcuteFlag)
		{
		case -1:
		case 0:
		case 3:
		case 5:
			{
				//只打包程序不需要密钥
				if (m_packetSetting.m_iExcuteFlag != -1)
				{
					if (!m_packetSetting.m_bHaveEnKey)
					{
						printf("获取密钥失败, 任意键退出\n");
						getchar();
						return 0;
					}
				}

				if (m_packetSetting.m_iExcuteFlag == 3)
					return DoPacketImage(m_packetSetting);
				return DoPacketExcute(m_packetSetting);
				break;
			}
		case 1:
			{
				return DoAssistImage(m_packetSetting);
				break;
			}
		case 2:
			{
				return DoMergeImage(m_packetSetting);
				break;
			}
		case 4:
			{
				return DoRepairImage(m_packetSetting);
				break;
			}
		}
	}
	return false;
}

bool QcPacketPerformer::DoPacketExcute(QcPacketSetting& setting)
{
	bool bMBox2wOn = setting.m_iExcuteFlag == 5;
	printf("TargetPath:%s%s\n", setting.m_sSourceTargetDir.c_str(), setting.m_sPacketSourceName.c_str());
	printf("mediaPath:%s\n", setting.m_mediaPath.c_str());
	printf("image media path:%s\n", setting.m_imageMediaPath.c_str());
	printf("assist image path: %s\n", setting.m_assistImagePath.c_str());
	printf("Output Path:%s\n", setting.m_sOutPath.c_str());

	QcString destMediaPath(setting.GetOutputPath() + "Media\\");
	QcFileSystem::DeleteDir(setting.GetOutputPath());
	if (bMBox2wOn)
		QcFileSystem::MakePath(destMediaPath);
	else
		QcFileSystem::MakePath(setting.GetOutputPath());

	printf("拷贝资源开始\n");

	//Copy Media;
	if (setting.m_iExcuteFlag != -1)
	{
		//拷贝加密Media子目录下非图片资源
		QcString imagePath = setting.m_imageMediaPath;
		QcStringList mediaSubFolderList;
		QcFileSystem::GetSubFolderList(setting.m_mediaPath, mediaSubFolderList, QsFileFilter("*.*", false));
		QfErase(mediaSubFolderList, setting.m_imageMediaPath);

		//拷贝加密图片资源
		QcStringList mediaFileList;
		if (QcFileSystem::IsExist(setting.m_assistImagePath.c_str()))
		{
			QcFileSystem::GetFileList(setting.m_assistImagePath, &mediaFileList, NULL, QsFileFilter("*.xml", true));
			imagePath = setting.m_assistImagePath;
		}
		if (QcFileSystem::IsExist(setting.m_imageMergePath.c_str()))
		{
			QcFileSystem::GetFileList(setting.m_imageMergePath, &mediaFileList, NULL, QsFileFilter("*.xml", true));
			imagePath = setting.m_imageMergePath;
		}
		if (!QcFileSystem::IsExist(setting.m_pakPath.c_str()))
		{
			mediaSubFolderList.push_back(imagePath);
		}
		//拷贝加密Media目录下图片资源
		mediaSubFolderList.push_back(setting.m_mediaPath);
		int iCount = mediaSubFolderList.size();
		for (int i=0; i<iCount; ++i)
		{
			if (i == iCount - 1)
				QcFileSystem::GetFileList(mediaSubFolderList[i], &mediaFileList, NULL, QsFileFilter("*.*", false));
			else
				QcFileSystem::GetFileList(mediaSubFolderList[i], &mediaFileList, NULL, QsFileFilter("*.*", true));
		}
		QcFileSystem::RemoveFile(mediaFileList, "ylwq_update.ini");

		printf("开始拷贝Pak文件\n");
		QcFileSystem::CopyFileList(setting.m_pakPath, setting.GetOutputPath(), QsFileFilter("Temp.*", false));
		printf("结束拷贝Pak文件\n");

		printf("开始加密\n");
		//打包到大文件
		{
			QcFilePacker2 packer(QcString(setting.GetOutputPath() + PackName), true);
			packer.PakFile(mediaFileList);
		}
		EncryptFile(QcString(setting.GetOutputPath() + PackName));
		printf("结束加密\n");
	}

	//copy exe, dll
	printf("copy Dll and exe \n");
	QcStringList copyList;
	//if (bMBox2wOn)
	{
		if (setting.m_iExcuteFlag != -1)
			QcFileSystem::GetFileList(setting.m_mediaPath, &copyList, NULL, QsFileFilter("*.ini", false));
		copyList.insert(copyList.end(), setting.m_sDllPathList.begin(), setting.m_sDllPathList.end());
	}
	QcFileSystem::GetFileList(setting.m_sSourceTargetDir, copyList, QsFileFilter("*.pdb", false));
	copyList.push_back(setting.m_sSourceTargetDir + setting.m_sPacketSourceName);
	QcFileSystem::RemoveFile(copyList, "ntdll.dll");
	QcFileSystem::RemoveFile(copyList, "kernel32.dll");
	QcFileSystem::RemoveFile(copyList, "kernelBase.dll");
	QcFileSystem::RemoveFile(copyList, "user32.dll");
	QcFileSystem::RemoveFile(copyList, "gdi32.dll");
	if (bMBox2wOn)
	{
		QcFileSystem::RemoveFile(copyList, "ylwq_update.ini");
	}
	QcFileSystem::CopyFileList(copyList, setting.GetOutputPath());

	//生成配置 ,打包
	if (bMBox2wOn)
	{
		printf("生成配置文件\n");
		QcString cfgFile = setting.GetOutputPath() + setting.m_sPacketSourceName;
		CreateMBox2wSetting(setting);
		printf("打包开始\n");
		ExcuteShell(setting.m_sPacketTool.c_str(), cfgFile.c_str());
		printf("打包结束\n");

		QcString sourceName = setting.GetOutputPath() + setting.m_sPacketSourceName + ".exe";
		QcString destName;
		char buf[MAX_PATH] = {0};
		strcpy_s(buf, sourceName.c_str());
		PathRemoveExtensionA(buf);
		destName = buf;

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);	
		sprintf_s(buf, "%s_%04d-%02d-%02d_%s_Encrypt.exe", destName.c_str(), sysTime.wYear, sysTime.wMonth, sysTime.wDay, setting.m_sPacketVersion.c_str());
		MoveFileA(sourceName.c_str(), buf);

		QcStringList removeFileList;

		QcString packetName(setting.GetOutputPath() + PackName);
		char dataFile[MAX_PATH] = {0};
		strcpy_s(dataFile, packetName.c_str());
		PathRemoveExtensionA(dataFile);
		removeFileList.push_back(packetName);
		removeFileList.push_back(dataFile);
		removeFileList.push_back(cfgFile);
		//QcFileSystem::GetFileList(setting.GetOutputPath(), removeFileList);
		//QcFileSystem::RemoveFile(removeFileList, QcString(buf));
		QcFileSystem::DeleteFileList(removeFileList);
	}
	else
	{
		QcString sourceName = setting.GetOutputPath() + setting.m_sPacketSourceName;
		QcString destName;
		char buf[MAX_PATH] = {0};
		strcpy_s(buf, sourceName.c_str());
		PathRemoveExtensionA(buf);
		destName = buf;

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);	
		sprintf_s(buf, "%s_%04d-%02d-%02d_%s_Encrypt.exe", destName.c_str(), sysTime.wYear, sysTime.wMonth, sysTime.wDay, setting.m_sPacketVersion.c_str());
		MoveFileA(sourceName.c_str(), buf);
	}
	ShellExecuteA(NULL, "explore", setting.GetOutputPath().c_str(), NULL, NULL, SW_SHOW);
	//ExcuteShell("explore");
	return true;
}

void QcPacketPerformer::ExcuteShell(const char* psExe, const char* psPara)
{
	SHELLEXECUTEINFOA exeInfo = {0};
	exeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	exeInfo.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE | 0x04000000;
	exeInfo.lpFile = psExe;
	exeInfo.lpParameters = psPara;
	exeInfo.lpVerb = "open";
	exeInfo.nShow = SW_HIDE;
	ShellExecuteExA(&exeInfo);
	DWORD dWord = WaitForSingleObject(exeInfo.hProcess, INFINITE);
	CloseHandle(exeInfo.hProcess );
}

void QcPacketPerformer::CreateImageMergeSetting(const QcStringList& imageList, const QcString& fileName)
{
	std::fstream fsWriteFile(fileName.c_str(), std::ios::in | std::ios::out| std::ios::trunc);

	fsWriteFile<<fileName<<NEWLINE_CHAR;

	QcStringListConstIter iter(imageList.begin());
	for (; iter != imageList.end(); iter++)
	{
		fsWriteFile<<*iter<<NEWLINE_CHAR;
	}
	fsWriteFile.close();
}

void QcPacketPerformer::CreateMBox2wSetting(const QcPacketSetting& setting)
{
	QcString fileName = setting.GetOutputPath() + setting.m_sPacketSourceName + ".mbxcfg";
	std::fstream fsWriteFile(fileName.c_str(), std::ios::in | std::ios::out| std::ios::trunc);

	fsWriteFile<<NEWLINE_CHAR;
	fsWriteFile<<"box.encrypt=1"<<NEWLINE_CHAR;
	fsWriteFile<<"box.packdefault=1"<<NEWLINE_CHAR;
	fsWriteFile<<"box.separate=0"<<NEWLINE_CHAR;
	fsWriteFile<<"box.ziplevel=0"<<NEWLINE_CHAR;			//不压缩
	fsWriteFile<<"exe.crc=0"<<NEWLINE_CHAR;
	fsWriteFile<<"exe.pack=1"<<NEWLINE_CHAR;
	fsWriteFile<<"exe.packedfilesvisible=1"<<NEWLINE_CHAR;
	fsWriteFile<<"exe.source="<<setting.GetOutputPath()<<setting.m_sPacketSourceName<<NEWLINE_CHAR;
	fsWriteFile<<"exe.target="<<setting.GetOutputPath()<<setting.m_sPacketSourceName<<".exe"<<NEWLINE_CHAR;
	fsWriteFile<<"key.0=9E5E704454F7C740BD17D975C86DD33D"<<NEWLINE_CHAR;
	fsWriteFile<<"key.1=9705C475944E05489FD019966E7CCB40"<<NEWLINE_CHAR;

	fsWriteFile<<NEWLINE_CHAR;
	fsWriteFile<<"box.contents~"<<NEWLINE_CHAR;
	fsWriteFile<<"00 "<<setting.m_sPacketSourceName<<NEWLINE_CHAR;

	QcString packetName(setting.GetOutputPath() + PackName);
	char dataFile[MAX_PATH] = {0};
	strcpy_s(dataFile, PackName);
	PathRemoveExtensionA(dataFile);
	strcat_s(dataFile, PackDataExtension);
	fsWriteFile<<"00 "<<PackName<<NEWLINE_CHAR;
	fsWriteFile<<"00 "<<dataFile<<NEWLINE_CHAR;



	//char buf[MAX_PATH] = {0};
	//PathRelativePathToA(buf, m_sBasePath.c_str(), FILE_ATTRIBUTE_DIRECTORY,sPath.c_str() , FILE_ATTRIBUTE_DIRECTORY);
	//if (buf[0] == '/' || buf[0] == '\\' || buf[0] == '.')
	//	pBuf++;
	//if (setting.m_iPacketFlag)
	{
		//QcFileSystem::ForEachFile(setting.GetOutputPath(), MBox2wContentFiller(fsWriteFile, setting.GetOutputPath()), QsFileFilter("*.dll", false));
		//QcFileSystem::ForEachFile(setting.GetOutputPath(), MBox2wContentFiller(fsWriteFile, setting.GetOutputPath()), QsFileFilter("Temp.*", false));
	}
	//QcFileSystem::ForEachFile(setting.GetOutputPath() + "Media\\", MBox2wContentFiller(fsWriteFile, setting.GetOutputPath()), QsFileFilter("*.*", true));

	fsWriteFile<<"~"<<NEWLINE_CHAR;
	fsWriteFile.close();
}

void QcPacketPerformer::EncryptFile(const QcStringList& fileList)
{
	QcStringList::const_iterator iter(fileList.begin());
	QcStringList::const_iterator endIter(fileList.end());
	for (; iter != endIter; ++iter)
	{
		EncryptFile(*iter);
	}
}

void QcPacketPerformer::EncryptFile(const QcString& file)
{
#ifdef QmWinFileOn
	DWORD dwOut = 0; 
	HANDLE ifs = CreateFileA(file.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwFileSize = GetFileSize(ifs, 0);
	char* pSource = new char[dwFileSize];
	ReadFile(ifs, pSource, dwFileSize, &dwOut, 0);
	EncryptData(pSource, dwFileSize);
	SetFilePointer(ifs, 0, 0, FILE_BEGIN);
	WriteFile(ifs, pSource, dwFileSize, &dwOut, 0);
	CloseHandle(ifs);
	delete [] pSource;
#else
	std::fstream ifs(file.c_str(), std::ios::binary | std::ios::in | std::ios::out);
	ifs.seekg(0, std::ios::end);
	DWORD dwFileSize = ifs.tellg();
	char* pSource = new char[dwFileSize];
	ifs.seekg(0, std::ios::beg);
	ifs.read(pSource, dwFileSize);

	EncryptData(pSource, dwFileSize);

	ifs.seekg(0, std::ios::beg);
	ifs.write(pSource, dwFileSize);
	ifs.close();

	delete [] pSource;
#endif
}


bool QcPacketPerformer::DoAssistImage(QcPacketSetting& setting)
{
	DWORD tm = GetTickCount();
	QcFileSystem::MakePath(setting.m_assistImagePath);
	QcMinImageCutter imageCutter;
	imageCutter.DoExcute(setting.m_imageMediaPath, setting.m_assistImagePath + "offsetPoint.xml", setting.m_arCutFilterList);
	tm = GetTickCount() - tm;
	printf("裁剪时间%d(ms)\n", tm);
	::Sleep(3000);
	return true;
}

bool QcPacketPerformer::DoMergeImage(QcPacketSetting& setting)
{
	//获取文件夹列表
	printf("合图开始\n");
	const QcString& sourceImagePath = QcFileSystem::IsExist(setting.m_assistImagePath.c_str()) ? setting.m_assistImagePath : setting.m_imageMediaPath;
	QcString destMergePath = setting.m_imageMergePath;
	QcFileSystem::DeleteDir(destMergePath);
	QcFileSystem::MakePath(destMergePath);
#if 0
	QcStringList imageFolderList;
	QcFileSystem::GetSubFolderList(sourceImagePath, imageFolderList, QsFileFilter("*.*", false));
	imageFolderList.push_back(sourceImagePath);

	int iCount = imageFolderList.size();
	for (int i=0; i<iCount; ++i)
	{
		bool bMerge = false;
		QcString folderName = imageFolderList[i];
		QcFileSystem::UnifyPathName(folderName, true);
		char sNameBuf[MAX_PATH] = {0};
		sprintf_s(sNameBuf, "%s", ::PathFindFileNameA(folderName.c_str()));
		folderName = sNameBuf;
		QcString destMergePath = setting.m_imageMergePath + folderName + "\\";

		FILETIME mergeTime = {0};
		if (!QcFileSystem::GetFileTime(QcString(destMergePath + "imageset_" + folderName + ".xml").c_str(), &mergeTime))
		{
			bMerge = true;
		}

		QcStringList fileList;
		if (i == iCount - 1)
			QcFileSystem::GetFileList(imageFolderList[i], fileList, QsFileFilter("*.png", false));
		else
			QcFileSystem::GetFileList(imageFolderList[i], fileList, QsFileFilter("*.png", true));
		if (!bMerge)
		{
			FILETIME writeTime;
			FILETIME createTime;
			QcStringList::iterator fileIter(fileList.begin());
			for (; fileIter != fileList.end(); ++fileIter)
			{
				if (QcFileSystem::GetFileTime(fileIter->c_str(), &writeTime, &createTime))
				{
					if (CompareFileTime(&writeTime, &mergeTime) >= 1 || CompareFileTime(&createTime, &mergeTime) >= 1)
					{
						bMerge = true;
						break;
					}
				}
			}
		}
		if (bMerge)
		{
			printf("开始合图: %s\n", folderName.c_str());
			QcFileSystem::DeleteDir(destMergePath);
			QcFileSystem::MakePath(destMergePath.c_str());

			QcString fileName(destMergePath + "imageset_" + folderName + ".txt");
			CreateImageMergeSetting(fileList, fileName);
			ExcuteShell(setting.m_sImgeMergeTool.c_str(), fileName.c_str());
			QcFileSystem::CopyFileList(sourceImagePath, destMergePath, QsFileFilter("*.xml", false), false);

			QcFileSystem::DeleteFile(fileName);
			printf("完成合图: %s\n", folderName.c_str());
		}
		else
		{
			printf("跳过合图: %s\n", folderName.c_str());
		}
	}
#else
	QcStringList fileList;
	QcFileSystem::GetFileList(sourceImagePath, fileList, QsFileFilter("*.png", true));

	bool bMerge = false;
	FILETIME mergeTime = {0};
	if (!QcFileSystem::GetFileTime(QcString(destMergePath + "imageset_All.xml").c_str(), &mergeTime))
	{
		bMerge = true;
	}
	if (!bMerge)
	{
		FILETIME writeTime;
		FILETIME createTime;
		FILETIME accessTime= {0};
		QcStringList::iterator fileIter(fileList.begin());
		for (; fileIter != fileList.end(); ++fileIter)
		{
			if (QcFileSystem::GetFileTime(fileIter->c_str(), &writeTime, &createTime, &accessTime))
			{
				if (CompareFileTime(&writeTime, &mergeTime) >= 1 || CompareFileTime(&createTime, &mergeTime) >= 1 || CompareFileTime(&accessTime, &mergeTime) >= 1)
				{
					bMerge = true;
					break;
				}
			}
		}
	}
	if (bMerge)
	{
		QcFileSystem::DeleteDir(destMergePath);
		QcFileSystem::MakePath(destMergePath.c_str());

		QcString fileName(destMergePath + "imageset_All.txt");
		CreateImageMergeSetting(fileList, fileName);
		ExcuteShell(setting.m_sImgeMergeTool.c_str(), fileName.c_str());

		QcFileSystem::DeleteFile(fileName);
	}

#endif
	printf("合图结束\n");

	return true;
}

bool QcPacketPerformer::DoPacketImage(QcPacketSetting& setting)
{
	QcFileSystem::DeleteDir(m_packetSetting.m_pakPath);
	QcFileSystem::MakePath(m_packetSetting.m_pakPath);

	QcStringList fileList;
	if (QcFileSystem::IsExist(m_packetSetting.m_imageMergePath))
	{
		QcFileSystem::GetFileList(m_packetSetting.m_imageMergePath, fileList, QsFileFilter("*.png", true));
	}
	else if (QcFileSystem::IsExist(m_packetSetting.m_assistImagePath))
	{	
		QcFileSystem::GetFileList(m_packetSetting.m_assistImagePath, fileList, QsFileFilter("*.png", true));
	}
	else
	{
		QcFileSystem::GetFileList(m_packetSetting.m_imageMediaPath, fileList, QsFileFilter("*.png", true));
	}
	DWORD tm = GetTickCount();
	QcFilePacker2 packer(QcString(m_packetSetting.m_pakPath + PackName), true);
	packer.PakFile(fileList);

	tm = GetTickCount() - tm;
	printf("加密时间%d(ms)\n", tm);
	::Sleep(3000);
	return true;
}

bool QcPacketPerformer::DoRepairImage(QcPacketSetting& setting)
{
	QcStringList fileList;
	QcFileSystem::GetFileList(m_packetSetting.m_imageMediaPath, fileList, QsFileFilter("*.png", true));

	QcStringList::const_iterator iter(fileList.begin());
	QcStringList::const_iterator endIter(fileList.end());
	for (; iter != endIter; ++iter)
	{
		int iRepairFlag = 0;
		char* pPixelData = NULL;
		unsigned long w = 0, h = 0;
		ReadImage(pPixelData, w, h, iter->c_str());
		int nLen = w * h * 4;

		int nPixel = w * h;
		QuARGB* pARGB = (QuARGB*)pPixelData;
		for(int i=0; i<nPixel; ++i, ++pARGB)
		{
			//将透明像素置为黑色透明
			if (pARGB->m_argb.a == 0 && pARGB->m_value != 0)
			{
				pARGB->m_value = 0;
				iRepairFlag |= 2;
			}
		}
		if (iRepairFlag)
		{
			printf("RepairFlag[%d]:%s\n", iRepairFlag, iter->c_str());
			WritePngImage(pPixelData, w, 0, 0, w, h, iter->c_str());
		}
		DestroyBitsData(pPixelData);
	}
	::Sleep(3000);
	return true;
}