#include "pch.hpp"
#include "QcFishMgr.hpp"
#include "QcFish.hpp"
#include "QcFishFilter.hpp"
#include "QcSameKindBombFish.hpp"
#include "Scene\QcBullet.hpp"
#include "Scene\QcPlayer.hpp"
#include "Res\QcResImageListDesc.hpp"
#include "Res\QcResImageDesc.hpp"
#include "Regular\QcRegularMgr.hpp"
#include "Regular\QiSceneRegularMaker.hpp"
#include "Collision\QcRegionCircle.hpp"

#define QmLivingState (eLiving | ePlaying | eLivingToDown | eDowning | eDownToLiving | eLivingToUp | eUping | eUpToLiving)

struct QsFishSorter
{
	bool operator ()(const QcFish* left, const QcFish* right) const
	{
		if (left && right)
			return left->GetDepth() < right->GetDepth();
		return left != NULL;
	}
};


QcFishMgr::QcFishMgr()
{
	m_bInFrozen = false;
	m_bSceneNormal = true;
	m_nFishCount = 0;
	m_pSceneRegularMaker = NULL;
	m_fRegularTime = 0.f;
}

QcFishMgr::~QcFishMgr()
{

}

void QcFishMgr::Init(QcResManager* pResMgr, const char* psConfig)
{
	m_oControlInfo.Init(this, psConfig);
	m_pSceneRegularMaker = pResMgr->GetRegularMgr();
	m_nFishCount = 0;
	Reset(false);
}

void QcFishMgr::Reset(bool bFishControlEnable)
{
	m_arHitableFishList.clear();
	m_bSceneNormal = bFishControlEnable;
	if (!m_bSceneNormal)
	{
		for (u32 iLayer=0; iLayer<QmCountOf(m_arFishList); ++iLayer)
		{
			QcFishList& fishList = GetFishList(iLayer);
			fishList.Clear();
		}
		m_arBottomFish.Clear();
		m_nFishCount = 0;
	}
	m_oControlInfo.Reset(bFishControlEnable);

	if (m_bSceneNormal)
	{
		m_oControlInfo.PostEvent(eEvtForceFillFish, 0, true);
		m_oControlInfo.PostEvent(eEvtFillFish, 0, true);
		SetTimer(eFishGenerateTimer, m_oControlInfo.GetFishFillTime());
	}
	else
	{
		KillTimer(eFishGenerateTimer);
	}
}

void QcFishMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QmProfileFragment(QcFishMgr::Animate);
	m_arHitableFishList.clear();

	if (m_pCurrentSceneRegularMaker)
	{
		if (!m_pCurrentSceneRegularMaker->IsFinished())
		{
			QcList<QcFish*> fishList;
			m_pCurrentSceneRegularMaker->Animate(dCurrentTime, fElapsedTime, fishList);
			if (fishList.size())
			{
				QcList<QcFish*>::iterator iter(fishList.begin());
				QcList<QcFish*>::iterator endIter(fishList.end());
				for (; iter != endIter; ++iter)
				{
					(*iter)->SetFishFlag((*iter)->GetFishFlag() | QcFish::eSceneRegular);
				}
				AddFish(fishList);
			}
			m_fRegularTime += fElapsedTime;
		}

		if (m_pCurrentSceneRegularMaker->IsFinished()
#ifndef QmTestVersion
			|| m_fRegularTime > 2.0f * 60.f
#endif
			)
		{
			m_pCurrentSceneRegularMaker->EndSceneRegular();
			m_pCurrentSceneRegularMaker = NULL;
			QmSceneMgr->PostEvent(eEvtSceneStateChange, QcSceneMgr::eSceneNormal);
		}
	}

	for (u32 iLayer=0; iLayer<QmCountOf(m_arFishList); ++iLayer)
	{
		Animate(GetFishList(iLayer), dCurrentTime, fElapsedTime);
	}
	Animate(m_arBottomFish, dCurrentTime, fElapsedTime);
	std::sort(m_arBottomFish.begin(), m_arBottomFish.end(), QsFishSorter());
    Animate(m_arTopFish, dCurrentTime, fElapsedTime);
}

