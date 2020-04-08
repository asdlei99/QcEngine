#pragma once

#pragma warning(disable : 4100)

#define QmClassFactoryInstance GetInstance<QcClassFactory>()
#define QmFormatBuffer (4096)
#define QmArrayFind(array, value) (std::find(array, array + QmCountOf(array), value) - array)
#define QmArrayInit(array, value) do {for (int i=0; i<QmCountOf(array); ++i) array[i] = value;} while(0,0)
#define QmSafeDelete(p) do {if (p){ delete p; p=NULL;} } while(0,0)
#define QmSafeDeleteArray(p) do {if (p){ delete [] p; p=NULL;} } while(0,0)
#define QmThreadData __declspec(thread)

/* Define offsetof macro */
#ifdef __cplusplus
#ifdef  _WIN64
#define QmOffsetof(s,m)   (size_t)( (ptrdiff_t)&reinterpret_cast<const volatile char&>((((s *)0)->m)) )
#else
#define QmOffsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
#endif
#else
#ifdef  _WIN64
#define QmOffsetof(s,m)   (size_t)( (ptrdiff_t)&(((s *)0)->m) )
#else
#define QmOffsetof(s,m)   (size_t)&(((s *)0)->m)
#endif
#endif	/* __cplusplus */

/* Define QmCountOf macro */
#define QmCountOf(ar) (sizeof(ar)/sizeof(ar[0]))

//register scalar
#define QmRegisterScalarType(Type) \
template<>\
struct _Ptr_cat_helper<Type, Type>\
{\
	typedef _Scalar_ptr_iterator_tag _Type;\
}

//macor str
#define _QmMakeMacorStr(macor) #macor
#define QmMakeMacorStr(macor) _QmMakeMacorStr(macor)

//unique name.
#define __QmUniqueVarName(name, line) name##line
#define _QmUniqueVarName(name, line) __QmUniqueVarName(name, line)
#define QmUniqueVarName _QmUniqueVarName(defaultName, __LINE__)

//Align
#define QmAlignSize(size, align) ((size+align-1)/align * align)

//Locker
#define QmLocker(cs) QcLock QmUniqueVarName(cs)

//AutoRevert
#define QmAutoRevert(type, variable) QcAutoRevert<type> QmUniqueVarName(variable)
#define QmAutoRevertInit(type, variable, initValue) QcAutoRevert<type> QmUniqueVarName(variable,initValue)

