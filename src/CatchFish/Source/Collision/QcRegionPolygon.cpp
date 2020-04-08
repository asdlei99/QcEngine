#include "pch.hpp"
#include "QcRegionPolygon.hpp"

ImplRuntimeClass(QcRegionPolygon);

void QcRegionPolygon::Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* region)
{
	if (IsDirty())
	{
		if (region)
			m_polygon = static_cast<QcRegionPolygon*>(region)->m_polygon;

		int nSize = m_polygon.Size();
#ifdef QmRegionBoxOn
		bool bFirst = true;
#endif
		const f64 cs = cos(rotate);
		const f64 sn = sin(rotate);
		for (int i=0; i<nSize; ++i)
		{
			QcPoint& point = m_polygon[i];
			point.X *= scaleX;
			point.Y *= scaleY;
			point.rotateBy(cs, sn);
			point += pt;
#ifdef QmRegionBoxOn
			if (bFirst)
			{
				m_rcBox._BottomLeftPt = point;
				m_rcBox._TopRightPt = point;
				bFirst = false;
			}
			else
			{
				m_rcBox.addInternalPoint(point);
			}
#endif
		}
		SetDirty(false);
	}	
}

bool QcRegionPolygon::Intersect(QcRegion* pRegion)
{
	if (QcRegion::Intersect(pRegion))
	{
		return pRegion->Intersect(m_polygon);
	}
	return false;
}

bool QcRegionPolygon::Intersect(const QcPoint& vPos, f32 fRadius)
{
	return m_polygon.intersectWith(QcCircle(vPos, fRadius));
}
bool QcRegionPolygon::Intersect(const QcPolygon& polygon)
{
	return m_polygon.intersectWith(polygon);
}
bool QcRegionPolygon::Include(const QcPoint& vPos)
{
	return m_polygon.isPointInside(vPos);
}

QcRegion* QcRegionPolygon::Clone() const
{
	QcRegionPolygon* pRegion = new QcRegionPolygon(m_polygon);
	return pRegion;
}