void QcFishMgr::Animate(QcFishList& fishList, double dCurrentTime,f32 fElapsedTime)
{
	if (fishList.size())
	{
		QcFishListIter iter(fishList.begin());
		QcFishListIter endIter(fishList.end());
		for (; iter != endIter; ++iter)
		{
			QcFish* pFish = (*iter);
			if (pFish)
			{
				pFish->Animate(dCurrentTime, fElapsedTime);
				if (pFish->IsLiving() && pFish->IsVisible() && !pFish->IsSheltered() && pFish->IsReady() && pFish->IsIntersectScreen())
					m_arHitableFishList.push_back(pFish);
			}
		}
	}
}

bool QcFishMgr::HandleCollision(QcBulletPtrList& bulletList)
{
	QmProfileFragment(HandleCollision);
	QcBullet* pBullet = NULL;
	QcBulletPtrListIter iter(bulletList.begin());
	QcBulletPtrListIter endIter(bulletList.end());
	for (; iter != endIter; ++iter)
	{
		pBullet = *iter;
		if (pBullet)
		{
			const QcFish* pTargetFish = pBullet->GetTargetFish();
			QcFish* pFish = NULL;
			std::vector<QcFish*>::iterator fishIter(m_arHitableFishList.begin());
			std::vector<QcFish*>::iterator fishEndIter(m_arHitableFishList.end());
			for (; fishIter != fishEndIter; ++fishIter)
			{
				pFish = *fishIter;
				if (pFish->IsLiving())
				{
					if (pTargetFish && pTargetFish != pFish)
						continue;

					if (pBullet->Intersect(pFish))
					{
						pBullet->GetOwner()->HitFish(pBullet, pFish);
						break;
					}
				}
			}
		}
	}
	return false;
}

void QcFishMgr::Render(QcDeviceRender& render, u32 iLayer)
{
	switch(iLayer)
	{
	case 0:
		{
			Render(render, m_arBottomFish, true, 1);
			break;
		}
	case 1:
		{
			for (u32 id=0; id<QmCountOf(m_arFishList); ++id)
			{
				Render(render, GetFishList(id), true, 1 + id);
			}
			break;
		}
	case 2:
		{
			Render(render, m_arBottomFish, false, 3 + QmCountOf(m_arFishList));
			for (u32 id=0; id<QmCountOf(m_arFishList); ++id)
			{
				Render(render, GetFishList(id), false, 5 + QmCountOf(m_arFishList) + id);
			}
			break;
		}

    case 3:
        {
            Render(render, m_arTopFish, false, 7 + QmCountOf(m_arFishList));
        }
        break;

    default:
        break;
	}
}

void QcFishMgr::Render(QcDeviceRender& render, QcFishList& fishList, bool bLiving, u32 iLayer)
{
	{
		static int iClear = -1;
		if (iClear == giScreenHandleing)
		{
			fishList.Clear();
		}

		QmAutoRevertAlpha(render);
		//render.BeginCaptureShadow(iLayer);
		QcFishListIter listIter(fishList.begin());
		QcFishListIter endIter(fishList.end());
		for (; listIter != endIter; ++listIter)
		{
			if (*listIter && (*listIter)->IsLiving() == bLiving )
			{
				(*listIter)->Render(render);
			}
		}
		
	}
	//render.EndCaptureShadow();
}

void QcFishMgr::OnDead(QcFish* pFish)
{
	if (pFish->GetFishFlag() & QcFish::eCalController)
	{
		m_oControlInfo.OnDead(pFish);
		pFish->SetFishFlag(pFish->GetFishFlag() & ~QcFish::eCalController);
	}
	if ((pFish->GetFishFlag() & QcFish::eSceneRegular))
	{
		if (m_pCurrentSceneRegularMaker)
			m_pCurrentSceneRegularMaker->OnDead(pFish);
		pFish->SetFishFlag(pFish->GetFishFlag() & ~QcFish::eSceneRegular);
	}
	RemoveFish(pFish);
}

