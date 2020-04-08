#pragma once

class QcRuntimeClass;

class QcClassFactory : public QcObject, public QcSingleton<QcClassFactory>
{
	typedef std::map<QcString, QcRuntimeClass*, QcNoCaseCompare> QcRuntimeClassMap;
public:
	void Register(const char* psClassName, QcRuntimeClass* pRuntimeClass);
	QcRuntimeClass* Find(const char* psClassName);
	QcRuntimeClass* Find(const QcString& className);
	QcObject* Create(const char* psClassName);

	template<class T>
	T* Create(const char* psClassName)
	{
		QcObject* p = Create(psClassName);
		if (p && !p->Is<T*>())
		{
			delete p;
			return NULL;
		}
		return static_cast<T*>(p);
	}
private:
	QcRuntimeClassMap gsRuntimeClassMap;
};