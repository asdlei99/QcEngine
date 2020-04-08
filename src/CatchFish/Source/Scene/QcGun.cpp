#include "pch.hpp"
#include "QcGun.hpp"
#include "QcPlayer.hpp"
#include "QcBullet.hpp"
#include "QcBulletMgr.hpp"
#include "Res\QcResGunDesc.hpp"
#include "Res\QcResManager.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "QcSceneAnimation.hpp"
#include "QcSceneMgr.hpp"
#include "QcSceneAnimationMgr.hpp"

ImplRuntimeClass2(QcGun, 100);

QcGun::QcGun()
{

}

QcGun::~QcGun()
{

}

void QcGun::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcSceneNode::Init(desc, bLoadSubNode);

	m_bFired = false;
	m_pNormolAnimator = static_cast<QcAnimatorTexture*>(MakeTextureAnimator(eLiving));
	m_pFireAnimator = static_cast<QcAnimatorTexture*>(MakeTextureAnimator(eDying));

	m_pBulletDesc = NULL;
}

void QcGun::Animate(double dCurrentTime,f32 fElapsedTime)
{
	//QcSceneNode::Animate(dCurrentTime, fElapsedTime);
	if (m_bFired)
		m_pFireAnimator->Animate(this, dCurrentTime, fElapsedTime);
	else
		m_pNormolAnimator->Animate(this, dCurrentTime, fElapsedTime);
}

void QcGun::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	QcSceneNode::Render(render, pWorldMatrix);
 	if (m_bFired && m_pFireAnimator->IsFinished())
 	{
 		m_bFired = false;
 		m_pNormolAnimator->Reset(0);
 	}
}

void QcGun::Fire(u32 iScore, QcPlayer* pPlayer)
{
	QcBullet* pBullet = NULL;
	MakeBullet(iScore, pBullet, pPlayer);
	//炮火位置 三种不同
	if (pPlayer->GetGunMgr().GetCurrentLevel()<=49 )
	{
		if (!IsSuperGun())
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pBullet->GetPosition(), 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		else
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X += 20 * cos(pBullet->GetRotation());
			pos.Y += 20 * sin(pBullet->GetRotation());
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		}		
	}
	else if (pPlayer->GetGunMgr().GetCurrentLevel()<=99)
	{
		if(!IsSuperGun())
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X -= 13 * cos(pBullet->GetRotation());
			pos.Y -= 13 * sin(pBullet->GetRotation());
			pos.X += 15 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y += 15 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 13 * cos(pBullet->GetRotation());
			pos.Y -= 13 * sin(pBullet->GetRotation());
			pos.X -= 15 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y -= 15 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		}
		else
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X += 20 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y += 20 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 20 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y -= 20 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		}
	}
	else
	{
		if (!IsSuperGun())
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X -= 13 * cos(pBullet->GetRotation());
			pos.Y -= 13 * sin(pBullet->GetRotation());
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 30 * cos(pBullet->GetRotation());
			pos.Y -= 30 * sin(pBullet->GetRotation());
			pos.X += 25 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y += 25 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.0f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 30 * cos(pBullet->GetRotation());
			pos.Y -= 30 * sin(pBullet->GetRotation());
			pos.X -= 25 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y -= 25 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.0f, pBullet->GetRotation() - HALF_PI);
		}
		else
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X -= 13 * cos(pBullet->GetRotation());
			pos.Y -= 13 * sin(pBullet->GetRotation());
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 30 * cos(pBullet->GetRotation());
			pos.Y -= 30 * sin(pBullet->GetRotation());
			pos.X += 25 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y += 25 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 30 * cos(pBullet->GetRotation());
			pos.Y -= 30 * sin(pBullet->GetRotation());
			pos.X -= 25 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y -= 25 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			//QcPoint pos = pBullet->GetPosition();
			//QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		}	
	}
	m_bFired = true;
	m_pFireAnimator->Reset(0);
	m_pFireAnimator->SetLivingTime(0.1f);

	QmSceneMgr->GetBulletMgr()->Add(pBullet);
}

