#pragma once

#include "Scene\Fish\QcScopeBombFish.hpp"

class QcDelayBombFish : public QcScopeBombFish
{
	DeclareRuntimeClass(QcDelayBombFish);
public:
	QcDelayBombFish();

	virtual void OnTimer(u32 nTimer, void* pContext);
	virtual void HitToDie(QcBullet* pBullet);
	virtual bool IsDelayCalScore() const {return true;}
    virtual void OnRemoveNodeMonitor(QcSceneNode*);
	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
protected:
	void Explose();
protected:
	u32 m_iExplodeScore;
	//QcList<QcSharedPointer<QcFish>> m_arDelayBombList;
	QcList<u32> m_arFishTypeList;
    u32 m_iBombedDelayFishCount;
	float	m_fParticleTime;
};