#pragma once

class QcCrashHandler  
{
	friend void MakeDumpFile(const char* psName, _EXCEPTION_POINTERS* ExceptionInfo, DWORD dwThreadId);
public:
	// Installs C++ exception handlers that function on per-thread basis
	static void SetThreadExceptionHandlers();
private:
    // Sets exception handlers that work on per-process basis
    static void SetProcessExceptionHandlers();


    /* Exception handler functions. */
    static LONG WINAPI SehHandler(PEXCEPTION_POINTERS pExceptionPtrs);
    static void __cdecl TerminateHandler();
    static void __cdecl UnexpectedHandler();

    static void __cdecl PureCallHandler();

    static void __cdecl InvalidParameterHandler(const wchar_t* expression, 
        const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);

    static int __cdecl NewHandler(size_t);

    static void SigabrtHandler(int);
    static void SigfpeHandler(int /*code*/, int subcode);
    static void SigintHandler(int);
    static void SigillHandler(int);
    static void SigsegvHandler(int);
    static void SigtermHandler(int);
	static void new_ThrowException(void* pObject, _ThrowInfo* pInfor);

	static const char* ExceptionCodeToStr(DWORD exceptionCode);
	static int QcCrashHandler::Record( _EXCEPTION_POINTERS* pExceptionInfo, const char* pPreName, DWORD dwThreadId = 0);
private:
	QcCrashHandler();
	static QcCrashHandler gsCrashHandle;
};


