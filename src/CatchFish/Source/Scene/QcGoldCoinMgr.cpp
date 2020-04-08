#include "pch.hpp"
#include "QcGoldCoinMgr.hpp"
#include "QcSceneAnimation.hpp"
#include "Animator\QiAnimator.hpp"
#include "Res\QcResImageDesc.hpp"

QcGoldCoinMgr::QcGoldCoinMgr()
{

}

void QcGoldCoinMgr::Init(QcResManager* pResMgr)
{
	m_pGoldDesc = pResMgr->GetImageDesc(ID_GoldCoin);
	m_pSilverDesc = pResMgr->GetImageDesc(ID_SilverCoin);
	m_fCoinSpeed = SGlobalData.GetSysValue("CoinSpeed", 200.f);
}

void QcGoldCoinMgr::MakeCoin(UInt32 iScore, const QcPoint& startPt, const QcPoint& endPt, f32 fRotation)
{
	QcResImageDesc* pCoinDesc = NULL;
	UInt32 nCount = 0;
	float fGap = 0.f;
	if(iScore <= 6)
	{
		nCount = iScore;
		pCoinDesc = m_pSilverDesc;
	}
	else
	{
		nCount = QcMath::clamp<UInt32>(iScore / 5, 2, 8);
		pCoinDesc = m_pGoldDesc;
	}
	fGap = m_pGoldDesc->GetWidth();
	
	QcPoint offset(0.f, 0.f);
	if (QcMath::equals(fRotation, PI) || QcMath::equals(fRotation, TWO_PI) || QcMath::equals(fRotation, 0.f))
	{
		offset.X = endPt.X > startPt.X ? -fGap : fGap;
	}
	else
	{
		offset.Y = endPt.Y > startPt.Y ? -fGap : fGap;
	}
	QcPoint firstPt = startPt - offset * (nCount - 1.f) * 0.5f;

	f32 fStartTime = 0.0f;
	QcAnimateCreator& pAnimaorCreator = SGlobalData.goAnimateCreator;
	for (UInt32 i=0; i<nCount; i++)
	{
		QcSceneAnimation* pSceneNode = pCoinDesc->MakeInstance<QcSceneAnimation>();

		QiAnimator* pAnimator = pAnimaorCreator.MakeLineAnimator2(firstPt, endPt, m_fCoinSpeed);
		pAnimator->SetStartTime(fStartTime);
		pSceneNode->AddAnimator(pAnimator);

		pSceneNode->SetRotation(fRotation);
		pSceneNode->SetPosition(firstPt);

		firstPt += offset;
		fStartTime += 0.09f;

		AddAnimation(pSceneNode);
	}
}