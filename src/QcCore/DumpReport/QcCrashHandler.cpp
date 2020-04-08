#include "pch\pch.hpp"
#include "QcCrashHandler.hpp"
#include "QcVectoredHandler.hpp"
#include "QcStructException.hpp"
#include "QcStackWalker.hpp"
#include "Common\QcHookFunction.hpp"


#ifndef _AddressOfReturnAddress

// Taken from: http://msdn.microsoft.com/en-us/library/s975zw7k(VS.71).aspx
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

// _ReturnAddress and _AddressOfReturnAddress should be prototyped before use 
EXTERNC void * _AddressOfReturnAddress(void);
EXTERNC void * _ReturnAddress(void);
#endif 

struct EXCEPTION_REGISTRATION
{
	EXCEPTION_REGISTRATION* prev;
	FARPROC                 handler;
};
//_except_handler4是编译器生成的函数，__except关键字被编译成 _except_handler4的调用
//throw C++ 异常实际上时调用 _CxxThrowException函数, _CxxThrowException调用RaiseException抛出异常，系统寻找异常堆栈链.
extern "C" int _except_handler4(PEXCEPTION_RECORD, EXCEPTION_REGISTRATION*,PCONTEXT, PEXCEPTION_RECORD);

static QcHookFunction gsHookThrow; //需要声明在gsHookThrow之前
QcCrashHandler QcCrashHandler::gsCrashHandle;

QcCrashHandler::QcCrashHandler()
{
	if (!IsDebuggerPresent())
	{
		SetProcessExceptionHandlers();
		SetThreadExceptionHandlers();

		//gsHookThrow.Init(MakeFunWord(_CxxThrowException), MakeFunWord(&QcCrashHandler::new_ThrowException));
	}
}

void QcCrashHandler::SetProcessExceptionHandlers()
{
	// Install top-level SEH handler
	SetUnhandledExceptionFilter(SehHandler);    

	// Catch pure virtual function calls.
	// Because there is one _purecall_handler for the whole process, 
	// calling this function immediately impacts all threads. The last 
	// caller on any thread sets the handler. 
	// http://msdn.microsoft.com/en-us/library/t296ys27.aspx
	_set_purecall_handler(PureCallHandler);    

	// Catch new operator memory allocation exceptions
	_set_new_handler(NewHandler);

	// Catch invalid parameter exceptions.
	_set_invalid_parameter_handler(InvalidParameterHandler); 

	// Set up C++ signal handlers

	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	// Catch an abnormal program termination
	signal(SIGABRT, SigabrtHandler);  

	// Catch illegal instruction handler
	signal(SIGINT, SigintHandler);     

	// Catch a termination request
	signal(SIGTERM, SigtermHandler);          

}

void QcCrashHandler::SetThreadExceptionHandlers()
{
	_set_se_translator(&QcStructException::SETransFunc);

	// Catch terminate() calls. 
	// In a multithreaded environment, terminate functions are maintained 
	// separately for each thread. Each new thread needs to install its own 
	// terminate function. Thus, each thread is in charge of its own termination handling.
	// http://msdn.microsoft.com/en-us/library/t6fk7h29.aspx
	set_terminate(TerminateHandler);       

	// Catch unexpected() calls.
	// In a multithreaded environment, unexpected functions are maintained 
	// separately for each thread. Each new thread needs to install its own 
	// unexpected function. Thus, each thread is in charge of its own unexpected handling.
	// http://msdn.microsoft.com/en-us/library/h46t5b69.aspx  
	set_unexpected(UnexpectedHandler);    

	//_controlfp_s apply a floating point error

	// Catch a floating point error
	typedef void (*sigh)(int);
	signal(SIGFPE, (sigh)SigfpeHandler);     

	// Catch an illegal instruction
	signal(SIGILL, SigillHandler);     

	// Catch illegal storage access errors
	signal(SIGSEGV, SigsegvHandler);   

}

static void ExitApp()
{
	if (QmApp)
		QmApp->ExitApp(); 
}

// Structured exception handler
LONG WINAPI QcCrashHandler::SehHandler(PEXCEPTION_POINTERS pExceptionPtrs)
{ 
	MakeDumpFile("SehHandler", pExceptionPtrs);
	ExitApp();
	return EXCEPTION_EXECUTE_HANDLER;
}

