#include "Pch.hpp"
#include "DcJumpScoreFish.hpp"
#include "Scene\QcPlayer.hpp"
#include "Scene\QcBullet.hpp"
#include "Scene\QcSceneAnimationMgr.hpp"
#include "Res\QcResFishDesc.hpp"
#include "QcFishMgr.hpp"
#include "Scene\QcDelayExplodeAnimation.hpp"
#include "Scene\QcJumpScoreFishAnimation.hpp"

ImplRuntimeClass2(DcJumpScoreFish, 10);

#define FISH_MOVE_TIME              2.0f
#define SCORE_JUMP_STEP_TIMER       10001 
#define SCORE_JUMP_PAUSE_TIMER      10002 
#define SCORE_JUMP_SOUND_TIMER      10003
#define SCORE_JUMP_END_TIMER        10004

#define JUMP_STEP                   (20.f)
#ifdef QmTestVersion
#define FIRST_JUMP                  (40.f)
#else
#define FIRST_JUMP                  (100.f)
#endif

#define JUMP_PAUSE_TIME             0.33f

DcJumpScoreFish::DcJumpScoreFish()
{
    m_fJumpingScore = 0;
    m_bJumping = false;
	m_pMyParticle = NULL;
	m_pJumpFishScoreBgAnimation = NULL;
}

DcJumpScoreFish::~DcJumpScoreFish()
{
	m_pMyParticle = NULL;
}

void DcJumpScoreFish::Animate( double dCurrentTime,f32 fElapsedTime )
{
    QcFish::Animate(dCurrentTime, fElapsedTime);
    if (m_pJumpingScore)
    {
        if (m_bJumping)
            m_fJumpingScore += JUMP_STEP * fElapsedTime;

        m_pJumpingScore->SetDigitalValue(u64(m_fJumpingScore*m_pBullet->GetLevel()*m_pBullet->GetMultiple()));
        m_pJumpingScore->Animate(dCurrentTime, fElapsedTime);
    }
#ifdef QmOldParticle
    CEmitterList  &templist = QmSceneMgr->GetAnimationMgr()->GetOldEmitter(eAnimationLayerSpecial);
	if (m_pMyParticle && templist[m_pMyParticle])
	{
		QcPoint pos = GetPosition();
		static f32 gfXScale = QmGlobalData.gWindowSize.Width/QmGlobalData.gScreenRect.getWidth();
		static f32 gfYScale = QmGlobalData.gWindowSize.Height/QmGlobalData.gScreenRect.getHeight();
		templist[m_pMyParticle]->x = pos.X * gfXScale;
		templist[m_pMyParticle]->y = -pos.Y * gfYScale;
	}
#endif
}

void DcJumpScoreFish::OnTimer( u32 nTimer, void* pContext )
{
    if (nTimer == SCORE_JUMP_STEP_TIMER)
    {
        m_bJumping = false;
        m_fJumpingScore = QcMath::round_((m_fJumpingScore/10))* 10;

        if (abs(m_fJumpingScore - GetScore()) < 10)
            m_fJumpingScore = f32(GetScore());
        else if (m_fJumpingScore < FIRST_JUMP)
            m_fJumpingScore = FIRST_JUMP;

        MakeScoreZoomAnimate();
        KillTimer(SCORE_JUMP_SOUND_TIMER);
        QmPlaySound(ScoreJumpPause);
        if (m_fJumpingScore < GetScore())
            SetTimer(SCORE_JUMP_PAUSE_TIMER, JUMP_PAUSE_TIME, true, pContext);
        else
        {
            SetTimer(eDyingTimer, m_pDesc->GetDyingTime(), true, pContext);
            SetTimer(SCORE_JUMP_END_TIMER, 0.5f, true);
        }
    }
    else if (nTimer == SCORE_JUMP_END_TIMER)
    {
        QcDigitalNode &node = m_pJumpingScore->GetDigitalNode();
        f32 fZoom = node.GetScaleX();
        DWORD dwCoef = (DWORD)pContext;
        node.SetScale(fZoom+(dwCoef%2 ? 0.06f : -0.03f));
        if (dwCoef < 5)
            SetTimer(SCORE_JUMP_END_TIMER, 0.1f, true, (void*)(++dwCoef));
    }
    else if (nTimer == SCORE_JUMP_PAUSE_TIMER)
    {
        m_bJumping = true;
        SetTimer(SCORE_JUMP_SOUND_TIMER, 0.5f);
        QmPlaySound(ScoreJumping);
        int iScore = GetScore();
        if (iScore-int(m_fJumpingScore) >= int(JUMP_STEP*2.5f))
            SetTimer(SCORE_JUMP_STEP_TIMER, 2.0f, true, pContext);

        //else if (iScore-int(m_fJumpingScore) >= int(JUMP_STEP*1.5f))
        //    SetTimer(SCORE_JUMP_STEP_TIMER, 1.0f, true, pContext);

        else
        {
            f32 fTimer = (iScore-m_fJumpingScore) / JUMP_STEP;
            SetTimer(SCORE_JUMP_STEP_TIMER, fTimer, true, pContext);
        }

    }
    else if (nTimer == SCORE_JUMP_SOUND_TIMER)
        QmPlaySound(ScoreJumping);
    else if (nTimer == eDyingTimer)
    {
        ToDead();
        QmPlaySound(ScoreJumpEnd);
        if (u32(pContext) == 1)
        {
            u32 iScore = GetScore()*m_pBullet->GetLevel()*m_pBullet->GetMultiple();
            m_pOwner->HitAddScore(iScore, iScore);
        }
        QmSceneMgr->LockNormalScene(false);
    }
	else if(nTimer == eTimerParticleGap)
	{
		m_pMyParticle = 	QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerSpecial, "打死最大鱼", GetPosition());
	}
    else
        QcFish::OnTimer(nTimer, pContext);
}

