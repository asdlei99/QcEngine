#pragma once

#include "Scene\Fish\QcFish.hpp"
class QcGravityFish : public QcFish
{
	DeclareRuntimeClass(QcGravityFish);
public:
	QcGravityFish();
	
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual bool ToDie(bool bDieTimer);

	void SetRelatedFishList(const QcList<QcFish*>& fishList)
	{
		m_arRelatedFishList.Clear();
		QcList<QcFish*>::const_iterator iter(fishList.begin());
		QcList<QcFish*>::const_iterator endIter(fishList.end());
		for (; iter != endIter; ++iter)
		{
			m_arRelatedFishList.Insert(*iter);
		}
	}
protected:
	QcFishList m_arRelatedFishList;
};