#include ".\..\QcCompileConfig.hpp"
#ifndef QmEfficiencyProfilerOn
#define QmEfficiencyProfilerOn
#endif
#include <windows.h>
#include <process.h>
#include <tchar.h>
#include <iosfwd>
#include <errno.h>
#include <dbghelp.h>
#include <shlwapi.h>
#include <atlconv.h>
#include <time.h>
#include <stdio.h>
#include <Psapi.h>
#include <signal.h>
#include <fcntl.h>
#include <io.h>

#include <iosfwd>
#include <sstream>
#include <fstream>
#include <strstream>
#include <hash_map>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <deque>
#include <functional>
#include <algorithm>
using namespace std;

#include "QcCore.hpp"
#pragma comment(lib, "QcCore.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib,"Psapi.lib")

#define QmTestCount (10000 * 5)
struct QsNoScalarType
{
	int iValue1;
	int iValue2;
	int iValue3;
	int iValue4;
};
struct QsScalarType
{
	int iValue1;
	int iValue2;
	int iValue3;
	int iValue4;
};

//
int TestNoScalarType()
{
	std::vector<QsNoScalarType> garScalarType;
	QsNoScalarType inputType;
	{
		QmProfileFragment(Case1:push_back);
		for (int i=0; i<QmTestCount; ++i)
		{
			inputType.iValue1 = i;
			inputType.iValue2 = i * 2;
			inputType.iValue3 = i * 3;
			inputType.iValue4 = i * 4;
			garScalarType.push_back(inputType);
		}
	}
	{
		QmProfileFragment(Case1:erase);
		while(garScalarType.size())
		{
			garScalarType.erase(garScalarType.begin());
		}
	}
	{
		QmProfileFragment(Case1:insert);
		for (int i=0; i<QmTestCount; ++i)
		{
			inputType.iValue1 = i;
			inputType.iValue2 = i * 2;
			inputType.iValue3 = i * 3;
			inputType.iValue4 = i * 4;
			garScalarType.insert(garScalarType.begin(), inputType);
		}
	}

	int iSum = 0;
	std::vector<QsNoScalarType>::iterator iter = garScalarType.begin();
	std::vector<QsNoScalarType>::iterator endIter = garScalarType.end();
	for (; iter != endIter; ++iter)
	{
		iSum += iter->iValue1 + iter->iValue2 + iter->iValue3 + iter->iValue4; 
	}
	return iSum;
}

//RegisterScalarType
QmRegisterScalarType(QsScalarType);
int TestScalarType()
{
	std::vector<QsScalarType> garScalarType;
	QsScalarType inputType;
	{
		QmProfileFragment(Case2:push_back);
		for (int i=0; i<QmTestCount; ++i)
		{
			inputType.iValue1 = i;
			inputType.iValue2 = i * 2;
			inputType.iValue3 = i * 3;
			inputType.iValue4 = i * 4;
			garScalarType.push_back(inputType);
		}
	}
	{
		QmProfileFragment(Case2:erase);
		while(garScalarType.size())
		{
			garScalarType.erase(garScalarType.begin());
		}
	}
	{
		QmProfileFragment(Case2:insert);
		for (int i=0; i<QmTestCount; ++i)
		{
			inputType.iValue1 = i;
			inputType.iValue2 = i * 2;
			inputType.iValue3 = i * 3;
			inputType.iValue4 = i * 4;
			garScalarType.insert(garScalarType.begin(), inputType);
		}
	}

	int iSum = 0;
	std::vector<QsScalarType>::iterator iter = garScalarType.begin();
	std::vector<QsScalarType>::iterator endIter = garScalarType.end();
	for (; iter != endIter; ++iter)
	{
		iSum += iter->iValue1 + iter->iValue2 + iter->iValue3 + iter->iValue4; 
	}
	return iSum;
}

//array
int TestArrayNoScalarType()
{
	QcArray<QsNoScalarType> garScalarType;
	QsNoScalarType inputType;
	{
		QmProfileFragment(Case3:push_back);
		for (int i=0; i<QmTestCount; ++i)
		{
			inputType.iValue1 = i;
			inputType.iValue2 = i * 2;
			inputType.iValue3 = i * 3;
			inputType.iValue4 = i * 4;
			garScalarType.push_back(inputType);
		}
	}
	{
		QmProfileFragment(Case3:erase);
		while(garScalarType.size())
		{
			garScalarType.erase(garScalarType.begin());
		}
	}
	{
		QmProfileFragment(Case3:insert);
		for (int i=0; i<QmTestCount; ++i)
		{
			inputType.iValue1 = i;
			inputType.iValue2 = i * 2;
			inputType.iValue3 = i * 3;
			inputType.iValue4 = i * 4;
			garScalarType.insert(garScalarType.begin(), inputType);
		}
	}

	int iSum = 0;
	QcArray<QsNoScalarType>::iterator iter = garScalarType.begin();
	QcArray<QsNoScalarType>::iterator endIter = garScalarType.end();
	for (; iter != endIter; ++iter)
	{
		iSum += iter->iValue1 + iter->iValue2 + iter->iValue3 + iter->iValue4; 
	}
	return iSum;
}

