#pragma once

namespace std
{
	template<class _FwdIt,
	class _Ty> inline
		_FwdIt binary_searchEx(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
	{	// test if _Val equivalent to some element, using operator<
		_First = std::lower_bound(_First, _Last, _Val);
		if (_First != _Last && !(_Val < *_First))
			return _First;
		return _Last;
	}

	// TEMPLATE FUNCTION binary_search WITH PRED
	template<class _FwdIt,
	class _Ty,
	class _Pr> inline
		_FwdIt binary_searchEx(_FwdIt _First, _FwdIt _Last,
		const _Ty& _Val, _Pr _Pred)
	{	// test if _Val equivalent to some element, using _Pred
		_First = std::lower_bound(_First, _Last, _Val, _Pred);
		if (_First != _Last && !_Pred(_Val, *_First))
			return _First;
		return _Last;
	}
}

template<class _FwdIt,class _Ty> 
inline _FwdIt QfMoveToBack(_FwdIt _First, _FwdIt _Last, const _Ty& _Val )
{
	_First = std::find(_First, _Last, _Val);
	if (_First == _Last)
		return (_First);	// empty sequence, all done

	_FwdIt beginIter = _First;
	for (; _First != _Last; ++_First)
	{
		if (!_Val != *_First)
		{
			if (beginIter != _First)
			{
				swap(*beginIter, *_First);
			}
			++beginIter;
		}
	}
	return beginIter;
}

template<class _FwdIt,class _Pr> 
inline _FwdIt QfMoveToBack_if(_FwdIt _First, _FwdIt _Last, _Pr _Pred)
{
	_First = std::find_if(_First, _Last, _Pred);
	if (_First == _Last)
		return (_First);	// empty sequence, all done

	_FwdIt beginIter = _First;
	for (; _First != _Last; ++_First)
	{
		if (!_Pred(*_First))
		{
			if (beginIter != _First)
			{
				swap(*beginIter, *_First);
			}
			++beginIter;
		}
	}
	return beginIter;
}

template<typename T1, typename T2>
inline bool QfErase(T1& vec, const T2& var)
{
	T1::iterator iter(std::find(vec.begin(), vec.end(), var));
	if (iter != vec.end())
	{
		vec.erase(iter);
		return true;
	}
	return false;
}

template<typename T1>
inline void QfMakeUnique(std::vector<T1>& vec)
{
	std::sort(vec.begin(), vec.end(), std::less<T1>());
	std::vector<T1>::iterator iter = std::unique(vec.begin(), vec.end());
	if (iter != vec.end())
		vec.erase(iter, vec.end());
}
template<typename T1>
inline void QfMakeUnique(QcArray<T1>& vec)
{
	std::sort(vec.begin(), vec.end(), std::less<T1>());
	QcArray<T1>::iterator iter = std::unique(vec.begin(), vec.end());
	if (iter != vec.end())
		vec.erase(iter, vec.end());
}

inline void QfSplitString(const char* psPath, QcStringList& pathList, char matchCh)
{
	char ch = *psPath;
	const char* pBegin = psPath;
	while (ch != 0)
	{
		if (ch == matchCh)
		{
			if (psPath != pBegin)
				pathList.push_back(QcString(pBegin, psPath - pBegin));
			++psPath;
			pBegin = psPath;
		}
		else
		{
			++psPath;
		}
		ch = *psPath;
	}
	if (psPath != pBegin)
		pathList.push_back(QcString(pBegin, psPath - pBegin));
}

inline u32 U8ToUInt32(u8* pArray, int nCount)
{
	u32 iRet = pArray[0];
	for (int i=1; i<nCount; ++i)
	{
		iRet *= 10;
		iRet += pArray[i];
	}
	return iRet;
}

inline void UInt32ToA(u32 iValue, u8* pArray, int nCount)
{
	for (int i=nCount-1; i>=0; i--)
	{
		pArray[i] = iValue %10;
		iValue = iValue/10;
	}
}

inline unsigned int QfCountOne(unsigned int x){
	x = (x & 0x55555555) + (x >> 1 & 0x55555555);
	x = (x & 0x33333333) + (x >> 2 & 0x33333333);
	x = (x & 0x0f0f0f0f) + (x >> 4 & 0x0f0f0f0f);
	x = (x & 0x00ff00ff) + (x >> 8 & 0x00ff00ff);
	x = (x & 0x0000ffff) + (x >> 16 & 0x0000ffff);
	return x;
}

inline BOOL EnbaleShutdownPrivilege()
{
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;

	LUID uID = {0};
	TOKEN_PRIVILEGES stToken_Privileges = {0};

	hProcess = ::GetCurrentProcess();
	if (!::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
		return FALSE;

	if (!::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &uID))
		return FALSE;

	stToken_Privileges.PrivilegeCount = 1;
	stToken_Privileges.Privileges[0].Luid = uID;
	stToken_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!::AdjustTokenPrivileges(hToken, FALSE, &stToken_Privileges, sizeof(stToken_Privileges), NULL, NULL))
		return FALSE;

