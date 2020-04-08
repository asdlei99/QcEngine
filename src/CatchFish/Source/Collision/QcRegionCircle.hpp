#pragma once

#include "Collision\QcRegion.hpp"

class QcRegionCircle : public QcRegion
{
	DeclareRuntimeClass(QcRegionCircle);
public:
	QcRegionCircle()  {}

	QcRegionCircle(const QcPoint& ptCenter, f32 fRadius)
		: m_circle(ptCenter, fRadius)
	{}

	QcRegionCircle(const QcCircle& circle)
		: m_circle(circle)
	{}

protected:
	inline const QcPoint& GetCenterPt() const
	{
		return m_circle.GetPos();
	}
	inline void SetCenterPt(const QcPoint& vPos)
	{
		m_circle.SetPos(vPos);
		SetDirty(true);
	}
	inline f32 GetRadiu() const
	{
		return m_circle.GetRadiu();
	}
	inline void SetRadiu(f32 fRadiu)
	{
		m_circle.SetRadiu(fRadiu); 
		SetDirty(true);
	}
protected:
	virtual QcRegion* Clone() const;
	virtual bool Intersect(QcRegion* pRegion);
	virtual bool Intersect(const QcPoint& vPos, f32 fRadius);
	virtual bool Intersect(const QcPolygon& polygon);
	virtual bool Include(const QcPoint& vPos);
	virtual void Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* region);
private:
	QcCircle m_circle;
};