#pragma once

class QcVectoredHandler
{
public:
	struct Helper
	{
	private:
		QmDisableCopyAssignment(Helper);
	public:
		Helper(QcVectoredHandler& handle) : m_handle(handle)
		{
			m_handle.Hook(false);	
		}
		~Helper()
		{
			m_handle.Hook(true);
		}
		QcVectoredHandler& m_handle;
	};

	QcVectoredHandler() : m_pExceptionHandler(NULL) {}
	~QcVectoredHandler()
	{
		Hook(false);
	}
	void Hook(bool bHook)
	{
		if (bHook)
			m_pExceptionHandler = AddVectoredExceptionHandler(1, VectoredHandler);
		else if (m_pExceptionHandler)
			RemoveVectoredExceptionHandler(m_pExceptionHandler);
	}
	static LONG WINAPI VectoredHandler(struct _EXCEPTION_POINTERS *ExceptionInfo)
	{
		MakeDumpFile("VectoredHandler", ExceptionInfo);
		return EXCEPTION_CONTINUE_SEARCH;
	}
protected:
	PVOID m_pExceptionHandler;
};