#pragma once

typedef QcObject* (PASCAL* ObjectCreatorFn)();
class QcRuntimeClass
{
	//仅限单线程
	class QcFixedMemPool
	{
		friend class QcRuntimeClass;
	public:
		QcFixedMemPool(u32 nFixedSize, u32 iSize)
			: m_FixedSize(nFixedSize)/*, m_nSize(iSize)*/
		{
			m_pBegin = (BYTE*)malloc(m_FixedSize * iSize);
			m_pCurrent = m_pBegin;
			m_pEnd = m_pBegin + m_FixedSize * iSize;
			m_pFreeList = NULL;
			m_iCheck = 0;
		}
		~QcFixedMemPool()
		{
			QmAssert(m_iCheck == 0);
			free(m_pBegin);
		}
		void* Allocate()
		{
			void* pRet = NULL;
			if (m_pFreeList != NULL) 
			{
				pRet = m_pFreeList;
				m_pFreeList = *(reinterpret_cast<void**>(pRet));
			}
			else
			{
				pRet = m_pCurrent + m_FixedSize;
				if (pRet < m_pEnd)
				{
					++m_iCheck;
					m_pCurrent = (BYTE*)pRet;
				}
				else
					pRet = NULL;
			}
			return pRet;
		}
		bool Deallocate(void* p)
		{
			if (p >= m_pBegin && p <= m_pCurrent)
			{
				*(reinterpret_cast<void**>(p)) = m_pFreeList;
				m_pFreeList = p;
				--m_iCheck;
				return true;
			}
			return false;
		}
	protected:
		BYTE* m_pBegin;
		BYTE* m_pCurrent;
		BYTE* m_pEnd;
		void* m_pFreeList;
		u32 m_FixedSize;
		s16 m_iCheck;
	};
public:
	QcRuntimeClass(const char* psClassName, ObjectCreatorFn fn, u32 nFixedSize, u32 iSize);
	QcRuntimeClass(const char* psClassName, ObjectCreatorFn fn);
	~QcRuntimeClass();

	void* Allocate(size_t nSize)
	{
		void* pRet = NULL;
		if (m_pFixedPool && nSize == m_pFixedPool->m_FixedSize)
		{
			pRet = m_pFixedPool->Allocate();
		}
		if (pRet == NULL)
			pRet = malloc(nSize);
		return pRet;
	}
	void Deallocate(void* p)
	{
		if (m_pFixedPool == NULL || !m_pFixedPool->Deallocate(p))
			free(p);
	}
	QcObject* CreateObject()
	{
		return m_pFn();
	}
	template<class T>
	T* CreateObject()
	{
		QcObject* pObj = CreateObject();
		return static_cast<T*>(pObj);
	}
private:
	ObjectCreatorFn m_pFn;
	QcFixedMemPool* m_pFixedPool;
};

#define QmRuntimeClass(class_name) class_name::RuntimeClass()

#define DeclareRuntimeClass(class_name)\
public:\
	virtual QcRuntimeClass* GetRuntimeClass() const {return class_name::RuntimeClass();}\
	static QcRuntimeClass* RuntimeClass() {return &gsClass##class_name;}\
	static QcObject* PASCAL CreateObject(){ return new class_name();} \
	static void* PASCAL operator new(size_t nSize)\
	{\
		return RuntimeClass()->Allocate(nSize);\
	}\
	void PASCAL operator delete(void* p)\
	{\
		RuntimeClass()->Deallocate(p);\
	}\
private:\
	static QcRuntimeClass gsClass##class_name

#define ImplRuntimeClass2(class_name, size) QcRuntimeClass class_name::gsClass##class_name(#class_name, class_name::CreateObject, sizeof(class_name), size)
#define ImplRuntimeClass(class_name) QcRuntimeClass class_name::gsClass##class_name(#class_name, class_name::CreateObject)
	



