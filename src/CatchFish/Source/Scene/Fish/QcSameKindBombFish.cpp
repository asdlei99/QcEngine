#include "pch.hpp"
#include "QcSameKindBombFish.hpp"
#include "QcFishMgr.hpp"
#include "QcFishFilter.hpp"
#include "Scene\QcSceneAnimationMgr.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "Res\QcResDescParam.hpp"
#include "Res\QcResFishDesc.hpp"
#include "Animator\QcAnimatorRotation.hpp"
#include "Animator\QcAnimatorDither.hpp"
#include "Animator\QcAnimatorMoveCircle.hpp"
#include "MP_Particle\3d.h"
#include "Animator\QcAmimatorCyclone.hpp"
#include "Animator\QcAnimatorScale.hpp"
#include "Animator\QiAnimatorDeque.hpp"
#include "Scene\QcDigitalNode.hpp"
#include "Animator\QcAnimatorAlpha.hpp"
#include "..\QcBackGroundMgr.hpp"

#define XuanFengYuMovingFlag (1)

ImplRuntimeClass(QcSameKindBombFish);

QcSameKindBombFish* QcSameKindBombFish::MakeSameKindBombFish(const QcResFishDesc* pFishDesc)
{
	return QmSceneMgr->GetFishMgr()->MakeSameKindBombFish(pFishDesc);
}

bool QcSameKindBombFish::CanGenSameKindFish(const QcResFishDesc* pFishDesc)
{
	return QmSceneMgr->GetFishMgr()->CanGenSameKindFish(pFishDesc);
}

QcSameKindBombFish::QcSameKindBombFish(QcResFishDesc* pFishInfo)
: m_pFishInfo(pFishInfo)
{
}

QcSameKindBombFish::~QcSameKindBombFish()
{
	if (m_pFishInfo != NULL)
	{
		QmSceneMgr->GetFishMgr()->ToDeadGenSameKindFish(m_pFishInfo);
		m_pFishInfo = NULL;
	}
	m_arBombList.Clear();
	m_arSameKindFishInfoList.clear();
}

void QcSameKindBombFish::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eXuanFengYuDieTimer)
	{
		QcList<QsSameKindFishInfo>::iterator iter(m_arSameKindFishInfoList.begin());
		QcList<QsSameKindFishInfo>::iterator endIter(m_arSameKindFishInfoList.end());
		for (; iter !=endIter; ++iter)
		{
			QcFish* pFish = iter->m_pFish;
			if (pFish != this)
				pFish->ToDead();
		}
		m_arSameKindFishInfoList.Clear();
		
		ToDead();
		
		u32 iScore = m_iExplodeScore * m_pBullet->GetLevel() *  m_pBullet->GetMultiple();
		GetOwner()->HitAddScore(iScore, iScore);
		QmSceneMgr->LockNormalScene(false);
	}
	else if (nTimer == eSameKindFishWaitToExploseTimer)
	{
		Explose();
	}
	else
	{
		QcBombFish::OnTimer(nTimer, pContext);
	}
}

void QcSameKindBombFish::ToDead()
{
	if (m_pFishInfo != NULL)
	{
		QmSceneMgr->GetFishMgr()->ToDeadGenSameKindFish(m_pFishInfo);
		m_pFishInfo = NULL;
	}
	QcBombFish::ToDead();
}

void QcSameKindBombFish::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QmAssert(m_pFishInfo != NULL);
	QcBombFish::Init(*(m_pFishInfo), bLoadSubNode);

	m_nID = static_cast<const QcResFishDesc&>(desc).GetFishType();
	m_objType = desc.GetObjType();

	if (desc.GetEffectRender().size())
		m_pEffectRender = QmResMgr->MakeEffect(desc.GetEffectRender().c_str());
	SetScore(m_pFishInfo->GetScore());

    m_pRing = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(BloomRing));
    f32 fSize = QcMath::max_(m_pFishInfo->GetWidth(), m_pFishInfo->GetHeight())+150;
    m_pRing->ScaleTo(fSize, fSize);
}

