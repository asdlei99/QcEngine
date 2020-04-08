#pragma once

#include "Scene\Fish\QcBombFish.hpp"
class QcScopeBombFish : public QcBombFish
{
	DeclareRuntimeClass(QcScopeBombFish);
public:
	QcScopeBombFish();

	virtual void Init(const QcResImageDesc& desc, bool = false);
	virtual void HitByBullet(QcBullet* pBullet);
    virtual void HitToDie(QcBullet* pBullet);
protected:
	bool IsFullScreen() const {return m_bFullScreen;}
	f32 GetExpodeRadiu() const {return m_fRadiu;}
protected:
	f32 m_fRadiu;
	bool m_bFullScreen;
};