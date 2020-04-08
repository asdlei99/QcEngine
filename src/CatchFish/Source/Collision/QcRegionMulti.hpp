#pragma once

#include "Collision\QcRegion.hpp"

class QcRegionMulti : public QcRegion
{
	DeclareRuntimeClass(QcRegionMulti);
public:
	QcRegionMulti(){}
	QcRegionMulti(const QcRegionList& regionList) : m_arRegionList(regionList){}
	virtual ~QcRegionMulti();

	int AddRegion(QcRegion* pRegion)
	{
		m_arRegionList.PushBack(pRegion);
		return m_arRegionList.size();
	}
protected:
	virtual QcRegion* Clone() const;
	virtual bool Intersect(QcRegion* pRegion);
	virtual bool Intersect(const QcPoint& vPos, f32 fRadius);
	virtual bool Intersect(const QcPolygon& polygon);
	virtual bool Include(const QcPoint& vPos);
	virtual void Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* region);
private:
	QcRegionList m_arRegionList;
};