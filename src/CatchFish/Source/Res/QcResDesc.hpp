#pragma once

class QcXmlNodeIterator;
class QcResDescParam;
class QcResDesc : public QcObject
{
public:
	virtual ~QcResDesc();
	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual const QcObjType& GetObjType() const {return ID_NULL; };

	template<class T>
	const T* GetParam() const
	{
		return static_cast<T*>(m_pParam);
	}
	QcResDescParam* GetParam() const
	{
		return m_pParam.Get();
	}
	bool operator < (const QcResDesc* right) const
	{
		return GetObjType() < right->GetObjType();
	}
	bool operator < (const QcObjType& objType) const
	{
		return GetObjType() < objType;
	}
protected:
	QcSharedPtr<QcResDescParam> m_pParam;
};

struct QsResDescSorter
{
	bool operator ()(const QcResDesc* left, const QcResDesc* right) const
	{
		return left->GetObjType() < right->GetObjType();
	}
	bool operator()(const QcResDesc* left, const QcObjType& right) const
	{
		return left->GetObjType() < right;
	}
	bool operator()(const QcObjType& left, const QcResDesc* right) const
	{
		return left < right->GetObjType();
	}
	bool operator()(const QcObjType& left, const QcObjType& right) const
	{
		return left < right;
	}
};