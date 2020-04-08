#include "pch.hpp"
#include "QcFilePacker2.hpp"
#include "QcPacketPerformer.hpp"
#include <iostream>

QcFilePacker2::QcFilePacker2(const QcString& packetName, bool bPackerPixel)
: m_bPackerPixel(bPackerPixel)
, m_iIndex(-1)
, m_iErrorFlag(0)
{
#ifdef QmWinFileOn
	m_ifsFile = CreateFileA(packetName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	SetFilePointer(m_ifsFile, 0, NULL, FILE_END);

	char dataFile[MAX_PATH] = {0};
	strcpy_s(dataFile, packetName.c_str());
	PathRemoveExtensionA(dataFile);
	strcat_s(dataFile, PackDataExtension);

	m_ifsData = CreateFileA(dataFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	SetFilePointer(m_ifsData, 0, NULL, FILE_END);
	m_iCurrentPos = GetFileSize(m_ifsData, NULL);
#else
	m_ifsFile.open(packetName.c_str(), std::ios::in | std::ios::out | std::ios::app | std::ios::binary);

	char dataFile[MAX_PATH] = {0};
	strcpy_s(dataFile, packetName.c_str());
	PathRemoveExtensionA(dataFile);
	strcat_s(dataFile, PackDataExtension);

	m_ifsData.open(dataFile, std::ios::in | std::ios::out | std::ios::app | std::ios::binary);
	m_ifsData.seekg(0, std::ios::end);
	m_iCurrentPos = m_ifsData.tellg();
#endif
}

QcFilePacker2::~QcFilePacker2()
{
#ifdef QmWinFileOn
	CloseHandle(m_ifsFile);
	CloseHandle(m_ifsData);
#else
	m_ifsFile.close();
	m_ifsData.close();
#endif
}

DWORD __stdcall QcFilePacker2::ThreadFun(LPVOID lpParameter)
{
	FILETIME writeTime = {0};
	FILETIME createTime= {0};
	FILETIME accessTime= {0};

	QcFilePacker2* pThis = (QcFilePacker2*)lpParameter;
	long index = InterlockedIncrement(&pThis->m_iIndex);
	QsPackInfo info;
	while(index < pThis->ToExcuteSize())
	{
		char* pData = pThis->PakFile(pThis->m_arFileList[index], info);
		printf("打包:%s\n", info.m_fileName);
		if (pThis->m_iErrorFlag)
		{
			printf("Error:%d\n", pThis->m_iErrorFlag);
			getchar();
			return 0;
		}
		{
#ifdef QmMutilThreadOn
			QmLocker(pThis->m_cs);
#endif
			info.m_iPos = pThis->m_iCurrentPos;
			pThis->m_iCurrentPos += info.m_iLen;
			pThis->WriteData(pData, info);
		}
		delete [] pData;
		index = InterlockedIncrement(&pThis->m_iIndex);
	}
	return 0;
}

int QcFilePacker2::PakFile(const QcStringList& fileList)
{
	printf("开始打包资源\n");
	m_arFileList = fileList;

#if 0
	HANDLE arThread[QmThreadNum] = {0}; 
	for (int i=0; i<QmCountOf(arThread); ++i)
	{
		arThread[i] = CreateThread(NULL, 0, &ThreadFun, (void*)this, 0, NULL);
	}
	ThreadFun((void*)this);
	WaitForMultipleObjects(QmCountOf(arThread), arThread, TRUE, INFINITE);
	for (int i=0; i<QmCountOf(arThread); ++i)
	{
		CloseHandle(arThread[i]);
	}
#else
	ThreadFun((void*)this);
#endif

	printf("打包完成.共%d文件\n", fileList.size());
	return true;
}

char* QcFilePacker2::PakFile(const QcString& file, QsPackInfo& packetInfo)
{
	memset(&packetInfo, 0, sizeof(QsPackInfo));
	char* pData = NULL;

#ifdef QmWinFileOn
	DWORD dwOut = 0;
	HANDLE hFile = CreateFileA(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD nLen = GetFileSize(hFile, NULL);
	pData = new char[nLen];
	ReadFile(hFile, pData, nLen, &dwOut, NULL);
	CloseHandle(hFile);
#else
	std::fstream ifs(file.c_str(), std::ios::binary | std::ios::in | std::ios::out);
	ifs.seekg(0, std::ios::end);
	unsigned long nLen = ifs.tellg();
	pData = new char[nLen];
	ifs.seekg(0, std::ios::beg);
	ifs.read(pData, nLen);
	ifs.close();
#endif

	strcpy_s(packetInfo.m_fileName, PathFindFileNameA(file.c_str()));
	if (m_bPackerPixel && stricmp(PathFindExtensionA(file.c_str()), ".png") == 0)
	{
		char* pPixelData = NULL;
		unsigned long w = 0, h = 0;
		ReadImage(pPixelData, w, h, pData, nLen);
		packetInfo.m_width = w;
		packetInfo.m_height = h;
		nLen = w * h * 4;

		delete [] pData;
		pData = new char[nLen];

		if (nLen >= QmMinEncryLen)
		{
			//压缩
			if (!CompressData(pData, &nLen, pPixelData, nLen))
				assert(false);

			//加密
			QcPacketPerformer::GetInstance()->EncryptData(pData, nLen, 16);
			packetInfo.m_iEncryFlag = 1;
		}
		else
		{
			memcpy(pData, pPixelData, nLen);
			packetInfo.m_iEncryFlag = -1;
		}
		DestroyBitsData(pPixelData);
	}
	else
	{
		//加密
		if (nLen >= QmMinEncryLen)
		{
			QcPacketPerformer::GetInstance()->EncryptData(pData, nLen, 16);
			packetInfo.m_iEncryFlag = 2;
		}
		else
		{
			packetInfo.m_iEncryFlag = 0;
		}
	}
	packetInfo.m_iLen = nLen;
	return pData;
}