bool QcGun::MakeBullet(u32 iScore, QcBullet* & pBullet, QcPlayer* pPlayer)
{
	const QcResGunDesc* pGunDesc = static_cast<const QcResGunDesc*>(GetDesc());
	if (m_pBulletDesc == NULL)
	{
		//查找子弹资源
        QcList<QcResImageDescPtr> &BulletDescList = QmGlobalData.gpResMgr->GetImageDesc(pGunDesc->GetBulletType())->GetSubDescList();
        m_pBulletDesc = BulletDescList.size()>pPlayer->GetID() ?  BulletDescList[pPlayer->GetID()] : BulletDescList[0];
	}

	//计算位置
	float fOriental = pPlayer->GetRotation() + GetRotation() + HALF_PI;
	QcMath::surroundRad(fOriental);
	QcPoint pos;
	pPlayer->GetMatrix().transformVect(pos, GetPosition());

	QcVector vec(1.f,0.f);
	vec.rotateByRad(fOriental);
	pos += vec * (GetHeight() - 20.f);

	pBullet = m_pBulletDesc->MakeInstance<QcBullet>();
	pBullet->SetRotation(fOriental);
	pBullet->SetPosition(pos);
	pBullet->LoadBulletInfo(pGunDesc, iScore, pPlayer);
	return true;
}

void QcGun::FreeFire( u32 iScore, QcPlayer* pPlayer )
{
	QcBullet* pBullet = NULL;
	MakeBullet(iScore, pBullet, pPlayer);
	//炮火位置 三种不同
	if (pPlayer->GetGunMgr().GetCurrentLevel()<=49 )
	{
		if (!IsSuperGun())
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pBullet->GetPosition(), 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		else
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X += 20 * cos(pBullet->GetRotation());
			pos.Y += 20 * sin(pBullet->GetRotation());
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		}		
	}
	else if (pPlayer->GetGunMgr().GetCurrentLevel()<=99)
	{
		if(!IsSuperGun())
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X -= 13 * cos(pBullet->GetRotation());
			pos.Y -= 13 * sin(pBullet->GetRotation());
			pos.X += 15 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y += 15 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 13 * cos(pBullet->GetRotation());
			pos.Y -= 13 * sin(pBullet->GetRotation());
			pos.X -= 15 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y -= 15 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		}
		else
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X += 20 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y += 20 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 20 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y -= 20 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		}
	}
	else
	{
		if (!IsSuperGun())
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X -= 13 * cos(pBullet->GetRotation());
			pos.Y -= 13 * sin(pBullet->GetRotation());
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 30 * cos(pBullet->GetRotation());
			pos.Y -= 30 * sin(pBullet->GetRotation());
			pos.X += 25 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y += 25 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.0f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 30 * cos(pBullet->GetRotation());
			pos.Y -= 30 * sin(pBullet->GetRotation());
			pos.X -= 25 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y -= 25 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "normalfire", pos, 1.f, 1.0f, pBullet->GetRotation() - HALF_PI);
		}
		else
		{
			QcPoint pos = pBullet->GetPosition();
			pos.X -= 13 * cos(pBullet->GetRotation());
			pos.Y -= 13 * sin(pBullet->GetRotation());
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 30 * cos(pBullet->GetRotation());
			pos.Y -= 30 * sin(pBullet->GetRotation());
			pos.X += 25 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y += 25 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			pos = pBullet->GetPosition();
			pos.X -= 30 * cos(pBullet->GetRotation());
			pos.Y -= 30 * sin(pBullet->GetRotation());
			pos.X -= 25 * cos(pBullet->GetRotation() - HALF_PI);
			pos.Y -= 25 * sin(pBullet->GetRotation() - HALF_PI);
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
			//QcPoint pos = pBullet->GetPosition();
			//QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "supperfire", pos, 1.f, 1.f, pBullet->GetRotation() - HALF_PI);
		}	
	}
	m_bFired = true;
	m_pFireAnimator->Reset(0);
	m_pFireAnimator->SetLivingTime(0.1f);
	pBullet->SetBugBullet();
	QmSceneMgr->GetBulletMgr()->Add(pBullet);
}
