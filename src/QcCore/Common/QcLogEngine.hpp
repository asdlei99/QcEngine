#pragma once

#include "Thread/QcThread.hpp"
#include "Thread/QcCriticalSection.hpp"
#include "Common/QcSingleton.hpp"
#include <deque>
#include <string>

class QcLogEngine : public QcThread, public QcSingleton<QcLogEngine>
{
	friend void QfLogString(const QcString& str);
	friend void QfLogSysError(const char* psFile, int iLine);
	friend void WriteLog(const char* psFile, int iLine, const wchar_t* sFormat, ...);
	friend void WriteLog(const char* psFile, int iLine, const char* sFormat, ...);
	friend void FlushLog();
	template<typename T> friend class QcSingleton;
private:
	QcLogEngine();
	~QcLogEngine();

	virtual bool OnIdle(u32 nIdle);
	virtual int Run();
	void FlushBuf(int nCount = 10);
	void push(const char* psStr);
	void Flush();
private:
	std::string m_sFileName;
	std::deque<std::string>	m_dequeWriteString;
	QcCriticalSection	m_cs;
	QcCriticalSection	m_fileCs;
};

