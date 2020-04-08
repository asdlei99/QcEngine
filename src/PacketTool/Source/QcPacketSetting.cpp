#include "pch.hpp"
#include "QcPacketSetting.hpp"
#include "QcPacketPerformer.hpp"

static void ReadSubElmentValue(TiXmlElement* pElment, const char* psName, QcString& value)
{
	if (pElment)
	{
		pElment = pElment->FirstChildElement(psName);
		if (pElment)
			value = pElment->GetText();
	}
}

static void ReadSubElmentValue(TiXmlElement* pElment, const char* psName, int& value)
{
	QcString str;
	ReadSubElmentValue(pElment, psName, str);
	value = atoi(str.c_str());
}

static void ReadElmentAttibute(TiXmlElement* pElment, const char* psName, QcString& value)
{
	const char* pAttribute = pElment->Attribute(psName);
	if (pAttribute)
		value = pAttribute;
}

static void ReadElmentAttibute(TiXmlElement* pElment, const char* psName, int& value)
{
	QcString str;
	ReadElmentAttibute(pElment, psName, str);
	value = atoi(str.c_str());
}

QcPacketSetting::QcPacketSetting() 
: m_iPacketFlag(1)
,m_iEnCryptFlag(1)
{
	m_bHaveEnKey = false;
}

void QcPacketSetting::AnalysisPara(const QcString& strPara, QsParaValues& para)
{
	printf("Param: %s\n", strPara.c_str());

	QcStringList valueList;
	QfSplitString(strPara.c_str(), valueList, ';');
	QcStringList::iterator iter(valueList.begin());
	for (; iter != valueList.end(); ++iter)
	{
		QcStringList keyValue;
		QfSplitString(iter->c_str(), keyValue, '=');
		if (keyValue.size() != 2)
			continue;

		if (_stricmp(keyValue[0].c_str(), "SettingFile") == 0)
		{
			para.m_sSettingFile = keyValue[1];
		}
		else if (_stricmp(keyValue[0].c_str(), "TargetPath") == 0)
		{
			para.m_sPacketSourcePath = keyValue[1];
		}
		else if (_stricmp(keyValue[0].c_str(), "PacketDll") == 0)
		{
			para.m_iPacketFlag = atoi(keyValue[1].c_str());
		}
		else if (_stricmp(keyValue[0].c_str(), "merge") == 0)
		{
			para.m_iMergeFlag = atoi(keyValue[1].c_str());
		}
		else if (_stricmp(keyValue[0].c_str(), "DelTmp") == 0)
		{
			para.m_iDelTmpAfterFinish = atoi(keyValue[1].c_str());
		}
		else if (_stricmp(keyValue[0].c_str(), "Encrypt") == 0)
		{
			para.m_iEnCryptFlag = atoi(keyValue[1].c_str());
		}
		else if (_stricmp(keyValue[0].c_str(), "ExcuteFlag") == 0)
		{
			para.m_iExcuteFlag = atoi(keyValue[1].c_str());
		}
		else if (_stricmp(keyValue[0].c_str(), "EnKey") == 0)
		{
			DWORD key[4] = {0xc54b84b2, 0x63183b3f, 0x451c314f, 0x4030201};
			QcPacketPerformer::GetInstance()->SetEncryptKey(key, sizeof(key));
			sscanf_s(keyValue[1].c_str(), "%u_%u_%u_%u", &(key[0]), &(key[1]), &(key[2]), &(key[3]));
			QcPacketPerformer::GetInstance()->DecryptData((char*)key, sizeof(key));
			QcPacketPerformer::GetInstance()->SetEncryptKey(key, sizeof(key));
			m_bHaveEnKey = true;
		}
		else if (_stricmp(keyValue[0].c_str(), "ModuleList") == 0)
		{
			QfSplitString(keyValue[1].c_str(), para.m_sModuleList, '|');
		}
	}
}

bool QcPacketSetting::ReadSetting(const char* psSetting, const char* psPara)
{
	QsParaValues para;
	AnalysisPara(QcString(psPara), para);
	m_iExcuteFlag = para.m_iExcuteFlag;
	
	QcFileFinder fileFinder;
	QcStringList folderList;
	QcString temp;

	//当前路径
	char pathBuf[MAX_PATH] = {0}; 
	GetCurrentDirectoryA(MAX_PATH, pathBuf);
	QcString currentPath(pathBuf);
	QcFileSystem::UnifyPathName(currentPath);
	fileFinder.AddSearchPath(currentPath);

	QcFileSystem::SplitPath(para.m_sPacketSourcePath, m_sSourceTargetDir, m_sPacketSourceName);
	QcFileSystem::UnifyPathName(m_sSourceTargetDir);
	fileFinder.AddSearchPath(m_sSourceTargetDir);
	QcString sTemp;
	QcString sPacketVersion;			//版本
	QcFileSystem::SplitPath(m_sSourceTargetDir, sTemp, sPacketVersion);
	if (sPacketVersion.size())
	{
		sPacketVersion.erase(sPacketVersion.end() - 1);
		m_sPacketVersion = sPacketVersion;
		if (sPacketVersion != "Release")
			sPacketVersion = "Test";
	}

	//mbox2w.exe
	QcString packetPath = "AssistTools";
	QcFileSystem::SearchUpFolder(m_sSourceTargetDir, packetPath);
	QcFileSystem::GetSubFolderList(packetPath, folderList);
	fileFinder.AddSearchPath(folderList);

	m_mediaPath = "Media";
	QcFileSystem::SearchUpFolder(m_sSourceTargetDir, m_mediaPath);
	m_imageMediaPath = "Image";
	QcFileSystem::SearchSubFolder(m_mediaPath, m_imageMediaPath);

	QcFileSystem::SplitPath(m_mediaPath, m_sOutPath, temp);
	QcFileSystem::UnifyPathName(m_sOutPath);
	m_imageMergePath = m_sOutPath + "Output\\MergeMedia";	//合图路径
	m_assistImagePath = m_sOutPath + "Output\\AssistImage";
	m_pakPath = m_sOutPath + "Output\\Pak" + sPacketVersion;
	m_sOutPath += "Output\\PacketOutput";					//输出路径
	 
	m_sImgeMergeTool = "ImageMerger.exe";
	fileFinder.FindFile(m_sImgeMergeTool);

	QcString settingFile(psSetting);
	if (para.m_sSettingFile.size())
		settingFile = para.m_sSettingFile;
	fileFinder.FindFile(settingFile);

	m_sDllPathList = para.m_sModuleList;

	m_sPacketTool = "mbox2c.exe";
	fileFinder.FindFile(m_sPacketTool);

	TiXmlDocument document;
	do 
	{
		if (!document.LoadFile(settingFile.c_str()))
			break;
		TiXmlElement* pRootElement = document.RootElement();
		if (pRootElement == NULL)
			break;
		TiXmlElement* pImageCutter = pRootElement->FirstChildElement("ImageCutter");
		if (pImageCutter == NULL)
			break;
		QcString sBuf;
		ReadElmentAttibute(pImageCutter, "filter", sBuf);
		if (sBuf.size())
		{
			QfSplitString(sBuf.c_str(), m_arCutFilterList, ';');
		}
	}
	while (0,0);
	return true;
}