void QcSameKindBombFish::HitByBullet(QcBullet* pBullet)
{
	m_arBombList.Clear();
	QcSameKindBombFishFilter sameKindFilter(m_pFishInfo->GetFishType());
	QmSceneMgr->GetFishMgr()->SearchHitableFish(m_arBombList, QmGlobalData.gScreenRect, &sameKindFilter);
	QfErase(m_arBombList, this);
	QcBombFish::HitByBullet(pBullet);
}

void QcSameKindBombFish::HitToDie(QcBullet* pBullet)
{
	//锁定场景变化
	QmSceneMgr->LockNormalScene(true);
	m_pBullet = pBullet;
	m_pOwner = m_pBullet->GetOwner();
	m_pEffectRender = NULL;

	ToDieFishList(false);
	MakeTornadoAnimation();
	//QmSoundMgr->Play(QmSoundType(SameKindFishDyingSound));
	QmPlaySound(SameKindFishDyingSound);
    QmSceneMgr->GetBgMgr()->Dither(2);
}

void QcSameKindBombFish::MakeTornadoAnimation()
{
	QcResManager* pResMgr = QmResMgr;

	m_vDestPos = GetOwner()->GetPosition() + 300.f * GetOwner()->GetVertical();

	static QcResImageDesc* pImageDesc = pResMgr->GetImageDesc(QmObjType(xuanfeng));
	m_pXuanFengAnimation = QfMakeSceneNodeInstance<QcSceneAnimation>(pImageDesc);
	m_pXuanFengAnimation->SetPosition(m_vDestPos);
	QiAnimator* pAnimator = QmAnimatorCreator->MakeAlphaAnimator2(0.0f, 1.0f, 0.5f);
	m_pXuanFengAnimation->AddAnimator(pAnimator);
	m_pXuanFengAnimation->SetCurrentTransparency(0);

    QiAnimatorDeque *pAniDeq = QmAnimatorCreator->MakeAnimatorDeque();
    pAnimator = QmAnimatorCreator->MakeScaleAnimator2(0.6f, 1.5f, 0.8f);
    pAniDeq->AddAnimator(pAnimator);
    pAnimator = QmAnimatorCreator->MakeScaleAnimator2(1.5f, 0.8f, 1.5f);
    pAnimator->SetStartTime(0.5f);
    pAniDeq->AddAnimator(pAnimator);
    m_pXuanFengAnimation->AddAnimator(pAniDeq);
    m_pXuanFengAnimation->SetScale(0.6f);
    m_pXuanFengAnimation->SetFinishFlag(pAniDeq);

	Drag();
}