void QcFishMgr::RemoveFish(const QcFish* pFish)
{
	QcFishList* pFishList = NULL;
	if (pFish->GetFishFlag() & QcFish::eBottom)
		pFishList = &m_arBottomFish;
	else
		pFishList = &(GetFishList(pFish->GetLayer()));

	int iPos = pFishList->Find(pFish);
	if (iPos != -1)
	{
		pFishList->EraseAt(iPos);
		--m_nFishCount;
	}
	else
	{
		int iPos = m_arBottomFish.Find(pFish);
		if (iPos != -1)
		{
			m_arBottomFish.EraseAt(iPos);
			QmLog("Fish Layer Have been changed.");
			--m_nFishCount;
		}
		else
		{
            iPos = m_arTopFish.Find(pFish);
            if (iPos != -1)
            {
                m_arTopFish.EraseAt(iPos);
            }
            else
            {
                for (u32 iLayer=0; iLayer<QmCountOf(m_arFishList); ++iLayer)
                {
                    QcFishList& fishList = GetFishList(iLayer);
                    iPos = fishList.Find(pFish);
                    if (iPos != -1)
                    {
                        fishList.EraseAt(iPos);
                        QmLog("Fish Layer Have been changed.");
                        --m_nFishCount;
                        break;
                    }
                }
            }
		}
	}
}

void QcFishMgr::AddFish(QcList<QcFish*>& fishList)
{
	QcList<QcFish*>::iterator iter(fishList.begin());
	QcList<QcFish*>::iterator endIter(fishList.end());
	for (; iter != endIter; ++iter)
	{
		QcFish* pFish = (*iter);
		if (pFish->GetFishFlag() & QcFish::eBottom)
			m_arBottomFish.Insert(pFish);
		else
			GetFishList(pFish->GetLayer()).Insert(pFish);
	}
	m_nFishCount += fishList.size();
}

void QcFishMgr::BeginFrozen(f32 fTime)
{
	m_bInFrozen = true;
	Frozen(true);
	SetTimer(eFishFrozenTimer, fTime, true);
}

void QcFishMgr::EndFrozen()
{
	m_bInFrozen = false;
	Frozen(false);
}

void QcFishMgr::Frozen(bool bFrozen)
{
	for (u32 id=0; id<QmCountOf(m_arFishList); ++id)
	{
		QcFishList& fishList = GetFishList(id);
		QcFishListIter listIter(fishList.begin());
		QcFishListIter listEndIter(fishList.end());
		for (; listIter != listEndIter; ++listIter)
		{
			if (*listIter && (*listIter)->IsLiving())
			{
				(*listIter)->Frozen(bFrozen);
			}
		}
	}

	QcFishList& fishList = m_arBottomFish;
	QcFishListIter listIter(fishList.begin());
	QcFishListIter listEndIter(fishList.end());
	for (; listIter != listEndIter; ++listIter)
	{
		if (*listIter && (*listIter)->IsLiving())
		{
			(*listIter)->Frozen(bFrozen);
		}
	}
}

void QcFishMgr::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eFishGenerateTimer)
	{
		if (!m_bInFrozen && m_bSceneNormal)
		{
			m_oControlInfo.PostEvent(eEvtFillFish);
		}
	}
	else if (nTimer == eFishLockFillTimer)
	{
		m_oControlInfo.LockFishFill(false);
	}
	else if (nTimer == eFishFrozenTimer)
	{
		EndFrozen();
		if (m_bSceneNormal)
		{
			m_oControlInfo.PostEvent(eEvtForceFillFish);
			m_oControlInfo.PostEvent(eEvtFillFish);
		}
	}
}