//encrypt
void TestEncrypt()
{
	char encryptBuf[] = "1djsfklajsdf43907657567klfsdf131646][][_=dq1`dsfkjlsadflksdjflk!@#$%^&*()_+<>:fsdfjlksdjflksdjflkjsdlkfjlksdf";
	char encryptDest[QmCountOf(encryptBuf)];
	u32 key[4] = {0xeadf2469, 0x45ed0278, 0x12345678, 0x23456789};

	QcSharedPtr<QiCrypt> enCrypt[2];
	enCrypt[0] = (new QcAESCrypt());
	enCrypt[1] = (new QcTEACrypt());
	for (int i=0; i<2; ++i)
	{
		printf("Begin Test %s\n", i==0 ? "AES" : "TEA");
		QcSharedPtr<QiCrypt> pEn = enCrypt[i];

		pEn->SetKey((const char*)key, sizeof(key));
		//normal encrypt
		memcpy(encryptDest, encryptBuf, QmCountOf(encryptBuf));
		pEn->Encrypt(encryptDest, QmCountOf(encryptBuf));
		QmAssert(memcmp(encryptBuf, encryptDest, QmCountOf(encryptBuf)) != 0);
		pEn->Decrypt(encryptDest, QmCountOf(encryptBuf));
		QmAssert(memcmp(encryptBuf, encryptDest, QmCountOf(encryptBuf)) == 0);

		//random encrypt
		int iDestSize = pEn->CalRandomEnBufSize(QmCountOf(encryptBuf));
		QcArray<char> sBuf(encryptBuf, QmCountOf(encryptBuf));
		QcArray<char> sBuf2(encryptBuf, QmCountOf(encryptBuf));
		sBuf.resize(iDestSize);
		sBuf2.resize(iDestSize);
		pEn->RandomEncrypt(sBuf.pointer(), iDestSize, QmCountOf(encryptBuf));
		pEn->RandomEncrypt(sBuf2.pointer(), iDestSize, QmCountOf(encryptBuf));
		QmAssert(memcmp(sBuf.pointer(), sBuf2.pointer(), sBuf.size()) != 0);

		int nRawSize1 = 0;
		int nRawSize2 = 0;
		pEn->RandomDecrypt(sBuf.pointer(), iDestSize, nRawSize1);
		pEn->RandomDecrypt(sBuf2.pointer(), iDestSize, nRawSize2);
		QmAssert(nRawSize2 == nRawSize1 && nRawSize1 != 0);
		QmAssert(memcmp(sBuf.pointer(), sBuf2.pointer(), nRawSize1) == 0);

		printf("Finished Test %s\n", i==0 ? "AES" : "TEA");
	}
	printf("pass Encrypt\n\n");
}

//Hook
static QcHookFunction gsHookHeapAlloc;
LPVOID WINAPI HookHeapAlloc(__in HANDLE hHeap,__in DWORD dwFlags,__in SIZE_T dwBytes)
{
	QcHookHelper hookerHelper(gsHookHeapAlloc);
	printf("Hook Alloc %d(B)\n", dwBytes);
	return HeapAlloc(hHeap, dwFlags, dwBytes);
}
void TestHook()
{
	gsHookHeapAlloc.Init(MakeFunWord(HeapAlloc), MakeFunWord(HookHeapAlloc));
	int iAllocSize = 1024 * 1024 * 10;
	char* p = new char[iAllocSize];
	printf("Alloc %d(B)\n", iAllocSize);
	delete [] p;

	gsHookHeapAlloc.Hook(0);
	printf("Stop hook Alloc\n");
	p = new char[iAllocSize];
	printf("Alloc %d(B)\n", iAllocSize);
	delete [] p;

	printf("pass Hook\n\n\n");
}

//StrToValue
void TestStrToValue()
{
	printf("Begin TestStrToValue\n");

	struct QsTemp
	{
		QsTemp(){}
		QsTemp(const char* psValue)
		{
			sscanf_s(psValue, "%d,%f,%lf", &iInt, &f, &df);
		}
		int iInt;
		f32 f;
		f64 df;
	};
	QsTemp test;
	test = QcStrToValue("100, 200, 300");
	QmAssert(test.iInt == 100 && abs(test.f - 200.f) < 0.000001f && abs(test.df - 300) < 0.000001f);

	test.iInt = QcStrToValue("200");
	test.f = QcStrToValue("300");
	test.df = QcStrToValue("400");
	QmAssert(test.iInt == 200 && abs(test.f - 300.f) < 0.000001f && abs(test.df - 400) < 0.000001f);

	printf("pass TestStrToValue\n\n\n");
}

//Dump
int MakeDumpWhenZero(int iNum)
{
	if (iNum == 0)
	{
		MakeDumpFile("Dump");
		return 1;
	}
	return MakeDumpWhenZero(iNum - 1) * iNum;
}

