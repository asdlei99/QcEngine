#pragma once

class QcReferenceCounted
{
	template<typename T> friend class QcSharedPtr;
public:
	QcReferenceCounted() : m_nReferenceCounter(0){}
	QcReferenceCounted(const QcReferenceCounted&) : m_nReferenceCounter(0){}
	QcReferenceCounted& operator=(const QcReferenceCounted&){return *this;}
	virtual ~QcReferenceCounted() = 0 {}

	LONG GetRefCount() const {return m_nReferenceCounter;}
private:
	friend inline void IncreaceReferenceCounted(QcReferenceCounted* pRef);
	friend inline bool DecreaceReferenceCounted(QcReferenceCounted* pRef);
private:
	volatile LONG m_nReferenceCounter;
};

inline void IncreaceReferenceCounted(QcReferenceCounted* pRef)
{
	InterlockedIncrement(&pRef->m_nReferenceCounter);
}

inline bool DecreaceReferenceCounted(QcReferenceCounted* pRef)
{
	if (InterlockedDecrement(&pRef->m_nReferenceCounter) == 0)
	{
		delete pRef;
		return true;
	}
	return false;
}