//run once
#define __QmRunOnce(line, Fun, ...) \
static volatile long giTemp##line = 0;\
if (InterlockedCompareExchange(&giTemp##line, 1, 0) == 0)\
{\
	Fun(__VA_ARGS__);\
	giTemp##line = 2;\
}\
else\
{\
	while(giTemp##line != 2) SwitchToThread();\
}
#define _QmRunOnce(line, Fun, ...) __QmRunOnce(line, Fun, __VA_ARGS__)
#define QmRunOnce(Fun,...) _QmRunOnce(__LINE__, Fun, __VA_ARGS__)

//run before main
#define __QmRunBeforeMain(line, Fun, ...) static const bool QmUniqueVarName = (Fun(__VA_ARGS__),1) ? true : false
#define _QmRunBeforeMain(line, Fun, ...) __QmRunBeforeMain(__LINE__, Fun, __VA_ARGS__)
#define QmRunBeforeMain(Fun,...) _QmRunBeforeMain(__LINE__, Fun, __VA_ARGS__)

//Thread safe static obj
#define __QmThreadSafeStaticObj(ret, type, line, ...)			\
static type* gpTemp##line = NULL;								\
if (gpTemp##line == NULL)										\
{																\
	static volatile long giTemp##line = 0;						\
	if (InterlockedCompareExchange(&giTemp##line, 1, 0) == 0)	\
	{															\
		static type gTemp##line;								\
		gpTemp##line = &gTemp##line;							\
		giTemp##line = 2;										\
	}															\
	else														\
	{															\
		while(giTemp##line != 2) SwitchToThread();				\
	}															\
}																\
ret = gpTemp##line;
#define _QmThreadSafeStaticObj(ret, type, line, ...) __QmThreadSafeStaticObj(ret, type, line, __VA_ARGS__)
#define QmThreadSafeStaticObj(ret, type, ...) _QmThreadSafeStaticObj(ret, type, __LINE__, __VA_ARGS__)


//Disable CopyAssignment
#define QmDisableCopyAssignment(name) \
private:\
	name(const name&);\
	name& operator=(const name&);

//force inline
#ifndef REALINLINE
#ifdef _MSC_VER
#define REALINLINE __forceinline
#else
#define REALINLINE inline
#endif
#endif

//Assert
#ifdef QmAssertOn

#if !(defined(DEBUG) || defined(_DEBUG))
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
	_CRTIMP void __cdecl _wassert(_In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line);
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif

#define QmAssert(c) (void)( (!!(c)) || (QmLog("Assertion:%s", #c), IsDebuggerPresent() ? _wassert(_CRT_WIDE(#c), _CRT_WIDE(__FILE__), __LINE__) : 0 , 0) )
#define QmAssert2(c) QmAssert(c)
#else
#define QmAssert(c) ((void)0)
#define QmAssert2(c) (c)
#endif

//
#if (_MSC_VER >= 1600)
#define QmStatic_Assert(c) static_assert(c, #c)
#elif defined(QmBoostOn)
#define QmStatic_Assert(c) BOOST_STATIC_ASSERT(c)
#endif

//Log
#ifdef QmLogOn
extern void QfLogSysError(const char* psFile, int iLine);
extern void WriteLog(const char* psFile, int iLine, const char *sFormat, ...);
extern void WriteLog(const char* psFile, int iLine, const wchar_t *sFormat, ...);
extern void FlushLog();
extern void QfClearLog();
#define QmLog(SFORMAT, ...) WriteLog(__FILE__,__LINE__, SFORMAT, __VA_ARGS__)
#define QmLogSysError() QfLogSysError(__FILE__,__LINE__)
#define QmClearLog() QfClearLog()
#define QmFlushLog() FlushLog()
#else
#define QmLog(SFORMAT, ...) ((void)0)
#define QmLogSysError() ((void)0)
#define QmClearLog() ((void)0)
#define QmFlushLog() ((void)0)
#endif

//Dump report
extern void MakeDumpFile(const char* psName, _EXCEPTION_POINTERS* ExceptionInfo = NULL, DWORD dwThreadId = 0);
#ifdef QmDumpReportOn
#define QmDump(name) MakeDumpFile(name)
#else
#define QmDump(name) ((void)0)
#endif

//Log Error
#if defined(DEBUG) || defined(_DEBUG)
#define QmLogError(name) assert(false)
#else
#define QmLogError(name) do { char sTemp[1024]; sprintf_s(sTemp, 1024, "File:%s\nLine:%d\n%s\n",__FILE__,__LINE__,name); QmLog(sTemp); QmFlushLog(); QmDump("Error"); QmApp->ExitApp();} while(0,0)
#endif

//check Thread loop
#if defined(QmThreadDeadLoopCheckOn) && defined(QmLogOn) 
extern void QfCheckDeadLoop(int nSecs = 10);
#define QmCheckDeadLoop2(nSecs) QfCheckDeadLoop(nSecs);
#define QmCheckDeadLoop() QmCheckDeadLoop2(5);
#else
#define QmCheckDeadLoop2(nSecs) ((void)0)
#define QmCheckDeadLoop() ((void)0)
#endif

//output
#if defined(DEBUG) || defined(_DEBUG)
#define QmOutputString(SFORMAT, ...) { char __buf_0_[QmFormatBuffer * 2]; sprintf_s(__buf_0_, SFORMAT, __VA_ARGS__); OutputDebugStringA(__buf_0_); }
#else
#define QmOutputString(SFORMAT, ...) ((void)0)
#endif