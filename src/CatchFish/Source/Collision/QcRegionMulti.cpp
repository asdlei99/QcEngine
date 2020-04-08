#include "pch.hpp"
#include "QcRegionMulti.hpp"

ImplRuntimeClass2(QcRegionMulti, 500);

QcRegionMulti::~QcRegionMulti()
{

}

void QcRegionMulti::Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* region)
{
	if (IsDirty())
	{
#ifdef QmRegionBoxOn
		bool bFirst = true;
#endif
		if (region)
		{
			QcRegionList& destRegionList = static_cast<QcRegionMulti*>(region)->m_arRegionList;
			QmAssert(destRegionList.size() && m_arRegionList.size());
			QcRegionListIter iter(m_arRegionList.begin());
			QcRegionListIter endIter(m_arRegionList.end());
			QcRegionListIter temp_iter(destRegionList.begin());

			for (; iter != endIter ; ++iter, ++temp_iter)
			{
				(*iter)->SetDirty(true);
				(*iter)->Update(pt, rotate, scaleX, scaleY, (*temp_iter));
#ifdef QmRegionBoxOn
				if (bFirst)
				{
					m_rcBox = (*iter)->GetBox();
					bFirst = false;
				}
				else
				{
					m_rcBox.unionRect((*iter)->GetBox());
				}
#endif
			}
		}
		else
		{
			QcRegionListIter iter(m_arRegionList.begin());
			QcRegionListIter endIter(m_arRegionList.end());
			for (; iter != endIter ; ++iter)
			{
				(*iter)->SetDirty(true);
				(*iter)->Update(pt, rotate, scaleX, scaleY, NULL);
#ifdef QmRegionBoxOn
				if (bFirst)
				{
					m_rcBox = (*iter)->GetBox();
					bFirst = false;
				}
				else
				{
					m_rcBox.unionRect((*iter)->GetBox());
				}
#endif
			}
		}
		SetDirty(false);
	}	
}

bool QcRegionMulti::Intersect(QcRegion* pRegion)
{
	if (QcRegion::Intersect(pRegion))
	{
		QcRegionListIter iter(m_arRegionList.begin());
		QcRegionListIter endIter(m_arRegionList.end());
		for (; iter != endIter; ++iter)
		{
			if ((*iter)->Intersect(pRegion))
				return true;
		}
	}
	return false;
}

bool QcRegionMulti::Intersect(const QcPoint& vPos, f32 fRadius)
{
	QcRegionListConstIter iter(m_arRegionList.begin());
	QcRegionListIter endIter(m_arRegionList.end());
	for (; iter != endIter; ++iter)
	{
		if ((*iter)->Intersect(vPos, fRadius))
			return true;
	}
	return false;
}

bool QcRegionMulti::Intersect(const QcPolygon& polygon)
{
	QcRegionListConstIter iter(m_arRegionList.begin());
	QcRegionListConstIter endIter(m_arRegionList.end());
	for (; iter != endIter; ++iter)
	{
		if ((*iter)->Intersect(polygon))
			return true;
	}
	return false;
}

bool QcRegionMulti::Include(const QcPoint& vPos)
{
	QcRegionListConstIter iter(m_arRegionList.begin());
	QcRegionListConstIter endIter(m_arRegionList.end());
	for (; iter != endIter; ++iter)
	{
		if ((*iter)->Include(vPos))
			return true;
	}
	return false;
}

QcRegion* QcRegionMulti::Clone() const
{
	QcRegionMulti* pRegion = new QcRegionMulti();
	QcRegionListConstIter iter(m_arRegionList.begin());
	QcRegionListConstIter endIter(m_arRegionList.end());
	for (; iter != endIter; ++iter)
	{
		pRegion->m_arRegionList.PushBack((*iter)->Clone());
	}
	return pRegion;
}