int TestDump()
{
	int iNum = 20;
	return MakeDumpWhenZero(iNum);
}

//XML
struct QsPoint
{
	QsPoint() : x(0),y(0) {}
	int x;
	int y;
};
QmStrToValueTemplateFun2(QsPoint, psStr)
{
	QsPoint ret;
	sscanf_s(psStr, "%d,%d", &ret.x, &ret.y);
	return ret; 
}
struct QsOffsetPtInfo
{
	char sName[64];
	QsPoint pt;
};
void TestXML()
{
	printf("Begin XML Test\n");
	QcArray<QsOffsetPtInfo> infoList;
	QsOffsetPtInfo item;
	QcXmlLoader loader;
	do 
	{
		QcXmlNodeIterator rootNode = loader.LoadFromFile("offsetPoint.xml");
		if (!rootNode)
			break;
		QcXmlNodeIterator firstTopNode = rootNode.FirstNode("OffsetPointMap");
		if (!firstTopNode)
			break;

		QsPoint defalutPt;
		QcXmlNodeIterator itemNode = firstTopNode.FirstNode();
		while(itemNode)
		{
			item.pt = itemNode.Attribute("Offset", defalutPt);
			strcpy_s(item.sName, itemNode.Attribute("File"));
			infoList.push_back(item);

			itemNode = itemNode.NextSibling();
		}
	} while (0,0);

	for (int i=0; i<(int)infoList.size(); ++i)
	{
		printf("%s[%d,%d]\n", infoList[i].sName, infoList[i].pt.x, infoList[i].pt.y);
	}
	printf("pass XML Test\n\n");
}

void TestFileSystem()
{
	printf("begin FileSystem\n");
	char currentPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, currentPath);
	QcString destMediePath(currentPath);
	QcString destMediePath2(currentPath);
	QcString destMediePath3(currentPath);
	destMediePath2 += "\\Media2";
	destMediePath3 += "\\Media3";
	QcFileSystem::MakePath(destMediePath2);
	QcFileSystem::MakePath(destMediePath3);

	QcStringList fileList;
	QcFileSystem::GetFileList(destMediePath + "\\Media", fileList);
	{
		QcFilePacker packet(destMediePath + "\\Pak.zip");
		packet.PackFile(fileList);
	}

	{
		//读取文件内容
		QcFilePacker packet(destMediePath + "\\Pak.zip");
		QcStringList::iterator iter(fileList.begin());
		QcStringList::iterator endIter(fileList.end());
		for (; iter != endIter; ++iter)
		{
			QcPackFileHandle fileHandle = packet.FindFile(PathFindFileNameA(iter->c_str()));
			QmAssert(fileHandle != QmInvaidHandle);

			u32 iPos = 0;
			u32 iLen = 0;
			QcString newFileName = destMediePath2 + "\\new_";
			newFileName += packet.GetPackInfo(fileHandle, iPos, iLen);
			QcArray<char> buf(iLen, true);
			packet.ReadData(iPos, iLen, buf.pointer());

			QcFileBuf fileBuf;
			fileBuf.readFile(iter->c_str());
			QmAssert(buf.size() == fileBuf.size());
			QmAssert(memcmp(buf.pointer(), fileBuf.pointer(), buf.size()) == 0);

			QcWinFile file;
			file.Open(newFileName.c_str(), std::ios::out);
			file.Write(buf.pointer(), buf.size());
		}
	}
	{
		fileList.clear();
		QcFileSystem::GetFileList(destMediePath2, fileList);
		QcFilePacker packet(destMediePath + "\\Pak.zip");
		packet.PackFile(fileList);

		//便利文件包
		QcPackFileHandle packHandle = packet.FirstFile();
		while(packHandle != QmInvaidHandle)
		{
			u32 iPos = 0;
			u32 iLen = 0;
			QcString newFileName = destMediePath3 + "\\new_";
			newFileName += packet.GetPackInfo(packHandle, iPos, iLen);
			QcArray<char> buf(iLen, true);
			packet.ReadData(iPos, iLen, buf.pointer());

			QcWinFile file;
			file.Open(newFileName.c_str(), std::ios::out);
			file.Write(buf.pointer(), buf.size());

			packHandle = packet.NextFile(packHandle);
		}
	}
	printf("pass FileSystem \n\n");
}


int main()
{
	TestXML();
	TestFileSystem();
	TestHook();
	TestEncrypt();
	TestStrToValue();

	SetThreadAffinityMask(GetCurrentThread(), 0x00000002);
	printf("Begin Profiler Test\n");
	QmProfileOn();
	int iSum1 = TestNoScalarType();				
	int iSum2 = TestScalarType();				
	int iSum3 = TestArrayNoScalarType();	
	QmAssert(iSum1 == iSum2);
	QmAssert(iSum3 == iSum2);
	QmProfileOff();
	QmPrintData(NULL);
	printf("End Profiler Test\n");

	TestDump();

	system("pause");
}