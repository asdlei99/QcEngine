#pragma once

#define FUN_CALL_BYTE_COUNT (5)
#define MakeFunWord(functionName) (DWORD)(functionName)

template<class T>
DWORD MakeClassFunWord(T value)
{
	return *((DWORD*)&value);
}

class QcHookFunction
{
	friend class QcHookHelper; 
public:
	explicit QcHookFunction()
	{
		m_newAddress = 0;
		m_pRawAddress = NULL;
		memset(m_JmpCode, 0, sizeof(m_JmpCode));
	}
	~QcHookFunction()
	{
		Hook(0);
		m_pRawAddress = 0;
		m_newAddress = 0;
	}
	DWORD Init(const char* psDll, const char* psFunction, DWORD pNewAddress = 0)
	{
		HMODULE hModule = GetModuleHandleA(psDll);
		DWORD pFun = (DWORD)::GetProcAddress(hModule, psFunction);
		Init(pFun, pNewAddress);
		return pFun;
	}
	void Init(DWORD pRawAddress, DWORD pNewAddress = 0)
	{
		if (pRawAddress)
		{
			//Save the jmp code.
			m_pRawAddress = (void*)pRawAddress;
			memcpy(m_JmpCode, m_pRawAddress, sizeof(m_JmpCode));

			//open the read/write 
			DWORD oldpro=0; 
			VirtualProtect(m_pRawAddress,5,PAGE_EXECUTE_READWRITE,&oldpro); 
			if (pNewAddress)
			{
				Hook(pNewAddress);
			}
		}
	}

	void Hook(DWORD pNewAddress = 0)
	{
		if (m_pRawAddress)
		{
			if (pNewAddress == 0)
			{
				memcpy(m_pRawAddress, m_JmpCode, sizeof(m_JmpCode));
			}
			else
			{
				*(BYTE*)m_pRawAddress = 0xe9; 
				*(DWORD*)((BYTE*)m_pRawAddress+1)= pNewAddress - (DWORD)m_pRawAddress - 5;
				m_newAddress = pNewAddress;
			}
		}
	}
private:
	DWORD m_newAddress;
	void* m_pRawAddress;
	BYTE m_JmpCode[FUN_CALL_BYTE_COUNT];
};

class QcHookHelper
{
	QmDisableCopyAssignment(QcHookHelper);
public:
	QcHookHelper(QcHookFunction& funHooker) : m_funHooker(funHooker)
	{
		m_funHooker.Hook(NULL);
	}
	~QcHookHelper()
	{
		m_funHooker.Hook(m_funHooker.m_newAddress);
	}
private:
	QcHookFunction& m_funHooker;
};