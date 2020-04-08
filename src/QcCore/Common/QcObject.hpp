#pragma once

#include "Common\QcReferenceCounted.hpp"
class QcObject : public QcReferenceCounted
{
public:
	template<class T> bool Is() const{ return dynamic_cast<const T>(this) != NULL;}
	template<class T> bool Is() { return dynamic_cast<T>(this) != NULL;}
	template<class T> T As() { return dynamic_cast<T>(this); }
	template<class T> const T As() const { return dynamic_cast<const T>(this);}

#ifdef QmBoostOn
	QcObject() : m_pVariantMap(NULL) {}
	virtual ~QcObject()
	{
		if (m_pVariantMap)
			delete m_pVariantMap;
	}
	template <typename T>
	const T& GetProperty(const QcString& key, const T& defaultValue) const
	{
		if (m_pVariantMap)
		{
			QcVariantMapConstIter iter = m_pVariantMap->find(key);
			if (iter != m_pVariantMap->end())
				return iter->second;
		}
		return defaultValue;
	}
	
	const QcVariant& GetProperty(const char* key) const
	{
		return GetProperty(QcString(key));
	}
	const QcVariant& GetProperty(const QcString& key) const
	{
		if (m_pVariantMap)
		{
			QcVariantMapConstIter iter = m_pVariantMap->find(key);
			if (iter != m_pVariantMap->end())
				return iter->second;
		}
		static QcVariant var;
		return var;
	}
	void SetProperty(const char* key, const QcVariant& value)
	{
		SetProperty(QcString(key), value);
	}
	void SetProperty(const QcString& key, const QcVariant& value)
	{
		if (m_pVariantMap == NULL)
			m_pVariantMap = new QcVariantMap();
		(*m_pVariantMap)[key] = value;
	}
protected:
	QcVariantMap* m_pVariantMap;
#endif
};