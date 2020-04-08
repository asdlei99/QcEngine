#include "pch.hpp"
#include "QcRemoteBomberFish.hpp"
#include "QcFishMgr.hpp"
#include "QcFishFilter.hpp"
#include "Scene\QcBullet.hpp"
#include "Res\QcResDescParam.hpp"
#include "Scene\QcSceneAnimationMgr.hpp"

ImplRuntimeClass(QcRemoteBomberFish);

QcRemoteBomberFish::QcRemoteBomberFish()
{
}

void QcRemoteBomberFish::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcBombFish::Init(desc, bLoadSubNode);
}

void QcRemoteBomberFish::HitByBullet(QcBullet* pBullet)
{
	m_arBombList.Clear();

	QcBombFishFilter bombFilter;

	QmSceneMgr->GetFishMgr()->SearchHitableFish(m_arBombList, QmGlobalData.gScreenRect, &bombFilter);
	
#ifdef QmTestVersion
	//尽管QcBombFishFilter过滤器已经把炸弹类型排除了
	QmAssert(m_arBombList.Find(this) == -1);
#else
	QfErase(m_arBombList, this);
#endif
	QcBombFish::HitByBullet(pBullet);
}

void QcRemoteBomberFish::HitToDie( QcBullet* pBullet )
{
	//QcBombFish::HitToDie(pBullet);
    ToDie(false);
}