const QcObjType& QcFishMgr::BeginSceneRegular(const QcObjType& objType)
{
	m_fRegularTime = 0.f;
	QcList<QcFish*> fishList;
	m_pCurrentSceneRegularMaker = GetSceneRegularMaker()->BeginSceneRegular(fishList, objType);

	QcList<QcFish*>::iterator iter(fishList.begin());
	QcList<QcFish*>::iterator endIter(fishList.end());
	for (; iter != endIter; ++iter)
	{
		(*iter)->SetFishFlag((*iter)->GetFishFlag() | QcFish::eSceneRegular);
	}
	AddFish(fishList);
	return m_pCurrentSceneRegularMaker->GetObjType();
}

QcFish* QcFishMgr::FindTargetFish()
{	
	QcList<QcFish*> targetFishList;

	std::vector<QcFish*>::iterator listIter(m_arHitableFishList.begin());
	std::vector<QcFish*>::iterator listEndIter(m_arHitableFishList.end());
	for (; listIter != listEndIter; ++listIter)
	{
		QcFish* pFish = (*listIter);
		if (pFish 
			&& pFish->IsLiving() 
			&& (pFish->GetScore() > 10 || pFish->IsBombFish() || pFish->IsChainLinkFish() || pFish->GetObjType() == QmObjName(GambleFish)) 
			&& (!pFish->IsSheltered()) 
			&& QmSysSetting->GetTrackedBox().isPointInside(pFish->GetPosition()) )
		{
			targetFishList.push_back(pFish);
		}
	}
	return FindTargetFish(targetFishList);
}

QcFish* QcFishMgr::FindTargetFish(QcList<QcFish*>& fishList)
{
	QcFish* pFish = NULL;
	if (fishList.size())
	{
		pFish = fishList[RandomInt(0, fishList.size())];
	}
	return pFish;
}

u32 QcFishMgr::SearchHitableFish(QcList<QcFish*>& fishList, const QcPoint& pos, f32 fRadiu, QcFishFilter* excluder)
{	
	QcRegionCircle circle(pos, fRadiu);
	return SearchHitableFish(fishList, &circle, excluder);
}

u32 QcFishMgr::SearchHitableFish(QcList<QcFish*>& fishList, const QcRegion* pRegion, QcFishFilter* excluder)
{
	std::vector<QcFish*>::iterator iter(m_arHitableFishList.begin());
	std::vector<QcFish*>::iterator endIter(m_arHitableFishList.end());
	for (; iter != endIter; iter++)
	{
		QcFish* pFish = *iter;
		if (pFish)
		{
			if (pFish->IsLiving() && pFish->IsIntersectScreen() && pFish->Intersect(pRegion))
			{
				if (excluder == NULL || !excluder->filter(pFish))
					fishList.push_back(pFish);
			}
		}
	}
	return fishList.size();
}
u32 QcFishMgr::SearchHitableFish(QcList<QcFish*>& scopeFish, const QcRect& rect, QcFishFilter* excluder)
{
	std::vector<QcFish*>::iterator iter(m_arHitableFishList.begin());
	std::vector<QcFish*>::iterator endIter(m_arHitableFishList.end());
	for (; iter != endIter; iter++)
	{
		QcFish* pFish = *iter;
		if (pFish)
		{
			if (pFish->IsLiving() && pFish->IsIntersectScreen() && rect.isPointInside(pFish->GetPosition()))
			{
				if (excluder == NULL || !excluder->filter(pFish))
					scopeFish.push_back(pFish);
			}
		}
	}
	return scopeFish.size();
}

void QcFishMgr::HoistFishToTop( const QcFish *pFish )
{
    m_arTopFish.Insert(pFish);
    RemoveFish(pFish);
}

bool QcFishMgr::CanGenSameKindFish(const QcResFishDesc* pFishDesc)
{
	return m_oControlInfo.CanGenSameKindFish(pFishDesc);
}

void QcFishMgr::ToDeadGenSameKindFish(const QcResFishDesc* pFishDesc)
{
	m_oControlInfo.ToDeadGenSameKindFish(pFishDesc);
}

QcSameKindBombFish* QcFishMgr::MakeSameKindBombFish(const QcResFishDesc* pFishDesc)
{
	return m_oControlInfo.MakeSameKindBombFish(pFishDesc);
}