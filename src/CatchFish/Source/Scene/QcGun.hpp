#pragma once

#include "Scene\QcSceneNode.hpp"
#include "Res\QcResGunDesc.hpp"
class QcAnimatorTexture;
class QcResGunDesc;
class QcGun : public QcSceneNode
{
	DeclareRuntimeClass(QcGun);
public:
	QcGun();
	~QcGun();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	void Fire(u32 iScore, QcPlayer* pPlayer);
	void FreeFire(u32 iScore, QcPlayer* pPlayer);	
	bool IsSuperGun() const {return GetGunDesc()->GetSuperGun(); }
	const QcResGunDesc* GetGunDesc() const{ return reinterpret_cast<const QcResGunDesc*>(GetDesc());}
protected:
	virtual bool MakeBullet(u32 iScore, QcBullet* & bullet, QcPlayer* pPlayer);
protected:
	QcSharedPtr<QcAnimatorTexture> m_pNormolAnimator;
	QcSharedPtr<QcAnimatorTexture> m_pFireAnimator;
	QcResImageDesc* m_pBulletDesc;
	BOOL m_bFired;
};