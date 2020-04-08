#include "pch.hpp"
#include "QcSceneAnimationMgr.hpp"
#include "QcSceneAnimation.hpp"
#include "Res\QcResImageDesc.hpp"
#include "Animator\QiAnimatorDeque.hpp"
#include "Animator\QcAnimatorScale.hpp"
#include "Animator\QcAnimatorDither.hpp"
#include "Animator/QcAnimatorMoveParapola.hpp"
#include "Animator/QcAnimatorAlpha.hpp"
#include "EffectRender/QcEffectMgr.hpp"
#include "EffectRender/QcEffect.hpp"
#include "Scene\QcPlayer.hpp"

QcSceneAnimationMgr::QcSceneAnimationMgr(const QcPoint& vParticleOffsetPos)
: m_vParticleOffsetPos(vParticleOffsetPos)
{
}

QcSceneAnimationMgr::~QcSceneAnimationMgr()
{
}

void QcSceneAnimationMgr::Init(QcResManager* pResMgr)
{
#ifdef QmOldParticle
	m_pPyroFile = pResMgr->GetPyroFile();
#endif
#ifdef QmMPParticle
	m_pMpMgr = pResMgr->GetMpMgr();
#endif

	m_sBubbleTimeCount = 1.0f;
}

PyroParticles::IPyroParticleEmitter* QcSceneAnimationMgr::AddParticle(QeLayer eLayer, const char* psName, const QcPoint& pos, float MaxDelta, f32 fScale, f32 fAngle)
{
#ifdef QmOldParticle
 	if (QcMath::iszero(fScale))
 		fScale = 1.0f;

	static f32 gfXScale = QmGlobalData.gWindowSize.Width/QmGlobalData.gScreenRect.getWidth();
	static f32 gfYScale = QmGlobalData.gWindowSize.Height/QmGlobalData.gScreenRect.getHeight();

	PyroParticles::IPyroParticleEmitter* pEmitter = GetOldEmitter(eLayer).Append(m_pPyroFile, psName, (f32)DXUTGetTime(), gfXScale * pos.X/fScale, gfYScale * pos.Y/fScale, 1.0f, MaxDelta, fAngle);
	pEmitter->SetOffset(m_vParticleOffsetPos.X, m_vParticleOffsetPos.Y, 0.f);
	pEmitter->SetScale(fScale);

	return pEmitter;
#else
	return NULL;
#endif
}