	if (::GetLastError() != ERROR_SUCCESS)
		return FALSE;

	::CloseHandle(hToken);
	return TRUE;
}

inline __time64_t MakeTime64(const SYSTEMTIME& sysTime)
{
	struct tm atm;

	atm.tm_sec = sysTime.wSecond;
	atm.tm_min = sysTime.wMinute;
	atm.tm_hour = sysTime.wHour;
	atm.tm_mday = sysTime.wDay;
	atm.tm_mon = sysTime.wMonth - 1;        // tm_mon is 0 based
	atm.tm_year = sysTime.wYear - 1900;     // tm_year is 1900 based
	atm.tm_isdst = -1;

	return _mktime64(&atm);
}

inline SYSTEMTIME MakeSystemTime(__time64_t t)
{
	SYSTEMTIME timeDest = {0};
	struct tm tm;
	errno_t err = _localtime64_s(&tm, &t);
	if (err == 0)
	{
		timeDest.wYear = (WORD) (1900 + tm.tm_year);
		timeDest.wMonth = (WORD) (1 + tm.tm_mon);
		timeDest.wDayOfWeek = (WORD) tm.tm_wday;
		timeDest.wDay = (WORD) tm.tm_mday;
		timeDest.wHour = (WORD) tm.tm_hour;
		timeDest.wMinute = (WORD) tm.tm_min;
		timeDest.wSecond = (WORD) tm.tm_sec;
		timeDest.wMilliseconds = 0;
	}
	return timeDest;
}

inline __time64_t GetLocalTime64()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	return MakeTime64(sysTime);
}

inline __time64_t SpanTime(__time64_t t, LONG lDays, int nHours = 0, int nMins = 0, int nSecs = 0)
{
	return t + nSecs + 60* (nMins + 60* (nHours + __int64(24) * lDays));
}

template<size_t _size>
inline const char* QfGetModuleBaseName(HMODULE hModule, char (&buf)[_size])
{
	char temp[MAX_PATH];
	GetModuleFileNameA(hModule, temp, MAX_PATH);
	strcpy_s(buf, ::PathFindFileNameA(temp));
	return buf;
}

static HMODULE QfGetModuleHandle(DWORD64 address = 0)
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, address == 0 ? (LPCTSTR)&QfGetModuleHandle : (LPCTSTR)address, &hModule);
	return hModule;
}

inline void QfExcuteShell(const char* psExe, const char* psPara, DWORD dwMilliseconds = INFINITE)
{
	SHELLEXECUTEINFOA exeInfo = {0};
	exeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	exeInfo.fMask = SEE_MASK_DOENVSUBST | SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOCLOSEPROCESS | 0x04000000;
	exeInfo.lpFile = psExe;
	exeInfo.lpParameters = psPara;
	exeInfo.lpVerb = "open";
	exeInfo.nShow = SW_SHOWNORMAL;
	ShellExecuteExA(&exeInfo);
	WaitForSingleObject(exeInfo.hProcess, dwMilliseconds);
	CloseHandle(exeInfo.hProcess );
}

inline u64 QfGetCPUTicks()
{
	u64 iRet = 0;
	return QueryPerformanceCounter((LARGE_INTEGER*)&iRet) ? iRet : 0;
}
inline u64 QfGetCPUFrequency()
{
	u64 iRet = 0;
	return QueryPerformanceFrequency((LARGE_INTEGER*)&iRet) ? iRet : 1000;
}
const __int64 giCPUFrequency = QfGetCPUFrequency();
const f64 giCPUFrequency1 = f64(giCPUFrequency);
const f32 gfCPUFrequency2 = giCPUFrequency/1000.f;
inline f32 QfCPUTicksToMS(u64 tm)
{
	return tm/gfCPUFrequency2;
}
inline f32 QfCPUTicksToS(u64 tm)
{
	return f32(tm/giCPUFrequency);
}