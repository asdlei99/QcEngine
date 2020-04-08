#include "pch.hpp"
#include "QcSceneTrackFishHelper.hpp"
#include "QcPlayer.hpp"
#include "Fish\QcFish.hpp"
#include "Fish\QcFishMgr.hpp"
#include "Res\QcResPlayerDesc.hpp"
#include "Res\QcResImageListDesc.hpp"
#include "Animator\QcAnimatorMoveLine.hpp"
#include "Animator\QcAnimatorMoveCircle.hpp"
#include "Animator\QcAnimatorMoveTrack.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "QcPlayerMgr.hpp"
#include "Scene/Fish/DcChainFish.hpp"

ImplRuntimeClass(QcSceneTrackFishHelper);

QcSceneTrackFishHelper::QcSceneTrackFishHelper()
{

}

void QcSceneTrackFishHelper::Init(QcPlayer* pPlayer, const QcPoint& vFishFlagPos, const QcPoint& vGunPos)
{
	m_pPlayer = pPlayer;
	m_vAbsFishFlagPos = vFishFlagPos;
	m_pFishFlag = NULL;
	m_pTargetFish = NULL;

	const QcResPlayerDesc* pPlayerDesc = static_cast<const QcResPlayerDesc*>(pPlayer->GetDesc());
	m_pPlayerIDFlag = QfMakeEmptyInstance<QcSceneAnimation>();
	
	QcSceneNode* TempTrackLock = QfMakeSceneNodeInstance<QcSceneNode>(pPlayerDesc->GetFlag());
	TempTrackLock->AddAnimator(QmAnimatorCreator->MakeRotationAnimator(PI));
	m_pPlayerIDFlag->AddSubSceneNode(TempTrackLock);
	m_pPlayerIDFlag->SetRotation(pPlayer->GetRotation());
	QmSetDigitalType(m_pPlayerIDFlag->GetDigitalNode(), TrackLockDigital);
	m_pPlayerIDFlag->SetDigitalValue(m_pPlayer->GetID());
	m_pPlayerIDFlag->SetDigitalVisible(true);

	m_gunPosNode = QfMakeEmptyInstance<QcSceneNode>();
	m_gunPosNode->SetPosition(vGunPos);

	m_pMoveTrack = static_cast<QcAnimatorMoveTrack*>(QmAnimatorCreator->MakeTrackAnimator(NULL, 0.f));

	m_pPlayerIdMovingFlag = QfMakeSceneNodeInstance<QcSceneAnimation>(pPlayerDesc->GetMovingFlag());
	QmSetDigitalType(m_pPlayerIdMovingFlag->GetDigitalNode(), TrackLockDigital);
	m_pPlayerIdMovingFlag->SetDigitalValue(m_pPlayer->GetID());
	m_pPlayerIdMovingFlag->SetDigitalVisible(true);
	m_pPlayerIdMovingFlag->AddAnimator(m_pMoveTrack);
	m_pPlayerIdMovingFlag->SetPosition(pPlayer->GetPosition());
	m_pPlayerIdMovingFlag->SetVisible(false);
	m_pPlayerIdMovingFlag->SetRotation(pPlayer->GetRotation());

	m_pMoveCircle = static_cast<QcAnimatorMoveCircle*>(QmAnimatorCreator->MakeCircleAnimator(m_vAbsFishFlagPos, TWO_PI, true, false, 0.f));
}

QcSceneNode* QcSceneTrackFishHelper::MakeFishFlagNode(QcFish* pFish)
{
	QcSceneNode* pRet = NULL;
	if (pFish)
	{
		static QcResImageDesc* pFishFlagBgDesc = QmResMgr->GetImageDesc(QmObjType(suodingkuang));
		pRet = QfMakeSceneNodeInstance<QcSceneNode>(pFishFlagBgDesc);

		if (pFish->IsChainLinkFish())
		{
			QcSceneNode* ChainLinkMainFish = ((DcChainLinkFish*)pFish)->GetMainFish();
			QcSceneNode* pFindFish = QfMakeSceneNodeInstance<QcSceneNode>(ChainLinkMainFish->GetDesc());
			QcAnimatorTexture* pTexAnimator = pFindFish->FindAnimator<QcAnimatorTexture>();
			if (pTexAnimator)
				pTexAnimator->SetLoop(true);
			f32 scale = QcMath::min_(0.85f * pRet->GetWidth() / pFindFish->GetWidth(), 0.85f * pRet->GetHeight() / pFindFish->GetHeight(), 1.0f);
			pFindFish->SetScale(scale);
			static QcResImageDesc* shellDesc = QmResMgr->GetImageDesc(QmObjName(ChainShell));
			const QsStateParameter* pStateParam = shellDesc->GetStateParameter(eLiving);
			QcAnimatorTexture* pShellTexture = static_cast<QcAnimatorTexture*>(QmAnimatorCreator->MakeTextureAnimator(pStateParam));
			pShellTexture->SetLoop(true);
			QcSceneNode* shellnode = QfMakeEmptyInstance<QcSceneNode>();
			shellnode->AddAnimator(pShellTexture);
			shellnode->SetScale(0.3f);
			pRet->AddSubSceneNode(shellnode);
			pRet->AddSubSceneNode(pFindFish);
		}
		else
		{
			QcSceneNode* pFindFish = QfMakeSceneNodeInstance<QcSceneNode>(pFish->GetDesc());
			QcAnimatorTexture* pTexAnimator = pFindFish->FindAnimator<QcAnimatorTexture>();
			if (pTexAnimator)
				pTexAnimator->SetLoop(true);
			f32 scale = QcMath::min_(0.85f * pRet->GetWidth() / pFindFish->GetWidth(), 0.85f * pRet->GetHeight() / pFindFish->GetHeight(), 1.0f);
			if(pFish->GetObjType() == QmObjName(GambleFish))
				pFindFish->SetScale(scale *1.4f);
			else
				pFindFish->SetScale(scale);
			pRet->AddSubSceneNode(pFindFish);
		}
	}
	return pRet;
}