// CRT terminate() call handler
void __cdecl QcCrashHandler::TerminateHandler()
{
	MakeDumpFile("TerminateHandler");
	ExitApp();   
}

// CRT unexpected() call handler
void __cdecl QcCrashHandler::UnexpectedHandler()
{
	MakeDumpFile("UnexpectedHandler");
	ExitApp();   	    
}

// CRT Pure virtual method call handler
void __cdecl QcCrashHandler::PureCallHandler()
{
	MakeDumpFile("PureCallHandler");
	ExitApp();    
}


// CRT invalid parameter handler
void __cdecl QcCrashHandler::InvalidParameterHandler(
	const wchar_t* expression, 
	const wchar_t* function, 
	const wchar_t* file, 
	unsigned int line, 
	uintptr_t pReserved)
{
	pReserved;
	MakeDumpFile("InvalidParameterHandler");
	ExitApp();
}

// CRT new operator fault handler
int __cdecl QcCrashHandler::NewHandler(size_t)
{
	MakeDumpFile("NewHandler");
	ExitApp();
	return 0;
}

// CRT SIGABRT signal handler
void QcCrashHandler::SigabrtHandler(int)
{
	MakeDumpFile("SigabrtHandler");
	ExitApp(); 
}

// CRT SIGFPE signal handler
void QcCrashHandler::SigfpeHandler(int /*code*/, int subcode)
{
	MakeDumpFile("SigfpeHandler");
	ExitApp();   
}

// CRT sigill signal handler
void QcCrashHandler::SigillHandler(int)
{
	MakeDumpFile("SigillHandler");
	ExitApp(); 
}

// CRT sigint signal handler
void QcCrashHandler::SigintHandler(int)
{
	MakeDumpFile("SigintHandler");
	ExitApp(); 
}

// CRT SIGSEGV signal handler
void QcCrashHandler::SigsegvHandler(int)
{
	MakeDumpFile("SigsegvHandler");
	ExitApp();
}

// CRT SIGTERM signal handler
void QcCrashHandler::SigtermHandler(int)
{
	MakeDumpFile("SigtermHandler");
	ExitApp();
}

//截获C++抛出异常
void QcCrashHandler::new_ThrowException(void* pObject, _ThrowInfo* pInfor)
{
	QcHookHelper hooker(gsHookThrow);
	MakeDumpFile("ThrowException");
	_CxxThrowException(pObject, pInfor);
}

const char* QcCrashHandler::ExceptionCodeToStr(DWORD exceptionCode)
{
	struct ExceptionMap
	{
		DWORD dCode;
		const char* psStr;
	};
	static ExceptionMap exceptionMap[] = 
	{
		{EXCEPTION_ACCESS_VIOLATION, ("读写非法内存")},
		{EXCEPTION_BREAKPOINT, ("断点")},
		{EXCEPTION_DATATYPE_MISALIGNMENT, ("The thread attempted to read or write data that is misaligned on hardware that does not provide alignment. ")},
		{EXCEPTION_SINGLE_STEP, ("A trace trap or other single-instruction mechanism signaled that one instruction has been executed.")},
		{EXCEPTION_ARRAY_BOUNDS_EXCEEDED, ("数字越界")},
		{EXCEPTION_FLT_DENORMAL_OPERAND, ("An operand in a floating-point operation is denormal. ")},
		{EXCEPTION_FLT_DIVIDE_BY_ZERO, ("除0")},
		{EXCEPTION_FLT_INEXACT_RESULT, ("The result of a floating-point operation cannot be represented exactly as a decimal fraction.")},
		{EXCEPTION_FLT_INVALID_OPERATION, ("This exception represents a floating-point exception not included in this list.")},
		{EXCEPTION_FLT_OVERFLOW, ("The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.")},
		{EXCEPTION_FLT_STACK_CHECK, ("The stack overflowed or underflowed as a result of a floating-point operation.")},
		{EXCEPTION_FLT_UNDERFLOW, ("The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.")},
		{EXCEPTION_INT_DIVIDE_BY_ZERO, ("The thread attempted to divide an integer value by an integer divisor of zero.")},
		{EXCEPTION_INT_OVERFLOW, ("The result of an integer operation caused a carry out of the most significant bit of the result.")},
		{EXCEPTION_PRIV_INSTRUCTION, ("The thread attempted to execute an instruction whose operation is not allowed in the current machine mode.")},
		{EXCEPTION_NONCONTINUABLE_EXCEPTION, ("The thread attempted to continue execution after a noncontinuable exception occurred.")},
	};
	for (int i=0; i<sizeof(exceptionMap)/sizeof(ExceptionMap); ++i)
		if (exceptionCode == exceptionMap[i].dCode)
			return exceptionMap[i].psStr;
	return "";
}