void QcSameKindBombFish::Drag()
{
	m_iFinishedCount = 0;
	if (m_arBombList.empty())
	{
		m_arBombList.push_back(this);
		SetVisible(true);
		m_bShowedCoin = false;
		m_iExplodeScore = 0;
	}
	else
	{
		m_iExplodeScore = GetScore();
		SetVisible(false);
		m_bShowedCoin = true;
	}

#ifndef QmTestVersion
	SetTimer(eSameKindFishWaitToExploseTimer, 4.0f + QmGlobalData.gScreenRect.getWidth()/500.0f, true);
#endif

    QcResManager *&pResMgr = QmGlobalData.gpResMgr;
    static QcResImageDesc* pImageDesc = pResMgr->GetImageDesc(QmObjType(xuanfeng));
	u32 idx = 0;
	QcList<QcFish*>::iterator iter(m_arBombList.begin());
	QcList<QcFish*>::iterator endIter(m_arBombList.end());
	m_arSameKindFishInfoList.reserve(m_arBombList.size());
	for (; iter != endIter; ++iter, ++idx)
	{
		QcFish* pFish = *iter;
		m_iExplodeScore += pFish->GetScore();

		m_arSameKindFishInfoList.push_back(QsSameKindFishInfo());
		QsSameKindFishInfo& sameKindFishInfo = m_arSameKindFishInfoList.back();
		sameKindFishInfo.m_pFish= pFish;

        QcSceneNode *pNode = pImageDesc->MakeInstance();
        pNode->SetScale((QcMath::max_(pFish->GetWidth(),pFish->GetHeight()) + 100)/pImageDesc->GetWidth());
        pFish->SetSpecialFlag(pNode);

		QcVector vec(m_vDestPos, pFish->GetPosition());
		f32 fLen = vec.getLength();
		vec.normalize(fLen);
		if (fLen < 150.f)
		{
			FinishAnimator(idx);
		}
		else
		{
			QiAnimator* pAnimator = QmAnimatorCreator->MakeLineAnimator2(pFish->GetPosition(), m_vDestPos + vec * 150.f, 500.0f);
			pAnimator->AddFinishCallBack(QcFinishAnimatorCb(boost::bind(&QcSameKindBombFish::FinishAnimator, this, _1)), idx);
			pFish->AddAnimator(pAnimator);
			pFish->AddAnimator(QmAnimatorCreator->MakeRotationAnimator(TWO_PI));
		}
		
		if (pFish != this)
			QmSceneMgr->GetFishMgr()->RemoveFish(pFish);
	}
	m_arBombList.Clear();
}

void QcSameKindBombFish::FinishAnimator(u32 iParam)
{
	MakeIce(iParam);
	if (m_iFinishedCount == m_arSameKindFishInfoList.size())
	{
		SetTimer(eSameKindFishWaitToExploseTimer, 4.0f, true);
	}
}

void QcSameKindBombFish::MakeIce(u32 idx)
{
	QsSameKindFishInfo& info = m_arSameKindFishInfoList[idx];

	info.m_pFish->RemoveAnimator(NULL);
	QcAnimatorCyclone*pCycloneAnimator = static_cast<QcAnimatorCyclone*>(QmAnimatorCreator->MakeCycloneAnimator(m_vDestPos, Randomf(PI * 0.8f, PI * 1.2f), 0.f - Randomf(40.f, 60.f), 0.f, 0.f, 40.f, 0.f, false,false));
	info.m_pFish->AddAnimator(pCycloneAnimator);

	if (info.m_pFish->GetScore() > 2)
	{
		info.m_pFish->SetScale(1.0f);
		QcAnimatorScale* pFishScale = static_cast<QcAnimatorScale*>(QmAnimatorCreator->MakeScaleAnimator(1.0f,0.7f,-0.5f));
		info.m_pFish->AddAnimator(pFishScale);
	}

	{
		QcResFishDesc* pDesc = const_cast<QcResFishDesc*>(this->GetFishDesc());
		const QsStateParameter* pStateParam = pDesc->GetStateParameter(eDying);
		QcAnimatorTexture* pFishTexture = static_cast<QcAnimatorTexture*>(QmAnimatorCreator->MakeTextureAnimator(pStateParam));
		pFishTexture->SetLoop(true);
		pFishTexture->SetTimePerFrame(0.01f);

		
		QcSceneAnimation* pScoreAnimation = QfMakeEmptyInstance<QcSceneAnimation>();
		pScoreAnimation->SetRenderState(QmMakeTexState(gNumTexState));
		//pScoreAnimation->SetDigitalType(QmObjType(getscore));
		QmSetDigitalType(pScoreAnimation->GetDigitalNode(), getscore);
		pScoreAnimation->SetScale(0.4f);
		QcPoint Score_pos = info.m_pFish->GetPosition();
		Score_pos += GetOwner()->GetVertical() * 50.f;
		pScoreAnimation->SetPosition(Score_pos);
		pScoreAnimation->SetRotation(GetOwner()->GetRotation());

		pScoreAnimation->SetDigitalValue(GetScore() * m_pBullet->GetLevel() *  m_pBullet->GetMultiple());
		pScoreAnimation->SetDigitalVisible(true);
		pScoreAnimation->GetDigitalNode().SetColor(GetOwner()->GetColorFlag());

		info.m_pScore = pScoreAnimation;
		info.m_pScore->AddAnimator(pCycloneAnimator);
		info.m_pScore->SetFinishFlag(pCycloneAnimator);

		//info.m_pFish->AddSubSceneNode(pScoreAnimation);

		info.m_pFish->AddAnimator(pFishTexture);

        QcSceneNode *pXuanFeng = info.m_pFish->GetSpecialFlag();
        QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(pXuanFeng->GetScaleX(), 0, 0.3f);
        pXuanFeng->AddAnimator(pAni);
	}

	++m_iFinishedCount;
}