void QcSceneTrackFishHelper::Animate(double dCurrentTime,f32 fElapsedTime)
{
	if (m_pPlayerIdMovingFlag->IsVisible())
	{
		m_pPlayerIdMovingFlag->Animate(dCurrentTime, fElapsedTime);
	}
	if (m_pTargetFish)
	{
		const QcPoint& tarPos = m_pPlayerIdMovingFlag->IsFinished() ? m_pTargetFish->GetPosition() : m_pPlayerIdMovingFlag->GetPosition();
		if (QmSysSetting->GetTrackedBox().isPointInside(m_pTargetFish->GetPosition()) && !m_pTargetFish->IsSheltered())
		{
			m_pPlayerIDFlag->SetPosition(tarPos);
			if (m_pPlayerIdMovingFlag->IsFinished())
			{
				QcVector vec(GetPlayer()->GetAbsGunPos(), QcPoint(0.f, 0.f));
				vec.normalize();
				m_pPlayerIDFlag->SetPosition(tarPos + 25.f * vec);
				m_pPlayerIDFlag->Animate(dCurrentTime,fElapsedTime);
				m_pPlayerIdMovingFlag->SetPosition(tarPos);
			}
			GetPlayer()->GetGunMgr().AdjustGunOriental(QcVector(GetPlayer()->GetAbsGunPos(), m_pPlayerIDFlag->GetPosition()), 0.17f);
			m_pFishFlag->Animate(dCurrentTime, fElapsedTime);
		}
		else
		{
			//当鱼不在锁定区域内，清楚鱼的所有监控器。
			m_pTargetFish->RemoveMonitor(NULL);
			//将锁定标记收回
			LockFish(NULL);
		}
	}
}

void QcSceneTrackFishHelper::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	//追踪标记
	if (m_pPlayerIdMovingFlag->IsVisible())
	{
		if (m_pPlayerIdMovingFlag->IsFinished())
			m_pPlayerIdMovingFlag->SetVisible(false);
		else
			m_pPlayerIdMovingFlag->Render(render);
	}
	//锁定标记
	if (m_pTargetFish)
	{
		if (m_pPlayerIdMovingFlag->IsFinished())
		{
			m_pPlayerIDFlag->Render(render);
		}
		m_pFishFlag->Render(render);
	}
}

void QcSceneTrackFishHelper::LockFish()
{
	QcSceneMgr::QeSceneState eSceneState = QmSceneMgr->GetSceneState();
	if (eSceneState == QcSceneMgr::eSceneNormal || eSceneState == QcSceneMgr::eSceneRegular)
	{
		if (m_pTargetFish == NULL)
		{
			LockFish(QmSceneMgr->GetFishMgr()->FindTargetFish());
		}
	}
	else if (m_pTargetFish)
	{
		LockFish(NULL);
	}
}

void QcSceneTrackFishHelper::LockFish(QcFish* pFish)
{
	if (pFish == NULL && m_pTargetFish == NULL)
		return;
	if (m_pTargetFish)
		m_pTargetFish->RemoveLocker(GetPlayer());
	m_pTargetFish = pFish;
	if (m_pTargetFish)
		m_pTargetFish->AddLocker(GetPlayer());

	//控制追踪标记的运动轨迹。
	QcSceneNode* pTargetNode = m_pTargetFish ? m_pTargetFish : m_gunPosNode.Get();
	f32 fTempSpeed = 3240.f;
	m_pMoveTrack->Reset(pTargetNode, fTempSpeed + pTargetNode->GetDesc()->GetSpeed());
	f32 lenth = m_pPlayerIdMovingFlag->GetPosition().getDistanceFrom(pTargetNode->GetPosition());
	//控制追踪标记的速度变化
	if (lenth > 130)
	{
		lenth *= 0.8f;
		float a = ((700.f*700.f)-(fTempSpeed * fTempSpeed)) / (2 * lenth);
		if (a < -5000.f)
			a = -5000.f;
		f32 GspeedTime = abs((fTempSpeed -700.f) / a);
		QmAssert(GspeedTime >= 0.f);
		QcAnimatorSpeedPtr pSpeed(new QcGravitySpeed(a, GspeedTime));
		m_pMoveTrack->AddSpeedAnimator(pSpeed);
	}
	m_pPlayerIdMovingFlag->Reset();
	m_pPlayerIdMovingFlag->AddAnimator2(m_pMoveTrack);
	m_pPlayerIdMovingFlag->SetFinishFlag(m_pMoveTrack);
	m_pPlayerIdMovingFlag->SetVisible(true);

	if (m_pTargetFish)
	{
		//显示追踪鱼的动画
		m_pMoveCircle->Reset(m_vAbsFishFlagPos, TWO_PI, true, false, 0.f);
		m_pFishFlag = MakeFishFlagNode(m_pTargetFish);
		m_pFishFlag->AddAnimator2(m_pMoveCircle);
		m_pFishFlag->SetPosition(m_vAbsFishFlagPos);
		m_pFishFlag->SetRotation(GetPlayer()->GetRotation());
	}
}