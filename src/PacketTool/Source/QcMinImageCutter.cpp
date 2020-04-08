#include "pch.hpp"
#include "QcMinImageCutter.hpp"

DWORD __stdcall QcMinImageCutter::ThreadFun(LPVOID lpParameter)
{
	QcMinImageCutter* pThis = (QcMinImageCutter*)lpParameter;
#ifdef QmMutilThreadOn
	long index = InterlockedIncrement(&pThis->m_iIndex);
	while(index < pThis->ToExcuteSize())
	{
		pThis->Excute(pThis->m_arOffsetList[index]);
		index = InterlockedIncrement(&pThis->m_iIndex);
	}
#else
	std::vector<QcOffsetInfo>::iterator iter(pThis->m_arOffsetList.begin());
	std::vector<QcOffsetInfo>::iterator endIter(pThis->m_arOffsetList.end());
	for (; iter != endIter; ++iter)
	{
		pThis->Excute(*iter);
	}
	printf("ReadPngTime:%d(s)\n", pThis->m_readPngTime/QfGetCPUFrequency());
	printf("WritePngTime:%d(s)\n", pThis->m_writePngTime/QfGetCPUFrequency());
#endif
	return 0;
}

void QcMinImageCutter::Excute(QcOffsetInfo& infor)
{
	FILETIME writeTime;
	
	bool bDone = false;
	if (QcFileSystem::MatchFile(infor.sName, m_arFilterList))
	{
		QcFileSystem::CopyFile(infor.souceFile, infor.destFile);
		infor.X = 0.0;
		infor.Y = 0.0;
		printf("过滤:%s\n", infor.sName.c_str());
		bDone = false;
	}
	else
	{
		QcFileSystem::GetFileTime(infor.souceFile.c_str(), &writeTime);
		if (CompareFileTime(&writeTime, &infor.lastWriteTime) != 0 || !QcFileSystem::IsExist(infor.destFile.c_str()))
		{
			infor.lastWriteTime = writeTime;
			bDone = true;
		}
	}
	if (bDone)
	{
		if (!Excute(infor.souceFile, infor.destFile, infor.X, infor.Y))
		{
			m_iErrorFlag = 1;
		}
		if (m_iErrorFlag)
		{
			printf("Error:%d\n", m_iErrorFlag);
			getchar();
		}
		printf("裁剪:%s\n", infor.sName.c_str());
	}
}

bool QcMinImageCutter::FillInfoList(const QcString& sSourcePath, const QcString& destPath, bool bSamePath)
{
	int nSourcePathLen = sSourcePath.size();
	QcStringList folderList;
	QcFileSystem::GetSubFolderList(sSourcePath, folderList);
	if (bSamePath)
	{
		QcStringList::iterator iter(folderList.begin());
		for (; iter != folderList.end(); ++iter)
		{
			QcFileSystem::MakePath(destPath + (iter->c_str() + nSourcePathLen));
		}
	}
	else
	{
		QcFileSystem::MakePath(destPath);
	}

	QcStringList fileList;
	QcFileSystem::GetFileList(sSourcePath, &fileList, NULL, QsFileFilter("*.png"));
	m_arOffsetList.resize(fileList.size());

	QcStringList::iterator fileIter(fileList.begin());
	std::vector<QcOffsetInfo>::iterator iter(m_arOffsetList.begin());
	std::vector<QcOffsetInfo>::iterator endIter(m_arOffsetList.end());
	for (; iter != endIter; ++iter, ++fileIter)
	{
		iter->sName = ::PathFindFileNameA(fileIter->c_str());
		iter->souceFile = *fileIter;
		iter->destFile = destPath + (bSamePath ? fileIter->c_str() + nSourcePathLen : iter->sName);
		iter->X = 0.f;
		iter->Y = 0.f;
		iter->lastWriteTime.dwHighDateTime = 0;
		iter->lastWriteTime.dwLowDateTime = 0;
	}

	std::sort(m_arOffsetList.begin(), m_arOffsetList.end(), QcInfoSortor());

	bool bMutil = false;
	std::vector<QcOffsetInfo>::iterator iLastIter;
	iter = m_arOffsetList.begin();
	iLastIter = iter++;
	for (; iter != m_arOffsetList.end(); ++iter)
	{
		if (!(*iLastIter == *iter))
		{
			iLastIter = iter;
		}
		else
		{
			printf("文件重复: %s\n", iLastIter->sName.c_str());
			bMutil = true;
		}
	}
	if (bMutil)
	{
		printf("任意键退出\n");
		getchar();
		return false;
	}
	return true;
}


