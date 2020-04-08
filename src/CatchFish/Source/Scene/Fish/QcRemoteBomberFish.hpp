#pragma once

#include "Scene\Fish\QcBombFish.hpp"
class QcRemoteBomberFish : public QcBombFish
{
	DeclareRuntimeClass(QcRemoteBomberFish);
public:
	QcRemoteBomberFish();

	virtual void Init(const QcResImageDesc& desc, bool = false);
	virtual void HitByBullet(QcBullet* pBullet);
	virtual void HitToDie(QcBullet* pBullet);
	virtual bool IsDelayCalScore() const {return true;}
};