MP_Emitter* QcSceneAnimationMgr::AddParticleMP(QeLayer eLayer, const char* psName, const QcPoint& pos, float MaxDelta /* = 1.f */, f32 fScale /* = 1.f */, f32 fAngle /* = 0.f */)
{
#ifdef QmMPParticle
	MP_Emitter* pEmitter = NULL;
	if (QcMath::iszero(fScale))
		fScale = 1.0f;

	{
		static f32 gfXScale = QmGlobalData.gWindowSize.Width/QmGlobalData.gScreenRect.getWidth();
		static f32 gfYScale = QmGlobalData.gWindowSize.Height/QmGlobalData.gScreenRect.getHeight();

		pEmitter = m_pMpMgr->GetFreeEmitterByName(psName);
        if (pEmitter)
        {
            pEmitter->SetPosition(MP_POSITION(pos.X * gfXScale/fScale, -pos.Y * gfYScale/fScale));

            pEmitter->Offset(MP_POSITION(m_vParticleOffsetPos.X, m_vParticleOffsetPos.Y));
            pEmitter->SetDirection(fAngle*RADTODEG);
            pEmitter->Restart();
            pEmitter->SetState(MAGIC_STATE_UPDATE);
            Magic_SetRandomMode(pEmitter->GetEmitter(), true);

            if (eLayer != eLayerNone)
                GetEmitter(eLayer).push_back(pEmitter);
        }
	}
	return pEmitter;
#else
	return NULL;
#endif
}
QcEffect* QcSceneAnimationMgr::AddEffect(QeLayer eLayer, const char* psName)
{
	QcEffect* pRet = QmResMgr->GetEffectMgr()->MakeEffect(psName);
	if (eLayer != eLayerNone)
		GetEffectList(eLayer).Insert(pRet);
	return pRet;
	
}
void QcSceneAnimationMgr::MakeCoin(u32 iScale, u32 iLevel, const QcPoint& startPt, const QcPoint& endPt, f32 fRotation, f32 fStartTime)
{
	QmPlaySound(MakeCoinSound);

	static QcResImageDesc* m_pGoldDesc = QmResMgr->GetImageDesc(QmObjType(GoldCoin));
	static QcResImageDesc* m_pSilverDesc = QmResMgr->GetImageDesc(QmObjType(SilveryCoin));

    QcResImageDesc *pCoinDesc = iScale*iLevel >= 50 ? m_pGoldDesc : m_pSilverDesc;
    u32 nCount = 0;
    f32 fMakeDir = fRotation;

    if (iScale == 2)
        nCount = 2;
    else if (iScale < 7)
        nCount = 3;
    else if (iScale < 15)
        nCount = 3;
    else if (iScale < 25)
        nCount = 5;
    else if (iScale < 30)
        nCount = 5;
    else if (iScale < 40)
        nCount = 7;
    else if (iScale == 40)
        nCount = 8;
    else
    {
        nCount = QcMath::clamp<u32>(iScale / 10, 2, 4*15);
    }

    if (nCount >= 15)
        nCount = (nCount / 15) * 15;
    
    QcResManager *&pResMgr = QmGlobalData.gpResMgr;

    QcVector vCoinGap(pCoinDesc->GetWidth(), 0);
    u32 nCoinPerRow = nCount%15;
    if (nCoinPerRow == 0) nCoinPerRow = 15;

    QcPoint ptStart(-vCoinGap);
    if (nCoinPerRow%2 == 0)
        ptStart *= nCoinPerRow/2.f - 0.5f;
    else
        ptStart *= nCoinPerRow/2.f;

    for (u32 i=0; i<nCount; ++i)
    {
        QcSceneAnimation *pCoinNode = pResMgr->MakeSceneNodeInstance<QcSceneAnimation>(pCoinDesc);
        QcPoint pos((ptStart + vCoinGap* f32(i%15)).rotateByRad(fMakeDir) + startPt);
        pCoinNode->SetPosition(pos);
        pCoinNode->SetRotation(fRotation);

        QcAnimatorMove *pAni = QmAnimatorCreator->MakeLineAnimator2(pos, endPt, 250.f);
		pAni->AddSpeedAnimator(new QcGravitySpeed(1000.f, 10.f));
        pCoinNode->AddAnimator(pAni);
        AddAnimation(pCoinNode);
        if (i%15 == 14 )
            ptStart.Y += pCoinDesc->GetHeight() * 0.95f;
    }


#if 0
	QcResImageDesc* pCoinDesc = NULL;
	UInt32 nCount = 0;
    f32 fScale = 1;
	if(iScore <= 6)
	{
		nCount = iScore;
		pCoinDesc = m_pSilverDesc;
        fScale *= 0.75f;
	}
	else
	{
		nCount = QcMath::clamp<UInt32>(iScore / 5, 2, 8);
		pCoinDesc = m_pGoldDesc;
		if (iScore == 50)
		{
			nCount = 10;
		}
		else if (iScore == 80)
		{
			nCount = 12;
		}
		else if (iScore > 80)
		{
			nCount = 16;
		}
	}

	QcAnimateCreator& AnimaorCreator = QmGlobalData.goAnimateCreator;
    QcResManager *&pResMgr = QmGlobalData.gpResMgr;

    f32 fJumpDistance = Randomf(100, 150);
    QcPoint JumpPoint0(0, fJumpDistance), JumpPoint1;
    JumpPoint0.rotateByRad(fRotation);
    JumpPoint1 = JumpPoint0 * 0.2f;

    QcSceneAnimation *pCoinNode = NULL;
    QcAnimatorMove *pCoinMove = NULL;
    QcGravitySpeed *pCoinSpdChanger = NULL;
    QiAnimatorDeque *pCoinAniDeq = NULL;

    f32 fStartSpd0 = 2*fJumpDistance / 0.25f;
    f32 fStartSpd1 = 2*fJumpDistance*0.2f /0.15f;
    QcPoint tmpEndPt = QmSysSetting->GetRelativeCoinEndPt();
    tmpEndPt = tmpEndPt.rotateByRad(PI+fRotation)*1.5f + endPt;
    f32 fCollectionSpd = 2*startPt.getDistanceFrom(tmpEndPt) / 1.5f + 250.f;

    for (UInt32 i=0; i<nCount; i++)
    {
        pCoinNode = pResMgr->MakeSceneNodeInstance<QcSceneAnimation>(pCoinDesc);
        pCoinNode->SetRotation(fRotation);
        pCoinNode->SetScale(fScale);
        pCoinNode->SetPosition(QcPoint(Randomf(0,30),0).rotateByRad(Randomf(0, TWO_PI)) + startPt);

        pCoinAniDeq = AnimaorCreator.MakeAnimatorDeque();

        pCoinMove = AnimaorCreator.MakeLineAnimator2(QcPoint(), JumpPoint0, fStartSpd0)->As<QcAnimatorMove*>();
        pCoinSpdChanger = new QcGravitySpeed(-fStartSpd0 / 0.25f, 0.25f);
        pCoinMove->AddSpeedAnimator(pCoinSpdChanger);
        pCoinAniDeq->AddAnimator(pCoinMove);

        pCoinMove = AnimaorCreator.MakeLineAnimator2(JumpPoint0, QcPoint(), 0)->As<QcAnimatorMove*>();
        pCoinSpdChanger = new QcGravitySpeed(fStartSpd0 / 0.25f, 0.25f);
        pCoinMove->AddSpeedAnimator(pCoinSpdChanger);
        pCoinAniDeq->AddAnimator(pCoinMove);

        pCoinMove = AnimaorCreator.MakeLineAnimator2(QcPoint(), JumpPoint1, fStartSpd1)->As<QcAnimatorMove*>();
        pCoinSpdChanger = new QcGravitySpeed(-fStartSpd1 / 0.15f, 0.15f);
        pCoinMove->AddSpeedAnimator(pCoinSpdChanger);
        pCoinAniDeq->AddAnimator(pCoinMove);

        pCoinMove = AnimaorCreator.MakeLineAnimator2(JumpPoint1, QcPoint(), 0)->As<QcAnimatorMove*>();
        pCoinSpdChanger = new QcGravitySpeed(fStartSpd1 / 0.15f, 0.15f);
        pCoinMove->AddSpeedAnimator(pCoinSpdChanger);
        pCoinAniDeq->AddAnimator(pCoinMove);

        pCoinMove = AnimaorCreator.MakeLineAnimator2(startPt, tmpEndPt, fCollectionSpd)->As<QcAnimatorMove*>();
        pCoinSpdChanger = new QcGravitySpeed(-fCollectionSpd / 1.5f, 1.5f);
        pCoinMove->AddSpeedAnimator(pCoinSpdChanger);
        pCoinMove->SetStartTime(0.5f);
        pCoinAniDeq->AddAnimator(pCoinMove);

        pCoinMove = AnimaorCreator.MakeLineAnimator2(tmpEndPt, endPt, 250.f)->As<QcAnimatorMove*>();
        pCoinAniDeq->AddAnimator(pCoinMove);

        pCoinNode->AddAnimator(pCoinAniDeq);
        pCoinNode->SetStartTime(Randomf(0,0.5f));
        AddAnimation(pCoinNode, eAnimationLayerScore);
    }
#endif
#if 0
	f32 fVCs = cos(fRotation + HALF_PI);
	f32 fVSn = sin(fRotation + HALF_PI);

	f32 fHCs = cos(fRotation);
	f32 fHSn = sin(fRotation);

	QcPoint vStartPos(startPt);
	QcPoint Up_pos;
	QcPoint Up_pos2;
	QcPoint end_pos;
	QcPoint tempPos;
	QcVector vec;
	for (UInt32 i = 0; i < nCount; ++i)
	{
		QcSceneAnimation* pSceneNode = QmResMgr->MakeSceneNodeInstance<QcSceneAnimation>(pCoinDesc);
		float rand_num = Randomf(0.8f,1.0f);
		QiAnimatorDeque* pCoinAnimatorList = pAnimaorCreator.MakeAnimatorDeque();

		//////////////////////////////////////////////////////////////////////////
		vStartPos.X += 20 * fHCs * rand_num;
		vStartPos.Y += 20 * fHSn * rand_num;

		Up_pos.X = vStartPos.X + 150.f * fVCs * rand_num;
		Up_pos.Y = vStartPos.Y + 150.f * fVSn * rand_num;

		Up_pos2.X = vStartPos.X + 90.f * fVCs * rand_num;
		Up_pos2.Y = vStartPos.Y + 90.f * fVSn * rand_num;

		QcAnimatorMove* pAnimatorMoveUp = static_cast<QcAnimatorMove*>(pAnimaorCreator.MakeLineAnimator2(vStartPos, Up_pos, 500.f));
		QcAnimatorMove* pAnimatorMoveDown = static_cast<QcAnimatorMove*>(pAnimaorCreator.MakeLineAnimator2(Up_pos, vStartPos, 500.f));

		QcAnimatorMove* pAnimatorMoveUp2 = static_cast<QcAnimatorMove*>(pAnimaorCreator.MakeLineAnimator2(vStartPos, Up_pos2, 250.f));
		QcAnimatorMove* pAnimatorMoveDown2 = static_cast<QcAnimatorMove*>(pAnimaorCreator.MakeLineAnimator2(Up_pos2, vStartPos, 250.f));

		pCoinAnimatorList->AddAnimator(pAnimatorMoveUp);
		pCoinAnimatorList->AddAnimator(pAnimatorMoveDown);
		pCoinAnimatorList->AddAnimator(pAnimatorMoveUp2);
		pCoinAnimatorList->AddAnimator(pAnimatorMoveDown2);
		//////////////////////////////////////////////////////////////////////////

		end_pos.X = endPt.X + 150.f * fVCs;
		end_pos.Y = endPt.Y + 150.f * fVSn;
		
		vec = end_pos - vStartPos;
		tempPos = vStartPos;

		f32 fLen = vec.getLengthSQ();
		if (fLen > 200.f * 200.f)
		{
			fLen = sqrtf(fLen);
			vec.normalize(fLen);
			fLen -= 130.f;
			tempPos += vec * fLen;

			const float fBeginSpeed = 1692.f;
			const f32 fEndSpeed = 250.f;
			QcAnimatorMove* pAnimatorMoveToPlayer = static_cast<QcAnimatorMove*>(pAnimaorCreator.MakeLineAnimator2(vStartPos, tempPos, fBeginSpeed));
			pCoinAnimatorList->AddAnimator(pAnimatorMoveToPlayer);
			{
				f32 a = ((fEndSpeed * fEndSpeed)-(fBeginSpeed * fBeginSpeed))/(2 * fLen);
				f32 fSpeedDownTime = abs(fBeginSpeed - 250/a);
				if (fSpeedDownTime > 1.f/40.f)
				{
					pAnimatorMoveToPlayer->AddSpeedAnimator(new QcGravitySpeed(a, fSpeedDownTime));
				}
			}
		}
		QcAnimatorMove* pAnimatorMoveToPlayer= static_cast<QcAnimatorMove*>(pAnimaorCreator.MakeLineAnimator2(tempPos, end_pos, 300.f));
		pCoinAnimatorList->AddAnimator(pAnimatorMoveToPlayer);

		//QcAnimatorMove* pAnimatorMoveOutScreen = static_cast<QcAnimatorMove*>(pAnimaorCreator.MakeLineAnimator2(end_pos, endPt, 1000.f));
		//pCoinAnimatorList->AddAnimator(pAnimatorMoveOutScreen);
			
		pSceneNode->AddAnimator(pCoinAnimatorList);
		pSceneNode->SetFinishFlag(pCoinAnimatorList);
		pSceneNode->SetPosition(vStartPos);
		pSceneNode->SetRotation(fRotation);
		pSceneNode->SetStartTime(i * 0.1f);

		AddAnimation(pSceneNode, eAnimationLayerScore);
	}
#endif
}
void QcSceneAnimationMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
#ifdef QmMPParticle
	static u64 giCurrentFrame = 0;
	if (m_pMpMgr && giCurrentFrame != giFrameID)
	{
		giCurrentFrame = giFrameID;
		m_pMpMgr->Update(fElapsedTime*1000);
	}

	m_sBubbleTimeCount -= fElapsedTime;
	if (m_sBubbleTimeCount < 0.f)
	{
		m_sBubbleTimeCount = Randomf(1.f, 3.f);
		int count = RandomInt(1, 3);
		for (int i = 0; i< count; ++i)
		{
			QcPoint pos;
			pos.X = Randomf(QmGlobalData.gScreenRect.left(), QmGlobalData.gScreenRect.right());
			pos.Y = Randomf(QmGlobalData.gScreenRect.bottom(), QmGlobalData.gScreenRect.top());
			AddParticleMP(eAnimationLayerBubble, "bubble", pos, 12.f, 1.f, Randomf(0.f, 2*D3DX_PI));
		}
	}
