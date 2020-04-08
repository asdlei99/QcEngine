#include "pch\pch.hpp"
#include "QcRuntimeClass.hpp"

QcRuntimeClass::QcRuntimeClass(const char* psClassName, ObjectCreatorFn fn, u32 nFixedSize, u32 iSize)
: m_pFn(fn), m_pFixedPool(NULL)
{
#if !(defined(QmMemLeakOn) || defined(QmTcMallocOn))
	m_pFixedPool = new QcFixedMemPool(nFixedSize, iSize);
#endif
	QmClassFactoryInstance->Register(psClassName, this);
}

QcRuntimeClass::QcRuntimeClass(const char* psClassName, ObjectCreatorFn fn) 
: m_pFn(fn), m_pFixedPool(NULL) 
{
	QmClassFactoryInstance->Register(psClassName, this);
}

QcRuntimeClass::~QcRuntimeClass()
{
	if (m_pFixedPool)
		delete m_pFixedPool;
}