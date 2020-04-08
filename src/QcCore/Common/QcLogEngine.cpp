#include "Pch\Pch.hpp"
#include "QcLogEngine.hpp"
#include "Thread\QcLock.hpp"
#include "FileSystem\QcFileSystem.hpp"

static volatile LONG LogState = 0;

struct QsLoopChecker
{
	DWORD dwThreadId;
	DWORD iLastTm;
	DWORD tDestTime;
};
static QcCriticalSection gsCs;
static std::map<DWORD, QsLoopChecker> gsLoopChecker;

void QfCheckDeadLoop(int nSecs)
{
	if (IsDebuggerPresent())
		return;
	QmLocker(gsCs);
	DWORD threadId = GetCurrentThreadId();
	QsLoopChecker& checkInfor = gsLoopChecker[threadId];
	checkInfor.iLastTm = GetTickCount();
	checkInfor.dwThreadId = threadId;
	checkInfor.tDestTime = checkInfor.iLastTm +  nSecs * 1000;//SpanTime(GetLocalTime64(), 0, 0, 0, nSecs);
}

void QfCheckDeadLoopPtr(DWORD threadId)
{
	std::vector<DWORD> dumpIdList;
	{
		QmLocker(gsCs);
		std::map<DWORD, QsLoopChecker>::iterator iter(gsLoopChecker.begin());
		std::map<DWORD, QsLoopChecker>::iterator endIter(gsLoopChecker.end());
		for (; iter !=endIter; ++iter)
		{
			if (iter->second.dwThreadId != 0 && threadId != iter->second.dwThreadId && GetTickCount() > iter->second.tDestTime)
			{
				dumpIdList.push_back(iter->second.dwThreadId);
				QmLog("Loop Time:[%d,%d,%d]", iter->second.iLastTm, iter->second.tDestTime, GetTickCount());
				iter->second.dwThreadId = 0;
			}
		}
	}
	std::vector<DWORD>::iterator iter(dumpIdList.begin());
	std::vector<DWORD>::iterator endIter(dumpIdList.end());
	for (; iter != endIter; ++iter)
	{
		MakeDumpFile("Loop", NULL, *iter);
	}
	if (dumpIdList.size() && QmApp)
	{
		QmApp->ExitApp();
	}
}

QcLogEngine::QcLogEngine() 
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	char sFileName[MAX_PATH];
	sprintf_s(sFileName, MAX_PATH, "%s\\log_%d-%d-%d.txt", QmLogPath, sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	m_sFileName = sFileName;

	if (Create(true))
		push("日志系统启动...\n");
	else
		push("加载日志系统失败\n");

	InterlockedCompareExchange(&LogState, 1, 0);
}

QcLogEngine::~QcLogEngine()
{
	Flush();
	InterlockedCompareExchange(&LogState, 2, 1);
}

bool QcLogEngine::OnIdle(u32 nIdle)
{
	QfCheckDeadLoopPtr(GetCurrentThreadId());
	FlushBuf();
	return false;
}

int QcLogEngine::Run()
{
	int iRet = QcThread::Run();
	push("End.............................................................................");
	Flush();
	return iRet;
}

void QcLogEngine::push(const char* psStr)
{
	{
		QmLocker(m_cs);
		m_dequeWriteString.push_back(psStr);
	}
}

void QcLogEngine::FlushBuf(int nCount)
{
	if (m_dequeWriteString.size())
	{
		QcString strBuf;
		{
			QmLocker(m_cs);
			if (nCount == -1)
				nCount = m_dequeWriteString.size();
			else 
				nCount = min(nCount, (int)m_dequeWriteString.size());

			while(nCount > 0)
			{
				strBuf += m_dequeWriteString.front();
				strBuf += "\n";
				m_dequeWriteString.pop_front();
				--nCount;
			}
		}
		if (strBuf.c_str())
		{
			QmLocker(m_fileCs);
			QcFile file;
			file.Open(m_sFileName.c_str(), std::ios::app);
			file.Write(strBuf.c_str(), strBuf.size());
			file.Close();
		}
	}
}
void QcLogEngine::Flush()
{
	FlushBuf(-1);
}

void WriteLog(const char* psFile, int iLine, const wchar_t* sFormat, ...)
{
	{
		va_list arglist;
		if (sFormat == NULL || sFormat[0] == 0)
		{
			return;
		}

		char sBuf1[QmFormatBuffer];
		sprintf_s(sBuf1, QmFormatBuffer, "%s:%d", psFile, iLine);

		wchar_t sBuf2[QmFormatBuffer];
		va_start(arglist, sFormat);
		_vsnwprintf_s(sBuf2, QmFormatBuffer, QmFormatBuffer, sFormat, arglist);
		va_end(arglist);

		GetInstance<QcLogEngine>()->push(sBuf1);
		USES_CONVERSION;
		GetInstance<QcLogEngine>()->push(W2A(sBuf2));
	}
}

void WriteLog(const char* psFile, int iLine, const char* sFormat, ...)
{
	if (LogState == 1)
	{
		va_list arglist;
		if (sFormat == NULL || sFormat[0] == 0)
		{
			return;
		}

		char sBuf1[QmFormatBuffer] = {0};
		sprintf_s(sBuf1, QmFormatBuffer, "%s:%d", psFile, iLine);

		char sBuf2[QmFormatBuffer] = {0};
		va_start(arglist, sFormat);
		vsnprintf_s(sBuf2, QmFormatBuffer, QmFormatBuffer, sFormat, arglist);
		va_end(arglist);

		GetInstance<QcLogEngine>()->push(sBuf1);
		GetInstance<QcLogEngine>()->push(sBuf2);
	}
}

void QfLogSysError(const char* psFile, int iLine)
{
	DWORD errorCode = GetLastError();
	if (errorCode != 0 && LogState == 1)
	{
		char sBuf1[QmFormatBuffer] = {0};
		LPVOID lpMsgBuf;
		FormatMessageA( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorCode,
			0, // Default language
			(LPSTR) &lpMsgBuf,
			0,
			NULL 
			);
		sprintf_s(sBuf1, "[%s:%d]:%s", psFile, iLine, lpMsgBuf);
		LocalFree( lpMsgBuf );

		GetInstance<QcLogEngine>()->push(sBuf1);
	}
}

void FlushLog()
{
	if (LogState == 1)
	{
		GetInstance<QcLogEngine>()->Flush();
	}
}

void QfClearLog()
{
	QcFileSystem::DeleteDir(QmLogPath);
}