int QcCrashHandler::Record( _EXCEPTION_POINTERS* pExceptionInfo, const char* pPreName, DWORD dwThreadId)
{
	char szBuff[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szBuff, MAX_PATH);									
	LPSTR lpszExt = ::PathFindExtensionA(szBuff);
	if (*lpszExt == '.')
		*lpszExt = 0;
	LPSTR lpszName = ::PathFindFileNameA(szBuff);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	//write crash report
	if (pExceptionInfo)
	{
		char szName[MAX_PATH] = {0};
		sprintf_s(szName, MAX_PATH, "%s\\%s_%s_%d-%d-%d_%d-%d-%d-%d.dmp", QmLogPath, pPreName, lpszName, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
		HANDLE lhDumpFile = CreateFileA(szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH ,NULL);
		if (lhDumpFile == NULL ||  lhDumpFile == INVALID_HANDLE_VALUE)
			QmLog("Fail to create dump file");

		MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
		loExceptionInfo.ExceptionPointers = pExceptionInfo;
		loExceptionInfo.ThreadId = dwThreadId == 0 ? GetCurrentThreadId() : dwThreadId;
		loExceptionInfo.ClientPointers = FALSE;
		//THREAD_WRITE_FLAGS
		if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, (MINIDUMP_TYPE)(MiniDumpNormal) , &loExceptionInfo, NULL, NULL))
		{
			LPVOID lpMsgBuf = NULL;
			FormatMessageA( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				0, // Default language
				(LPSTR) &lpMsgBuf,
				0,
				NULL 
				);
			QmLog("Fail to dump stack");
			if (lpMsgBuf)
			{
				QmLog((const char*)lpMsgBuf);
				LocalFree( lpMsgBuf );
			}
		}
		CloseHandle(lhDumpFile);

		//Write Log
		QmLog("DumpLog:%s", pPreName);
		QmLog("[异常类型]:%s", ExceptionCodeToStr(pExceptionInfo->ExceptionRecord->ExceptionCode));
		QmLog("[异常地址]:%p, Eip=%d,Ebp=%d,Esp=%d", 
			pExceptionInfo->ExceptionRecord->ExceptionAddress,
			pExceptionInfo->ContextRecord->Eip, 
			pExceptionInfo->ContextRecord->Ebp,
			pExceptionInfo->ContextRecord->Esp
			);
	}
	QmFlushLog();
	return 1;
}

void MakeDumpFile(const char* pPreName, _EXCEPTION_POINTERS* pExceptionInfo, DWORD dwThreadId)
{
	if (pExceptionInfo == NULL)
	{
		EXCEPTION_RECORD exceptionRecord = {0};
		CONTEXT c = {0};
		HANDLE hThread = GetCurrentThread();
		if (dwThreadId == 0 || dwThreadId == GetCurrentThreadId())
		{
			dwThreadId = GetCurrentThreadId();
			c.ContextFlags = CONTEXT_FULL;
			RtlCaptureContext(&c);
		}
		else
		{
			hThread = OpenThread(SYNCHRONIZE | THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, dwThreadId);
			SuspendThread(hThread);
			c.ContextFlags = CONTEXT_FULL;
			if (!GetThreadContext(hThread, &c))
			{
				QmLogSysError();
			}
		}
		_EXCEPTION_POINTERS exceptionInfo = {&exceptionRecord, &c};
		exceptionRecord.ExceptionAddress = (PVOID)c.Eip;
		QcCrashHandler::Record(&exceptionInfo, pPreName, dwThreadId);
		QcStackWalker::GetInstance()->RecordCallStack("CallStack", NULL, &c, hThread);
	}
	else
	{
		QcCrashHandler::Record(pExceptionInfo, pPreName);
		QcStackWalker::GetInstance()->RecordCallStack(pPreName, NULL, pExceptionInfo->ContextRecord);
	}
}