void DcJumpScoreFish::HitToDie( QcBullet* pBullet )
{
    ToDie(false);
    QmSceneMgr->LockNormalScene(true);	
    //const QcString& bombEffect = GetFishDesc()->GetBombEffect();
    //if (bombEffect.size())
    //    QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, bombEffect.c_str(), GetPosition());
	
	{
		//QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerDyingFish, "打死大型鱼01", GetPosition());
		SetTimer(eTimerParticleGap, 0.1f, true);
        if (!m_pJumpFishScoreBgAnimation)
            m_pJumpFishScoreBgAnimation = QfMakeEmptyInstance<QcJumpScoreFishAnimation>();

        m_pJumpFishScoreBgAnimation->Start();
        QmSceneMgr->GetAnimationMgr()->AddAnimation(m_pJumpFishScoreBgAnimation, eAnimationLayerSpecial, true);
		//pAnimation->SetLivingTime(FISH_MOVE_TIME + 2.0f);
		//QmAnimationMgr->AddAnimation(pAnimation, eAnimationLayerDyingFish);
	}

    m_pOwner = pBullet->GetOwner();
    m_pBullet = pBullet;

    m_fJumpingScore = 0;
    m_pJumpingScore = QmGlobalData.gpResMgr->MakeSceneNodeInstance<QcSceneDigital>(QmObjName(JumpingScore));
    m_pJumpingScore->SetPosition(m_vPos);
    m_pJumpingScore->SetRotation(m_pOwner->GetRotation());
    m_pJumpingScore->SetDigitalValue(u64(m_fJumpingScore));
    switch (QmSysSetting->GetPlayerNumber())
    {
    default:
        m_pJumpingScore->GetDigitalNode().SetScale(0.34f);
        break;
    case eMachine_10_1:
    case eMachine_4_2:
        m_pJumpingScore->GetDigitalNode().SetScale(0.4f);
        break;
    }

    QcPoint vMoveEndPos,tmpPt(50,200);
    if (equals(m_pOwner->GetRotation(), PI) || iszero(m_pOwner->GetRotation()))
        tmpPt.Y = 300;

    m_pOwner->GetMatrix().transformVect(vMoveEndPos, tmpPt);
    f32 fMoveDistance = vMoveEndPos.getDistanceFrom(m_vPos);
    f32 fInitSpd = 2*fMoveDistance / FISH_MOVE_TIME;
    f32 fAccelSpd = -fInitSpd / FISH_MOVE_TIME;
    QcAnimatorMove *pMoveAni = QmAnimatorCreator->MakeLineAnimator2(m_vPos, vMoveEndPos, fInitSpd)->As<QcAnimatorMove*>();
    QcGravitySpeed *pSpdAni = new QcGravitySpeed(fAccelSpd, FISH_MOVE_TIME);
    pMoveAni->AddSpeedAnimator(pSpdAni);
	pMoveAni->AddFinishCallBack(QcFinishAnimatorCb(boost::bind(&DcJumpScoreFish::FinishAnimatorCallBack, this, _1)), 0);
    AddAnimator(pMoveAni);
    m_pJumpingScore->AddAnimator(pMoveAni);
    m_pJumpingScore->SetColor(m_pOwner->GetColorFlag());

    m_bJumping = true;
    f32 fJumpTime = FIRST_JUMP / JUMP_STEP;
    SetTimer(SCORE_JUMP_STEP_TIMER, fJumpTime, true, (void*)0x1);
    SetTimer(SCORE_JUMP_SOUND_TIMER, 0.5f);

    QmPlaySound(ScoreStartJump);

    QmSceneMgr->GetFishMgr()->HoistFishToTop(this);
    QmOutputString("Jump Fish Score:%u\n", GetExplodeScore());
}

