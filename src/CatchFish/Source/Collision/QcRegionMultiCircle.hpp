#pragma once

#include "Collision\QcRegion.hpp"

class QcRegionMultiCircle : public QcRegion
{
	DeclareRuntimeClass(QcRegionMultiCircle);
public:
	QcRegionMultiCircle(){}
	QcRegionMultiCircle(const vector<QcCircle>& circleList) : m_arCircleList(circleList){}
	~QcRegionMultiCircle(){}
	void AddCircle(const QcCircle& circle){ m_arCircleList.push_back(circle);}
protected:
	virtual bool Intersect(QcRegion* pRegion);
	virtual bool Intersect(const QcPoint& vPos, f32 fRadius);
	virtual bool Intersect(const QcPolygon& polygon);
	virtual bool Include(const QcPoint& vPos);
	virtual void Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* region);
	virtual QcRegion* Clone() const;
private:
	vector<QcCircle> m_arCircleList;
};