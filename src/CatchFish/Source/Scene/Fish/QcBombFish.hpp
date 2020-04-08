#pragma once

#include "Scene\Fish\QcFish.hpp"
class QcBombFish : public QcFish
{
	DeclareRuntimeClass(QcBombFish);
public:
	QcBombFish();

	virtual void HitToDie(QcBullet* pBullet);
	virtual bool IsBombFish() const { return true; }
	virtual u32 GetExplodeScore() const;

	void ToDieFishList(bool bDieTimer = false);
protected:
	QcList<QcFish*> m_arBombList;
};