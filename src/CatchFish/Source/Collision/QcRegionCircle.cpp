#include "pch.hpp"
#include "QcRegionCircle.hpp"

ImplRuntimeClass2(QcRegionCircle, 500);

void QcRegionCircle::Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* region)
{
	if (IsDirty())
	{
		if (region)
			m_circle = static_cast<QcRegionCircle*>(region)->m_circle;

		if (!(QcMath::equals(m_circle.m_vPos.X, 0.f) && QcMath::equals(m_circle.m_vPos.Y, 0.f)))
		{
			m_circle.m_vPos.X *= scaleX;
			m_circle.m_vPos.Y *= scaleY;
			m_circle.m_vPos.rotateByRad(rotate);
		}
		m_circle.m_vPos += pt;
		m_circle.m_radiu *= max(scaleX ,scaleY);
#ifdef QmRegionBoxOn
		m_rcBox = QcRect(m_circle.m_radiu, m_circle.m_radiu, m_circle.m_vPos);
#endif
		SetDirty(false);
	}	
}

bool QcRegionCircle::Intersect(QcRegion* pRegion)
{
	if (QcRegion::Intersect(pRegion))
	{
		return pRegion->Intersect(GetCenterPt(), GetRadiu());
	}
	return false;
}

bool QcRegionCircle::Intersect(const QcPoint& vPos, f32 fRadius)
{
	return QcVector(GetCenterPt(), vPos).smaller(fRadius + GetRadiu());
}

bool QcRegionCircle::Intersect(const QcPolygon& polygon)
{
	return polygon.intersectWith(QcCircle(GetCenterPt(), m_circle.GetRadiu()));
}

bool QcRegionCircle::Include(const QcPoint& vPos)
{
	return QcCircle(GetCenterPt(), m_circle.GetRadiu()).Include(vPos);
}

QcRegion* QcRegionCircle::Clone() const
{
	QcRegionCircle* pRegion = new QcRegionCircle(m_circle);
	return pRegion;
}