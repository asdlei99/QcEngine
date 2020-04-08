#include "Pch.hpp"
#include "QcRegionMultiCircle.hpp"

ImplRuntimeClass2(QcRegionMultiCircle, 500);

void QcRegionMultiCircle::Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* pSourceRegion)
{
	if (IsDirty())
	{
		if (pSourceRegion)
			m_arCircleList = static_cast<QcRegionMultiCircle*>(pSourceRegion)->m_arCircleList;

#ifdef QmRegionBoxOn
		bool bFirst = true;
#endif
		f32 fCs = cos(rotate);
		f32 fSn = sin(rotate);
		f32 fMaxScale = max(scaleX ,scaleY);
		vector<QcCircle>::iterator iter(m_arCircleList.begin());
		vector<QcCircle>::iterator endIter(m_arCircleList.end());
		for (; iter != endIter ; ++iter)
		{
			iter->m_vPos.X *= scaleX;
			iter->m_vPos.Y *= scaleY;
			iter->m_vPos.rotateBy(fCs, fSn);
			iter->m_vPos += pt;
			iter->m_radiu *= fMaxScale;
#ifdef QmRegionBoxOn
			if (bFirst)
			{
				m_rcBox = QcRect(iter->m_radiu, iter->m_radiu, iter->m_vPos);
				bFirst = false;
			}
			else
			{
				m_rcBox.unionRect(QcRect(iter->m_radiu, iter->m_radiu, iter->m_vPos));
			}
#endif
		}
		SetDirty(false);
	}	
}

bool QcRegionMultiCircle::Intersect(const QcPolygon& polygon)
{
	vector<QcCircle>::iterator iter(m_arCircleList.begin());
	vector<QcCircle>::iterator endIter(m_arCircleList.end());
	for (; iter != endIter; ++iter)
	{
		if (polygon.intersectWith(*iter))
		{
			return true;
		}
	}
	return false;
}

bool QcRegionMultiCircle::Intersect(const QcPoint& vPos, f32 fRadius)
{
	vector<QcCircle>::iterator iter(m_arCircleList.begin());
	vector<QcCircle>::iterator endIter(m_arCircleList.end());
	QcVector vec;
	f32 fLenSQ = (fRadius + iter->m_radiu) * (fRadius + iter->m_radiu);
	for (; iter != endIter; ++iter)
	{
		vec.X = iter->m_vPos.X - vPos.X;
		vec.Y = iter->m_vPos.Y - vPos.Y;
		if (vec.getLengthSQ() < fLenSQ)
		{
			return true;
		}
	}	
	return false;
}

bool QcRegionMultiCircle::Intersect(QcRegion* pRegion)
{
	if (QcRegion::Intersect(pRegion))
	{
		vector<QcCircle>::iterator iter(m_arCircleList.begin());
		vector<QcCircle>::iterator endIter(m_arCircleList.end());
		for (; iter != endIter; ++iter)
		{
			if (pRegion->Intersect(iter->m_vPos, iter->m_radiu))
			{
				return true;
			}
		}
	}
	return false;
}

QcRegion* QcRegionMultiCircle::Clone() const
{
	QcRegionMultiCircle* pRegion = new QcRegionMultiCircle(m_arCircleList);
	return pRegion;
}

bool QcRegionMultiCircle::Include(const QcPoint& vPos)
{
	vector<QcCircle>::iterator iter(m_arCircleList.begin());
	vector<QcCircle>::iterator endIter(m_arCircleList.end());
	for (; iter != endIter; ++iter)
	{
		if (iter->Include(vPos))
		{
			return true;
		}
	}	
	return false;
}