int QcMinImageCutter::DoExcute(const QcString& path, const QcString& file, const QcStringList& fileFilter, int iAphlaRef, int iAphlaEdgeWidth,bool bSamePath)
{
	printf("开始裁剪\n");
	QcString sSourcePath(path);
	QcFileSystem::UnifyPathName(sSourcePath);
	int nSourcePathLen = sSourcePath.size();
	m_arFilterList = fileFilter;
	m_iAphlaRef = iAphlaRef;
	m_iAphlaEdgeWidth = iAphlaEdgeWidth;
	
	QcString destPath;
	QcString destFile;
	QcFileFinder destFinder;
	QcStringList folderList;
	QcFileSystem::SplitPath(file, destPath, destFile);
	QcFileSystem::UnifyPathName(destPath);
	QcFileSystem::GetSubFolderList(destPath, folderList);
	destFinder.AddSearchPath(destPath);
	destFinder.AddSearchPath(folderList);

	if (!FillInfoList(sSourcePath, destPath, bSamePath))
		return 0;
	
	TiXmlDocument document;
	if (document.LoadFile(file.c_str()))
	{
		TiXmlElement* pRootElement = document.RootElement();
		TiXmlElement* pElment = pRootElement->FirstChildElement();
		QcString tempStr;
		while (pElment)
		{
			const char* psFile = pElment->Attribute("File");
			QcOffsetInfo* pInfo = FindInfo(psFile);
			if (pInfo == NULL)
			{
				QcString file(psFile);
				if (destFinder.FindFile(file))
				{
					QcFileSystem::DeleteFile(file);
				}
			}
			else
			{
				sscanf_s(pElment->Attribute("Offset"), "%f,%f", &pInfo->X, &pInfo->Y);
				const char* psFileTm = pElment->Attribute("FileTm");
				if (psFileTm != NULL && *psFileTm != 0)
				{
					sscanf_s(psFileTm, "%u,%u", &pInfo->lastWriteTime.dwHighDateTime, &pInfo->lastWriteTime.dwLowDateTime);
				}
			}
			pElment = pElment->NextSiblingElement();
		}
	}

#ifdef QmMutilThreadOn
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
	

	TiXmlDocument destDocument;
	TiXmlElement* pRoot = new TiXmlElement("OffsetPointMap");
	std::vector<QcOffsetInfo>::iterator iter(m_arOffsetList.begin());
	std::vector<QcOffsetInfo>::iterator endIter(m_arOffsetList.end());
	char buf[128];
	for (; iter != endIter; ++iter)
	{
		TiXmlElement* pElement = new TiXmlElement("Element");
		pElement->SetAttribute("File", iter->sName.c_str());
		sprintf_s(buf, "%f,%f", iter->X, iter->Y);
		pElement->SetAttribute("Offset", buf);
		sprintf_s(buf, "%u,%u", iter->lastWriteTime.dwHighDateTime, iter->lastWriteTime.dwLowDateTime);
		pElement->SetAttribute("FileTm", buf);
		pRoot->LinkEndChild(pElement);
	}
	destDocument.LinkEndChild(pRoot);
	destDocument.SaveFile(file.c_str());
	return 1;
}


