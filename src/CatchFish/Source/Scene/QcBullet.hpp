#pragma once

#include "Scene\QcSceneDigital.hpp"
class QcResGunDesc;
class QcAnimatorMoveBoundBox;
class QcBullet : public QcSceneDigital
{
	typedef QcSceneDigital BaseClass;
	DeclareRuntimeClass(QcBullet);
public:
	QcBullet();
	~QcBullet();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void OnRemoveNodeMonitor(QcSceneNode*);
	virtual void ToDead();

	void LoadBulletInfo(const QcResGunDesc* pGunDesc, u32 iScore, QcPlayer* pPlayer);
	void Explode();
	u32 GetLevel() const {return m_iLevel;}
	u32 GetMultiple() const {return m_bSuperBullet ? 2 : 1;}
	f32 GetExpodeRadiu() const {return m_fExpodeRadiu;}
	QcPlayer* GetOwner() const {return m_pPlayer;}
	const QcFish* GetTargetFish() const { return m_pTargetFish; }
	bool IsBugBullet(){return m_bIsBugBullet;}
	void SetBugBullet(){m_bIsBugBullet = true;}
protected:
	void RemoveMonitor();
protected:
	QcAnimatorMoveBoundBox* m_pBoundBox;
	QcPlayer* m_pPlayer;
	QcSharedPtr<QcFish> m_pTargetFish;
	f32 m_fExpodeRadiu;
	u32 m_iLevel;
	bool m_bSuperBullet;
	bool	 m_bIsBugBullet;
};