#pragma once
#include "Common\QcReferenceCounted.hpp"

template<class T>
class QcSharedPtr
{
public:
	inline explicit QcSharedPtr(const T* p = NULL)
		: m_p(const_cast<T*>(p))
	{
		AddRef();
	}
	inline ~QcSharedPtr()
	{
		ReduceRef();
	}
	inline QcSharedPtr(const QcSharedPtr<T>& p)
	{
		m_p = p.m_p;
		AddRef();
	}
	inline QcSharedPtr& operator=(const QcSharedPtr<T>& p)
	{
		Set(p.Get());
		return *this;
	}
	inline QcSharedPtr& operator=(const T* p)
	{
		Set(p);
		return *this;
	}
	inline bool operator < (const QcSharedPtr<T>& p) const
	{
		return m_p < p.m_p;
	}
 	inline bool operator == (const QcSharedPtr<T>& p) const
 	{
 		return m_p == p.m_p;
 	}
	inline bool operator == (const T* p) const
	{
		return m_p == p;
	}
	inline bool operator != (const QcSharedPtr<T>& p) const
	{
		return m_p != p.m_p;
	}
	inline operator bool()
	{
		return m_p != NULL;
	}
	inline operator T*()
	{
		return m_p;
	}
	inline operator const T*() const
	{
		return m_p;
	}

	inline T* operator-> () const// never throws
	{
		return m_p;
	}
	inline T* Get() const
	{
		return m_p;
	}
	inline void Set(const T* p)
	{
		if (m_p != p)
		{
			ReduceRef();
			m_p = const_cast<T*>(p);
			AddRef();
		}
	}
	inline void Reset()
	{
		ReduceRef();
		m_p = NULL;
	}
	inline void Swap(QcSharedPtr<T> & other)
	{
		std::swap(m_p, other.m_p);
	}
private:
	inline void AddRef()
	{
		if (m_p)
			IncreaceReferenceCounted(reinterpret_cast<QcReferenceCounted*>(m_p));
	}
	inline void ReduceRef()
	{
		if (m_p)
		{
			DecreaceReferenceCounted(reinterpret_cast<QcReferenceCounted*>(m_p));
			m_p = NULL;
		}
	}
private:
	T* m_p;
};