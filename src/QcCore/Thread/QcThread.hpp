#pragma once
#include "Common\QcObject.hpp"

class QcThread : public QcObject
{
	static unsigned __stdcall ThreadFunction(void* pThis);
public:
	QcThread(void);
	virtual ~QcThread(void);

	static void ShutDownAllThread();
	bool Create(bool bSuspend = true);
	int	Suspend();
	int Resume();
	int Close();
	void PostMessage(UINT idMsg,WPARAM wParam=0,LPARAM lParam=0);
	virtual int Run();
	virtual bool OnIdle(u32) = 0; //return true to continue OnIdle; false to back to the main loop;
	virtual void HandleMessage(UINT idMsg,WPARAM wParam=0,LPARAM lParam=0){};
	DWORD WaitThreadEnd(DWORD dwMilliseconds = INFINITE);
private:
	void CloseHandle();	
private:
	HANDLE m_hHandle;
	LONG volatile m_uThreadId;
};

