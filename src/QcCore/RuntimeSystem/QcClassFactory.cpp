#include "pch\pch.hpp"
#include "QcClassFactory.hpp"

void QcClassFactory::Register(const char* psClassName, QcRuntimeClass* pRuntimeClass)
{
	gsRuntimeClassMap[psClassName] = pRuntimeClass;
}

QcRuntimeClass* QcClassFactory::Find(const char* psClassName)
{
	QcRuntimeClassMap::iterator iter = gsRuntimeClassMap.find(psClassName);
	if (iter != gsRuntimeClassMap.end())
		return iter->second;
	return NULL;
}

QcRuntimeClass* QcClassFactory::Find(const QcString& className)
{
	QcRuntimeClassMap::iterator iter = gsRuntimeClassMap.find(className);
	if (iter != gsRuntimeClassMap.end())
		return iter->second;
	return NULL;
}

QcObject* QcClassFactory::Create(const char* psClassName)
{
	QcRuntimeClassMap::iterator iter = gsRuntimeClassMap.find(psClassName);
	if (iter != gsRuntimeClassMap.end())
	{
		return iter->second->CreateObject();
	}
	else
	{
		QmAssert(false);
	}
	return NULL;
}