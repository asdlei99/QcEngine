#pragma once

#include "Collision\QcRegion.hpp"

class QcRegionPolygon : public QcRegion
{
	DeclareRuntimeClass(QcRegionPolygon);
public:
	QcRegionPolygon(){}
	QcRegionPolygon(const QcPointList& pointList) 
		:m_polygon(pointList) 
	{}
	QcRegionPolygon(const QcPolygon& polygon) : m_polygon(polygon) {}
	QcRegionPolygon(const QcPoint* pPointList, int nCount) : m_polygon(pPointList, nCount) {}

protected:
	virtual QcRegion* Clone() const;
	virtual bool Intersect(QcRegion* pRegion);
	virtual bool Intersect(const QcPoint& vPos, f32 fRadius);
	virtual bool Intersect(const QcPolygon& polygon);
	virtual bool Include(const QcPoint& vPos);
	virtual void Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* region);
private:
	QcPolygon m_polygon;
};