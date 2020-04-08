#include "pch\pch.hpp"
#include <process.h>
#include "QcThread.hpp"
#include "DumpReport\QcCrashHandler.hpp"
#include "DumpReport\QcStructException.hpp"
#include "Common\QcSharedPtr.hpp"
#include "QcLock.hpp"
#include "QcCriticalSection.hpp"

typedef QcSharedPtr<QcThread> QcThreadAutoPtr;
typedef std::set<QcThreadAutoPtr> QcThreadSet;
typedef QcThreadSet::iterator QcThreadSetIter;
typedef QcThreadSet::const_iterator QcThreadSetConstIter;

static QcThreadSet gsThreadSet;
static QcCriticalSection gsCs(10);

void Run2(QcThread* pThread)
{
	__try
	{
		pThread->Run();
	}
	__except(MakeDumpFile("ThreadCrash", GetExceptionInformation()), EXCEPTION_EXECUTE_HANDLER)
	{
		if (QmApp)
			QmApp->ExitApp();
		else
			ExitProcess(1);
	}
}

unsigned __stdcall QcThread::ThreadFunction(void* pThis)
{
	QcCrashHandler::SetThreadExceptionHandlers();

	//force the system to create the message queue. 
	MSG msg;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	QcThread* pThread = static_cast<QcThread*>(pThis);
	QcSharedPtr<QcThread> threadObj(pThread);
	InterlockedExchange(&pThread->m_uThreadId, GetCurrentThreadId());

	Run2(pThread);

	QmOutputString("end Thread:%d\n", threadObj->m_uThreadId);
	threadObj->CloseHandle();
	InterlockedExchange(&threadObj->m_uThreadId, 0);

	QmLocker(gsCs);
	gsThreadSet.erase(threadObj);

	return 0;
}

void QcThread::ShutDownAllThread()
{
	QcThreadSetIter iter(gsThreadSet.begin());
	std::vector<QcThreadAutoPtr> threadList;
	for (; iter != gsThreadSet.end(); ++iter)
		threadList.push_back(*iter);

	std::vector<QcThreadAutoPtr>::iterator it(threadList.begin());
	for (; it != threadList.end(); ++it)
	{
		(*it)->Close();
	}
}

QcThread::QcThread()
: m_hHandle(NULL)
, m_uThreadId(0)
{
}

QcThread::~QcThread()
{
	Close();
}

bool QcThread::Create(bool bSuspend)
{
	if (m_hHandle == NULL && m_uThreadId == 0)
	{
		unsigned nThreadId = 0;
		m_hHandle = (HANDLE)_beginthreadex(NULL, 0, &QcThread::ThreadFunction, this, bSuspend ? CREATE_SUSPENDED : 0, &nThreadId);
		QmOutputString("Begin Thread:%d\n", nThreadId);
		QmLocker(gsCs);
		gsThreadSet.insert(QcSharedPtr<QcThread>(this));
	}
	return true;
}

void QcThread::CloseHandle()
{
	if (m_hHandle)
	{
		::CloseHandle(m_hHandle);
		m_hHandle = NULL;
	}
}

int	QcThread::Suspend()
{
	if (m_hHandle)
		SuspendThread(m_hHandle);
	return 0;
}
int QcThread::Resume()
{
	if (m_hHandle)
		return ResumeThread(m_hHandle);
	return 0;
}

int QcThread::Close()
{
	if (m_hHandle)
	{
		//唤醒挂起的线程。
		while(Resume() > 0);
		
		InterlockedExchange(&m_uThreadId, 0);

		//等待线程结束
		DWORD dwWaitResult = WaitThreadEnd(1000);
		if(dwWaitResult == WAIT_TIMEOUT)
		{   
			TerminateThread(m_hHandle, STILL_ACTIVE);
		}
		CloseHandle();
	}
	return 0;
}

void QcThread::PostMessage(UINT idMsg,WPARAM wParam,LPARAM lParam)
{
	if (m_hHandle)
	{
		//线程创建了，但是没有唤醒
		if (m_uThreadId == 0)
			while(Resume() > 0);
		PostThreadMessage(m_uThreadId, idMsg, wParam, lParam);
	}
}

int QcThread::Run()
{
	// for tracking the idle time state
	u32 nIdle = 0;
	MSG msg;

	// acquire and dispatch messages until a WM_QUIT message is received.
	for (;;)
	{
		// phase1: check to see if we can do idle work
		BOOL bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0;
		if (bGotMsg)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			QmCheckDeadLoop();
			while (nIdle < 10)
			{
				// call OnIdle while in bIdle state
				if (!OnIdle(++nIdle))
					break;
			}
			::Sleep(20);
		}

		if (m_uThreadId == 0 || msg.message == WM_QUIT)
			break;

		nIdle = 0;
	}
	return 0;
}

DWORD QcThread::WaitThreadEnd(DWORD dwMilliseconds)
{
	return WaitForSingleObject(m_hHandle, dwMilliseconds);
}