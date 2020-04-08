#include "pch.hpp"
#include "QcScopeBombFish.hpp"
#include "QcFishMgr.hpp"
#include "QcFishFilter.hpp"
#include "Scene\QcBullet.hpp"
#include "Res\QcResDescParam.hpp"
#include "Scene\QcSceneAnimationMgr.hpp"
#include "EffectRender\DcWaterEffect.hpp"

ImplRuntimeClass(QcScopeBombFish);

QcScopeBombFish::QcScopeBombFish()
{
}

void QcScopeBombFish::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcBombFish::Init(desc, bLoadSubNode);
	
	m_bFullScreen = false;
	QcResDescParam* pParam = desc.GetParam();
	if (pParam)
	{
		m_fRadiu = pParam->GetProperty("ExpodeRadiu", 100.f);
		m_bFullScreen = QcMath::equals(m_fRadiu, 0.f);
	}
}

void QcScopeBombFish::HitByBullet(QcBullet* pBullet)
{
	m_arBombList.Clear();

	{
		QcBombFishFilter bombFilter;
		if (m_bFullScreen)
			QmSceneMgr->GetFishMgr()->SearchHitableFish(m_arBombList, QmGlobalData.gScreenRect, &bombFilter);
		else
			QmSceneMgr->GetFishMgr()->SearchHitableFish(m_arBombList, pBullet->GetPosition(), m_fRadiu, &bombFilter);
	}
	
#ifdef QmTestVersion
	//尽管QcBombFishFilter过滤器已经把炸弹类型排除了
	QmAssert(m_arBombList.Find(this) == -1);
#else
	QfErase(m_arBombList, this);
#endif
	QcBombFish::HitByBullet(pBullet);
}

void QcScopeBombFish::HitToDie( QcBullet* pBullet )
{
    QcBombFish::HitToDie(pBullet);    
    /*DcWaterEffect *pEffect = QmSceneMgr->GetAnimationMgr()->AddEffect(eAnimationLayerEffect, "Water")->As<DcWaterEffect*>();
    pEffect->SetRenderTime(2);
    pEffect->SetWaterForce(1);
    pEffect->SetWaterDamp(0.99f);
    pEffect->SetUpdateTick(0.005f);
    pEffect->SetWaterPos(D3DXVECTOR3(GetPosition().X, GetPosition().Y, 0));*/
}