#endif
	for (u32 iLayer = eAnimationLayerBegin; iLayer < eAnimationLayerEnd; ++iLayer)
	{
		{
			QcSceneAnimationPtrList& animationList = GetAnimationList(iLayer);
			animationList.Animate(dCurrentTime, fElapsedTime);
		}
#ifdef QmOldParticle
		{
			CEmitterList& emitter = GetOldEmitter(iLayer);
			emitter.Move((f32)dCurrentTime, fElapsedTime);
			emitter.Prepare((f32)dCurrentTime, fElapsedTime);
		}
#endif
		{
			QcEffectList& effectList = GetEffectList(iLayer);
			QcEffectList::iterator iter(effectList.begin());
			QcEffectList::iterator endIter(effectList.end());
			for (; iter!=endIter;++iter)
			{
				if (*iter)
				{
					(*iter)->OnAnimate(NULL,dCurrentTime,fElapsedTime);
				}
			}
		}
	}
}

void QcSceneAnimationMgr::Render(QcDeviceRender& render, QeLayer eLayer, const QcMatrix* pWorldMatrix)
{
	{
		QcSceneAnimationPtrList& animationList = GetAnimationList(eLayer);
		QcSceneAnimationPtrListIter iter(animationList.begin());
		QcSceneAnimationPtrListIter endIter(animationList.end());
		for (; iter != endIter; ++iter)
		{
			if (*iter)
			{
				(*iter)->Render(render, pWorldMatrix);
				if ((*iter)->IsFinished())
				{
					animationList.EraseAt(iter - animationList.begin());
				}
			}
		}
	}
#ifdef QmOldParticle
	{
		CEmitterList& emitter = GetOldEmitter(eLayer);
		emitter.Render(0);
	}
#endif

#ifdef QmMPParticle
	{
		MP_EmitterList& emitter = GetEmitter(eLayer);
		MP_EmitterList::iterator iter(emitter.begin());
		while(iter != emitter.end())
		{
			if ((*iter)->GetState() == MAGIC_STATE_STOP)
			{
				iter = emitter.erase(iter);
				continue;
			}
			(*iter)->Render();
			++iter;
		}
	}
#endif

	{
		QcEffectList& effectList = GetEffectList(eLayer);
		QcEffectList::iterator iter(effectList.begin());
		QcEffectList::iterator endIter(effectList.end());
		for (; iter!=endIter;++iter)
		{
			if (*iter)
			{
				(*iter)->OnRender(render,NULL);
				if ((*iter)->IsFinish())
				{
					effectList.EraseAt(iter - effectList.begin());
				}
			}

		}
	}
	

}