void DcJumpScoreFish::ExploseToDie( QcFish* pFish )
{
    ToDie(false);
    QmSceneMgr->LockNormalScene(true);

    m_pBullet = pFish->GetBullet();
    m_pOwner = m_pBullet->GetOwner();

    m_fJumpingScore = 0;
    m_pJumpingScore = QmGlobalData.gpResMgr->MakeSceneNodeInstance<QcSceneDigital>(QmObjName(JumpingScore));
    m_pJumpingScore->SetPosition(m_vPos);
    m_pJumpingScore->SetRotation(m_pOwner->GetRotation());
    m_pJumpingScore->SetDigitalValue(u64(m_fJumpingScore));
    switch (QmSysSetting->GetPlayerNumber())
    {
    default:
        m_pJumpingScore->GetDigitalNode().SetScale(0.34f);
        break;
    case eMachine_10_1:
    case eMachine_4_2:
        m_pJumpingScore->GetDigitalNode().SetScale(0.5f);
        break;
    }

    QcPoint vMoveEndPos;
    m_pOwner->GetMatrix().transformVect(vMoveEndPos, QcPoint(0, 300));
    f32 fMoveDistance = vMoveEndPos.getDistanceFrom(m_vPos);
    f32 fInitSpd = 2*fMoveDistance / FISH_MOVE_TIME;
    f32 fAccelSpd = -fInitSpd / FISH_MOVE_TIME;
    QcAnimatorMove *pMoveAni = QmAnimatorCreator->MakeLineAnimator2(m_vPos, vMoveEndPos, fInitSpd)->As<QcAnimatorMove*>();
    QcGravitySpeed *pSpdAni = new QcGravitySpeed(fAccelSpd, FISH_MOVE_TIME);
    pMoveAni->AddSpeedAnimator(pSpdAni);

    AddAnimator(pMoveAni);
    m_pJumpingScore->AddAnimator(pMoveAni);
    m_pJumpingScore->SetColor(m_pOwner->GetColorFlag());

    m_bJumping = true;
    f32 fJumpTime = FIRST_JUMP / JUMP_STEP;
    SetTimer(SCORE_JUMP_STEP_TIMER, fJumpTime, true, (void*)0x1);
    SetTimer(SCORE_JUMP_SOUND_TIMER, 0.5f);

    QmPlaySound(ScoreStartJump);

    QmSceneMgr->GetFishMgr()->HoistFishToTop(this);
}

void DcJumpScoreFish::Render( QcDeviceRender& render, const QcMatrix* pWorldMatrix )
{
    QcFish::Render(render, pWorldMatrix);
    if (m_pJumpingScore)
        m_pJumpingScore->Render(render, pWorldMatrix);
}

void DcJumpScoreFish::ToDead()
{
    QcFish::ToDead();
    m_pJumpingScore = NULL;
    if (m_pJumpFishScoreBgAnimation)
    {
        m_pJumpFishScoreBgAnimation->Finish();
    }
}

void DcJumpScoreFish::MakeScoreZoomAnimate()
{
    QcDigitalNode &node = m_pJumpingScore->GetDigitalNode();
    f32 fZoom = node.GetScaleX();
    f32 fZoomCoef = 0.04f;
    f32 fMax = 0.8f;
    switch (QmSysSetting->GetPlayerNumber())
    {
    default:
        break;
    case eMachine_10_1:
    case eMachine_4_2:
        fZoomCoef = 0;
        fMax = 0.75f;
        break;
    }
    QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(fMax, fZoom+fZoomCoef, JUMP_PAUSE_TIME);
    node.SetScale(fMax);
    node.AddAnimator(pAni);

    QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerNormal, "最大鱼跳倍时", m_vPos);
}

void DcJumpScoreFish::FinishAnimatorCallBack( u32 pPara )
{
	m_pMyParticle = NULL;
}