void QcSameKindBombFish::Explose()
{
	QcList<QsSameKindFishInfo>::iterator iter(m_arSameKindFishInfoList.begin());
	QcList<QsSameKindFishInfo>::iterator endIter(m_arSameKindFishInfoList.end());
	for (; iter !=endIter; ++iter)
	{
        iter->m_pFish->ExploseToDie(this);
		iter->m_pFish->MakeCoinAnimation();
		iter->m_pFish->SetVisible(false);
		iter->m_pScore = NULL;
	}
	QiAnimator* pAnimator = QmAnimatorCreator->MakeScaleAnimator(m_pXuanFengAnimation->GetScaleX(), 0.0f, -1.0f);
	m_pXuanFengAnimation->AddAnimator(pAnimator);
	m_pXuanFengAnimation->SetFinishFlag(pAnimator);

	SetTimer(eXuanFengYuDieTimer, 1.f /*+ 1.0f*/, true);
}

void QcSameKindBombFish::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcBombFish::Animate(dCurrentTime, fElapsedTime);
	if (!IsLiving())
	{
		if (m_pXuanFengAnimation)
			m_pXuanFengAnimation->Animate(dCurrentTime, fElapsedTime);
		QcList<QsSameKindFishInfo>::iterator iter(m_arSameKindFishInfoList.begin());
		QcList<QsSameKindFishInfo>::iterator endIter(m_arSameKindFishInfoList.end());
		for (; iter != endIter; ++iter)
		{
			if (this != iter->m_pFish)
				iter->m_pFish->Animate(dCurrentTime, fElapsedTime);
			if (iter->m_pScore)
				iter->m_pScore->Animate(dCurrentTime, fElapsedTime);
		}
	}

    m_pRing->Animate(dCurrentTime, fElapsedTime);
}

void QcSameKindBombFish::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (!IsLiving())
	{
		QmAutoSuspendShadow(render);
		/*if (m_pFlagAnimation)
			m_pFlagAnimation->Render(render);
		if (m_pScoreAnimation)
			m_pScoreAnimation->Render(render);*/
		if (m_pXuanFengAnimation)
		{
			m_pXuanFengAnimation->Render(render);
			//m_pXuanFengEffect->Render();
		}
		QcList<QsSameKindFishInfo>::iterator iter(m_arSameKindFishInfoList.begin());
		QcList<QsSameKindFishInfo>::iterator endIter(m_arSameKindFishInfoList.end());
		for (; iter != endIter; ++iter)
		{
			if (iter->m_pFish != this)
				iter->m_pFish->Render(render);
		}

		QcBombFish::Render(render, pWorldMatrix);

		iter = m_arSameKindFishInfoList.begin();
		for (; iter != endIter; ++iter)
		{
			if (iter->m_pScore)
			{
				iter->m_pScore->Render(render);
			}
		}
	}
	else
	{
		QcBombFish::Render(render, pWorldMatrix);
        if (IsVisible() && IsReady())
            m_pRing->Render(render, pWorldMatrix ? &(*pWorldMatrix*GetMatrix()) : &GetMatrix());
	}
}

const QcObjType& QcSameKindBombFish::GetFishKind() const
{
    return m_pFishInfo->GetObjType();
}