bool QcMinImageCutter::Excute(const QcString& sSourceFile, const QcString& sDestFile, float& offsetX, float& offsetY)
{
	char* pData = NULL;
	unsigned long iWidth = 0;
	unsigned long iHeight = 0;
#ifndef QmMutilThreadOn
	static __int64 pLastReadTime = QfGetCPUTicks();
#endif
#ifdef QmQImageOn
	QImage sourceImage(sSourceFile.c_str());
	pData = (char*)sourceImage.bits();
	iWidth = sourceImage.width();
	iHeight = sourceImage.height();
#else
	if (!ReadImage(pData, iWidth, iHeight, sSourceFile.c_str()))
	{
		assert(false);
		return false;
	}
#endif

#ifndef QmMutilThreadOn
	m_readPngTime += (QfGetCPUTicks() - pLastReadTime);
#endif

	QsARGB* pARGB = (QsARGB*)pData;
	int iSourceWidth = iWidth;
	int iSourceHeight = iHeight;
	int yMin = 0;
	int yMax = 0;
	int xMin = 0;
	int xMax = 0;

	QsARGB* pPixel = pARGB;
	for (int j=0; j<iSourceHeight; ++j)
	{
		for (int i=0; i<iSourceWidth; ++i, ++pPixel)
		{
			if (pPixel->a > m_iAphlaRef)
			{
				yMin = max(j - m_iAphlaEdgeWidth, 0);
				//yMin = j;
				goto Flag1;
			}
		}
	}
Flag1:
	pPixel = pARGB + iSourceWidth * iSourceHeight - 1 ;
	for (int j=iSourceHeight-1; j>=0; --j)
	{
		for (int i=0; i<iSourceWidth; ++i, --pPixel)
		{
			if (pPixel->a > m_iAphlaRef)
			{
				yMax = min(j + m_iAphlaEdgeWidth, iSourceHeight-1);
				//yMax = j;
				goto Flag2;
			}
		}
	}
Flag2:
	for (int i=0; i<iSourceWidth; ++i)
	{
		pPixel = pARGB + yMin * iSourceWidth + i;
		for (int j=yMin; j<yMax; ++j, pPixel += iSourceWidth)
		{
			if (pPixel->a > m_iAphlaRef)
			{
				xMin = max(i-m_iAphlaEdgeWidth, 0);
				//xMin = i;
				goto Flag3;
			}
		}
	}
Flag3:
	for (int i=iSourceWidth-1; i>=0; --i)
	{
		pPixel = pARGB + yMin * iSourceWidth + i;
		for (int j=yMin; j<yMax; ++j, pPixel += iSourceWidth)
		{
			if (pPixel->a > m_iAphlaRef)
			{
				xMax = min(i+m_iAphlaEdgeWidth, iSourceWidth-1);
				//xMax = i;
				goto Flag4;
			}
		}
	}
Flag4:
	int iDestWidth = xMax - xMin + 1;
	int iDestHeight = yMax - yMin + 1;

	if (iDestWidth != iSourceWidth || iDestHeight != iSourceHeight)
	{
		offsetX = (xMax + xMin - iSourceWidth) * 0.5f;
		offsetY = (yMax + yMin - iSourceHeight) * 0.5f;
#ifndef QmMutilThreadOn
		static __int64 pWriteLastTime = QfGetCPUTicks();
#endif
#ifdef QmQImageOn
		QImage destImage(iDestWidth, iDestHeight, sourceImage.format());

		int iLineBit = destImage.bytesPerLine();
		int xOffset = xMin * (iLineBit/iDestWidth);
		int iSource = yMin;
		int iDest = 0;
		while(iDest < iDestHeight)
		{
			QRgb* pRgb = (QRgb*)destImage.constScanLine(iDest);
			memcpy(pRgb, sourceImage.constScanLine(iSource) + xOffset, iLineBit);
			++iSource;
			++iDest;
		}
		destImage.save(sDestFile.c_str());
#else
		WritePngImage(pData, iWidth, xMin, yMin, iDestWidth, iDestHeight, sDestFile.c_str());
#endif
#ifndef QmMutilThreadOn
		m_writePngTime += (QfGetCPUTicks() - pWriteLastTime);
#endif
	}
	else
	{
		offsetX = 0.f;
		offsetY = 0.f;
		QcFileSystem::CopyFile(sSourceFile.c_str(), sDestFile);
	}
#ifndef QmQImageOn
	DestroyBitsData(pData);
#endif
	return true;
}