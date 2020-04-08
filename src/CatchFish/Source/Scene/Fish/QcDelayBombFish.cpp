#include "pch.hpp"
#include "QcDelayBombFish.hpp"
#include "Scene\QcPlayer.hpp"
#include "Scene\QcBullet.hpp"
#include "Scene\QcDelayExplodeAnimation.hpp"
#include "Scene\QcSceneAnimationMgr.hpp"
#include "Scene\QcSceneMgr.hpp"
#include "Res\QcResDescParam.hpp"
#include "EffectRender\DcWaterEffect.hpp"
#include "Scene\QcBackGroundMgr.hpp"
#include "Scene\Fish\QcFishMgr.hpp"
#include "Scene\Fish\QcFishFilter.hpp"

ImplRuntimeClass(QcDelayBombFish);

#define BOOM_ZOOM_TIMER     10001

QcDelayBombFish::QcDelayBombFish()
{
}

void QcDelayBombFish::Init(const QcResImageDesc& desc, bool bLoadSubNode /* = false */)
{
	QcScopeBombFish::Init(desc, bLoadSubNode);
	m_fParticleTime = 0.f;
}

void QcDelayBombFish::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcScopeBombFish::Animate(dCurrentTime, fElapsedTime);
	if (!IsLiving())
	{
		m_fParticleTime += fElapsedTime;
		if (m_fParticleTime >= 1.5f)
		{
			m_fParticleTime -= 1.5f;
			//QmAnimationMgr->AddParticleMP(eAnimationLayerDyingFish, "全屏炸弹爆炸前", GetPosition());
		}
		
	}
}

void QcDelayBombFish::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eDelayTimer)
	{
		Explose();
		QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish,"DelayBomb4",GetPosition());
        //QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerDyingFish,
            //"Jububomb", GetPosition(), 1, 1.5f);
        QmSceneMgr->GetBgMgr()->Dither(2.f, 15);
		QmSceneMgr->LockNormalScene(false);
		//////////////////////////////////////////////////////////////////////////
	}
    else if (nTimer == BOOM_ZOOM_TIMER)
    {
        QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish,"DelayBomb3",GetPosition());
        SetScale(1.5f);
        QmSceneMgr->GetBgMgr()->Dither(1.f, 10);
    }
	else
	{
		QcScopeBombFish::OnTimer(nTimer, pContext);
	}
}

void QcDelayBombFish::HitToDie(QcBullet* pBullet)
{
	m_pBullet = pBullet;
	m_pOwner = m_pBullet->GetOwner();

	m_iExplodeScore = GetExplodeScore();
	
	//锁定场景变化
	QmSceneMgr->LockNormalScene(true);
	QcResDescParam* pParam = GetDesc()->GetParam();
	f32 fDelayTime = pParam->GetProperty("DelayTime", 2.f);
	QcFish::ToDie(false);
    SetTimer(eDelayTimer, (f32)fDelayTime , true);
    SetTimer(BOOM_ZOOM_TIMER, 1.5f , true);

	QcDelayExplodeAnimation* pAnimation = QfMakeEmptyInstance<QcDelayExplodeAnimation>();
	f32 fDyingTime = QcMath::min_(GetDesc()->GetDyingTime(), 1.f);
	pAnimation->SetLivingTime(fDelayTime + fDyingTime);
	QmAnimationMgr->AddAnimation(pAnimation, eAnimationLayerSpecial);

	//QmSoundMgr->Play(QmSoundType(DelayBombSound));
	if((rand()%2) == 0)
		QmPlaySound(BombBefore);
	else
		QmPlaySound(BombBefore2);
	m_fParticleTime = 0.f;
    QmAnimationMgr->AddParticleMP(eAnimationLayerDyingFish, "DelayBomb2", GetPosition());
    QmSceneMgr->GetBgMgr()->Dither(1.f, 5);

    QmSceneMgr->GetFishMgr()->HoistFishToTop(this);
}

void QcDelayBombFish::Explose()
{
	SetVisible(false);
    m_iBombedDelayFishCount = 0;
	QcList<QcFish*> DieFishList;
	{
		QcDelayBombFishFilter delayBombFilter(m_iExplodeScore);
		if (IsFullScreen())
		{
			QmSceneMgr->GetFishMgr()->SearchHitableFish(DieFishList, QmGlobalData.gScreenRect, &delayBombFilter);
		}
		else
		{
			QmSceneMgr->GetFishMgr()->SearchHitableFish(DieFishList, GetPosition(), GetExpodeRadiu(), &delayBombFilter);
		}
	}
	
	QcList<QcFish*>::iterator iter(DieFishList.begin());
	QcList<QcFish*>::iterator endIter(DieFishList.end());
	for (; iter != endIter; ++iter)
	{
        QcFish *&pFish = *iter;
		pFish->ExploseToDie(this);
        if (pFish->IsDelayCalScore())
        {
            ++m_iBombedDelayFishCount;
            pFish->AddMonitor(this);
        }
	}

    if (m_iBombedDelayFishCount == 0)
    {
        m_bShowedCoin = true;
        ToDead();

        u32 iScore = m_iExplodeScore * m_pBullet->GetLevel() *  m_pBullet->GetMultiple();
        GetOwner()->HitAddScore(iScore, iScore, 7.f);
    }
	//QmSoundMgr->Play(QmSoundType(ScreenBombSound));
	QmPlaySound(ScreenBombSound);
}

void QcDelayBombFish::OnRemoveNodeMonitor( QcSceneNode* )
{
    if (--m_iBombedDelayFishCount <= 0)
    {
        m_bShowedCoin = true;
        ToDead();

        u32 iScore = m_iExplodeScore * m_pBullet->GetLevel() *  m_pBullet->GetMultiple();
        GetOwner()->HitAddScore(iScore, iScore, 7.f);
    }
}


