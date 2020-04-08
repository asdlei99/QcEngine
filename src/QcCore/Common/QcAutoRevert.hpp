#pragma once

template<class T>
class QcAutoRevert
{
	void* PASCAL operator new(size_t nSize);
	void* PASCAL operator new(size_t nSize, void* p);
	QmDisableCopyAssignment(QcAutoRevert);
public:
	REALINLINE QcAutoRevert(T& ref, const T& init)
		: m_ref(ref), m_value(ref)
	{
		ref = init;
	}
	REALINLINE QcAutoRevert(T& ref)
		: m_ref(ref), m_value(ref)
	{
	}
	REALINLINE ~QcAutoRevert()
	{
		m_ref = m_value;
	}
protected:
	T& m_ref;
	T m_value;
};