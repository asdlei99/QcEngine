#include "pch.hpp"
#include "QcBombFish.hpp"

ImplRuntimeClass(QcBombFish);

QcBombFish::QcBombFish()
{

}

void QcBombFish::HitToDie(QcBullet* pBullet)
{
	QcFish::HitToDie(pBullet);
	QcList<QcFish*>::iterator iter(m_arBombList.begin());
	QcList<QcFish*>::iterator endIter(m_arBombList.end());
	for (; iter != endIter; ++iter)
	{
		(*iter)->ExploseToDie(this);
	}
}
void QcBombFish::ToDieFishList(bool bDieTimer)
{
	ToDie(bDieTimer);
	QcList<QcFish*>::iterator iter(m_arBombList.begin());
	QcList<QcFish*>::iterator endIter(m_arBombList.end());
	for (; iter != endIter; ++iter)
	{
		(*iter)->m_pOwner = m_pOwner;
		(*iter)->ToDie(bDieTimer);
	}
}

u32 QcBombFish::GetExplodeScore() const
{
	u32 iScore = GetScore();
	QcList<QcFish*>::const_iterator iter(m_arBombList.begin());
	QcList<QcFish*>::const_iterator endIter(m_arBombList.end());
	for (; iter != endIter; ++iter)
	{
        iScore += (*iter)->GetScore();
	}
	return iScore;
}