#include "Pch.hpp"
#include "DcGambleFish.hpp"
#include "..\QcBullet.hpp"
#include "..\QcPlayer.hpp"
#include "QcFishMgr.hpp"
#include "..\QcSceneAnimationMgr.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "Scene\QcBackGroundMgr.hpp"
#include "Scene\QcSceneMgr.hpp"
#include "Scene\QcBackGround.hpp"

ImplRuntimeClass(DcGambleFish);

#define FISH_MOVE_TIME              2.0f
#define SET_ANI_CB(x) AddFinishCallBack(boost::bind(&DcGambleFish::AnimationFinishCallBack,this,_1),(x));
#define GAMBLE_AUTO_TIMER  10005

#ifdef QmTestVersion
#define GambleDefault 1
#else
#define GambleDefault 0
#endif

const static f32 FishScaleS = 0.6f;
const static f32 FishScaleB = 0.8f;
const static f32 FishScoreShowScaleS = 0.5f;
const static f32 FishScoreShowScaleB = 0.7f;

DcGambleFish::DcGambleFish() : m_iSelectId(GambleDefault), m_bAcceptControl(false)
{

}


void DcGambleFish::Animate( double dCurrentTime,f32 fElapsedTime )
{
    QcFish::Animate(dCurrentTime, fElapsedTime);

    if (GetState() == eDying)
    {
        if (isnotzero(m_fScoreStep))
        {
            m_fCurScore += m_fScoreStep *fElapsedTime;
            if (m_fCurScore >= GetScore())
                m_fCurScore = (f32)GetScore();
            m_oScoreShadowDigit.SetValue(s32(m_fCurScore*m_pBullet->GetMultiple()*m_pBullet->GetLevel()));
        }

        for (u32 i=0; i<3; ++i)
            m_pSelectUi[i]->Animate(dCurrentTime, fElapsedTime);
        m_pResult->Animate(dCurrentTime, fElapsedTime);
        m_pScoreBg->Animate(dCurrentTime, fElapsedTime);

        m_pScaleTips->Animate(dCurrentTime, fElapsedTime);

        m_oScoreDigit.Animate(dCurrentTime, fElapsedTime);
        m_oScoreShadowDigit.Animate(dCurrentTime, fElapsedTime);
        m_oTimesDigit.Animate(dCurrentTime, fElapsedTime);

        m_pOver->Animate(dCurrentTime, fElapsedTime);
        m_pCounter->Animate(dCurrentTime, fElapsedTime);
    }
}

void DcGambleFish::RenderSub( QcDeviceRender& render, const QcMatrix* pWorldMatrix )
{
    QcFish::RenderSub(render, pWorldMatrix);

    if (GetState() == eDying)
    {
        m_pScoreBg->Render(render);
        for (u32 i=1; i<=3; ++i)
            m_pSelectUi[(m_iSelectId+i)%3]->Render(render);
        m_pResult->Render(render);

        QcMatrix mat(m_pScoreBg->GetMatrix());

        m_oTimesDigit.Render(render, &mat);
        m_oScoreShadowDigit.Render(render, &mat);
        m_oScoreDigit.Render(render, &mat);
        m_pOver->Render(render, &mat);
        m_pCounter->Render(render, &mat);

        m_pScaleTips->Render(render, &mat);
    }
}

void DcGambleFish::HitToDie( QcBullet* pBullet )
{
    QmSceneMgr->LockNormalScene(true);
	QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, "打中比倍鱼时", GetPosition());
	int iPlayMusicIndex = rand()%3;
	if(iPlayMusicIndex == 0)
		QmPlaySound(CatchGambleFish);
	else if (iPlayMusicIndex == 1)
		QmPlaySound(CatchGambleFish2);
	else
		QmPlaySound(CatchGambleFish3);

	m_pBgm = QmSoundMgr->FindSound(QmObjName(GambleFishBgm));
	if (m_pBgm)
	{
		QcObjType oCurrentBgmObj = QmSceneMgr->GetBgMgr()->GetCurrentBg()->GetMusic();
		m_pCurrentBgm = QmSoundMgr->FindSound(oCurrentBgmObj);
		if(m_pCurrentBgm)
			m_pCurrentBgm->BgmPause(true);
		m_pBgm->Play(true);
	}
    m_pBullet = pBullet;
    m_pOwner = pBullet->GetOwner();
    m_fCurScore = (f32)GetScore();

    SendEvent(1, 1, m_pOwner);

    ToDie(false);
    RemoveAnimator<QcAnimatorTexture>();
    QmSceneMgr->GetFishMgr()->RemoveFish(this);
    AddAnimator(MakeTextureAnimator(eLiving));   

    QcPoint vMoveEndPos;
    m_pOwner->GetMatrix().transformVect(vMoveEndPos, QcPoint(0, 230));
    f32 fMoveDistance = vMoveEndPos.getDistanceFrom(m_vPos);
    f32 fInitSpd = 2*fMoveDistance / FISH_MOVE_TIME;
    f32 fAccelSpd = -fInitSpd / FISH_MOVE_TIME;
    QcAnimatorMove *pMoveAni = QmAnimatorCreator->MakeLineAnimator2(m_vPos, vMoveEndPos, fInitSpd)->As<QcAnimatorMove*>();
    QcGravitySpeed *pSpdAni = new QcGravitySpeed(fAccelSpd, FISH_MOVE_TIME);
    pMoveAni->AddSpeedAnimator(pSpdAni);
    pMoveAni->SET_ANI_CB(0);

	//SetScale(FishScale);

    AddAnimator(pMoveAni);

    f32 fSrcRad = surroundRad2(m_fRotate);
    f32 fDscRad = surroundRad2(m_pOwner->GetRotation()+PI);
    QiAnimator *pAni = QmAnimatorCreator->MakeRotationAnimator((fDscRad-fSrcRad)/FISH_MOVE_TIME, fDscRad-fSrcRad);
    AddAnimator(pAni);
    pAni = QmAnimatorCreator->MakeScaleAnimator2(1.0f, FishScaleS, FISH_MOVE_TIME);
    AddAnimator(pAni);

    QcObjType SelectType[3] = {QmObjName(SelectAbandon),QmObjName(SelectLess),QmObjName(SelectGreat)};
    for (u32 i=0; i<3; ++i)
    {
        m_pSelectUi[i] = QmGlobalData.gpResMgr->MakeGameInstance(SelectType[i]);
        m_pSelectUi[i]->SetRotation(m_pOwner->GetRotation());
        m_pSelectUi[i]->GetSubSceneNode(0)->SetCurrentTransparency(0);
        m_pSelectUi[i]->SetVisible(false);
    }

    m_pScoreBg = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(GambleFishScoreBg));
    m_pScoreBg->SetRotation(m_pOwner->GetRotation());
    m_pScoreBg->SetVisible(false);

    m_pOver = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(GambleOver));
    m_pOver->SetVisible(false);

    m_pCounter = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(GambleCounter));
    m_pCounter->SetVisible(false);

    m_oScoreDigit.SetDigitalType(QmObjName(GambleScore));
    m_oScoreDigit.SetVisible(false);
    m_oScoreShadowDigit.SetDigitalType(QmObjName(GambleScore));
    m_oScoreShadowDigit.SetVisible(false);
    m_oTimesDigit.SetDigitalType(QmObjName(GambleWinTimes));
    m_oTimesDigit.SetVisible(false);

    m_pScaleTips = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(GambleFishScaleTips));
    m_pScaleTips->SetVisible(false);

    m_pResult = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(GambleFishResult));
    m_pResult->SetVisible(false);

    m_iWinTimes = 0;
    m_fScoreStep = 0;

	m_pOwner->GetInfo()->AddCatchScore(GetScore() * m_pBullet->GetMultiple() * m_pBullet->GetLevel());
}

void DcGambleFish::OnTimer( u32 nTimer, void* pContext )
{
    switch(nTimer)
    {
    case 10000:
        {
            if (!m_pOver->IsVisible())
            {
                if (!pContext)
                {
                    QiAnimator *pAni = m_pCounter->MakeTextureAnimator(eLiving);
                    pAni->SET_ANI_CB(12);
                    m_pCounter->RemoveAnimator(NULL);
                    m_pCounter->AddAnimator(pAni);
                    m_pCounter->SetVisible(true);
                    SetTimer(10000, 5, true, (void*)5);
                    QmPlaySound(BombCounter10);
                }
                else
                {
                    char szSoundType[100] = {0};
                    sprintf_s(szSoundType, 99, "BombCounter%d", (int)pContext);
                    QcSound *pSound = QmSoundMgr->FindSound(szSoundType);
                    if (pSound) pSound->Play(false);
                    SetTimer(10000, 1, true, (void*)(((int)pContext)-1));
                }

            }
            else
            {
                SendEvent(0);
            }
        }
        break;
		
	case 10001:
		{
			QcPoint pos = GetPosition();
			pos.X -= 50 * cos(GetRotation());
			pos.Y -= 50 * sin(GetRotation());
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, "色子结果出现时", pos);
		}
		break;

    case 10002:
        {
            m_bAcceptControl = true;
        }
        break;

    case 10003:
        {
            QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "比倍鱼结算时", m_pSelectUi[1]->GetPosition(), 1, 1, m_pOwner->GetRotation());
            QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "比倍鱼结算时", QcPoint(-80,0).rotateByRad(m_pOwner->GetRotation())+m_pSelectUi[1]->GetPosition(), 1, 1, m_pOwner->GetRotation());
            QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "比倍鱼结算时", QcPoint(80,0).rotateByRad(m_pOwner->GetRotation())+m_pSelectUi[1]->GetPosition(), 1, 1, m_pOwner->GetRotation());

            m_fCurScore = (f32)GetScore();
            m_oScoreShadowDigit.SetValue(GetScore()*m_pBullet->GetLevel()*m_pBullet->GetMultiple());
            m_fScoreStep = 0;

            //QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(3.f, 1, 0.5f);
            //m_oScoreDigit.AddAnimator(pAni);
            //m_oScoreDigit.SetVisible(true);
            //m_oScoreDigit.SetScale(3.f);
            //m_oScoreDigit.SetPosition(m_oScoreShadowDigit.GetPosition());
            //pAni->SET_ANI_CB(7);
            //pAni = QmAnimatorCreator->MakeAlphaAnimator2(0.3f, 1, 0.5f);
            //m_oScoreDigit.SetCurrentTransparency(0);
            //m_oScoreDigit.AddAnimator(pAni);
            //m_oScoreDigit.SetValue(GetScore()*m_pBullet->GetLevel()*m_pBullet->GetMultiple());
            SetTimer(10004, 0.5f, true);
        }
        break;

    case 10004:
        AnimationFinishCallBack(7);
        break;

    default:
        QcFish::OnTimer(nTimer, pContext);
        break;
    }
}

void DcGambleFish::OnEvent( const QsEvent& evt )
{
    if (GetState() != eDying) return;

    switch(evt.m_iEvt)
    {
    case 0: //选择项
        {
            if (!m_bAcceptControl)
                break;

            if (evt.m_iParam == 0)
            {
                m_pSelectUi[m_iSelectId]->RemoveAnimator(NULL);
                m_pSelectUi[m_iSelectId]->SetScale(1.f);

                QiAnimator *pAni = QmAnimatorCreator->MakeLineAnimator(
                    QcPoint(), QcPoint(-100,0).rotateByRad(m_pOwner->GetRotation()), 0.5f);
                m_pSelectUi[0]->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeLineAnimator(
                    QcPoint(),QcPoint(100,0).rotateByRad(m_pOwner->GetRotation()),0.5f);
                m_pSelectUi[2]->AddAnimator(pAni);
                pAni->SET_ANI_CB(2);
				if (m_iSelectId == 1)
					QmPlaySound(GambleFishSmaller);
				if (m_iSelectId == 2)
					QmPlaySound(GambleFishBig);
					
                KillTimer(10000);
                m_pCounter->RemoveAnimator(NULL);
                m_pCounter->SetVisible(false);

				if (m_iSelectId == 0 && m_pOver->IsVisible())
				{
                    pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.5f);
                    m_pOver->AddAnimator(pAni);
                }

                m_bAcceptControl = false;
            }

            else
            {
                if (m_pOver->IsVisible())
                    break;

                QiAnimator *pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.3f);
                m_pSelectUi[m_iSelectId]->RemoveAnimator(NULL);
                m_pSelectUi[m_iSelectId]->GetSubSceneNode(0)->RemoveAnimator(NULL);
                m_pSelectUi[m_iSelectId]->GetSubSceneNode(0)->AddAnimator(pAni);
                if (evt.m_iParam == 2)
                    m_iSelectId = m_iSelectId == 0 ? 2 : m_iSelectId-1;
                else if (evt.m_iParam == 1)
                    m_iSelectId = m_iSelectId == 2 ? 0 : m_iSelectId+1;

                pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.3f);
                m_pSelectUi[m_iSelectId]->GetSubSceneNode(0)->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeScaleAnimator2(1.0f, 0.9f, 0.1f, true);
                pAni->SetStartTime(0.3f);
                m_pSelectUi[m_iSelectId]->AddAnimator(pAni);

                m_bAcceptControl = false;
                SetTimer(10002, 0.3f, true);
                QmPlaySound(GambleFishChosen);
            }
        }
        break;

    }
}

void DcGambleFish::AnimationFinishCallBack( u32 iParam )
{
    switch(iParam)
    {
        case 0:
            {
				int iPlayMusicIndex = rand()%4;
				if(iPlayMusicIndex == 0)
					QmPlaySound(GambleFishSelect);
				else if(iPlayMusicIndex == 1)
					QmPlaySound(GambleFishSelect2);
				else if(iPlayMusicIndex == 2)
					QmPlaySound(GambleFishSelect3);
				else
					QmPlaySound(GambleFishSelect4);
                m_pScoreBg->SetPosition(QcPoint(0,30).rotateByRad(m_pOwner->GetRotation())+m_vPos);
                m_pScoreBg->SetVisible(true);
                m_pScoreBg->SetScale(0);
                QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(0,FishScoreShowScaleB,0.5f);
                m_pScoreBg->AddAnimator(pAni);

                QcPoint pos(QcPoint(0,-140).rotateByRad(m_pOwner->GetRotation())+m_vPos);
                m_pSelectUi[0]->SetPosition(pos);
                m_pSelectUi[0]->SetVisible(true);
                pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.3f);
                pAni->SET_ANI_CB(1);
                pAni->SetStartTime(0.5f);
                m_pSelectUi[m_iSelectId]->GetSubSceneNode(0)->AddAnimator(pAni);

                pAni = QmAnimatorCreator->MakeLineAnimator(
                    QcPoint(),QcPoint(100,0).rotateByRad(m_pOwner->GetRotation()),0.5f);
                m_pSelectUi[0]->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeScaleAnimator2(1.0f, 0.9f, 0.1f, true);
                pAni->SetStartTime(0.3f);
                m_pSelectUi[m_iSelectId]->AddAnimator(pAni);

                m_pSelectUi[1]->SetPosition(pos);
                m_pSelectUi[1]->SetVisible(true);

                m_pSelectUi[2]->SetPosition(pos);
                m_pSelectUi[2]->SetVisible(true);
                pAni = QmAnimatorCreator->MakeLineAnimator(
                    QcPoint(),QcPoint(-100,0).rotateByRad(m_pOwner->GetRotation()),0.5f);
                m_pSelectUi[2]->AddAnimator(pAni);

                QcPoint pos2(0,80);
                m_pOver->SetPosition(pos2);
                m_pCounter->SetPosition(pos2);

                RemoveAnimator<QcAnimatorTexture>();
                pAni = MakeTextureAnimator(StringToState("001"));
                AddAnimator(pAni);
                SetRotation(m_pOwner->GetRotation()+PI);
            }
            break;

        case 1:
            {
                m_oScoreShadowDigit.SetValue(GetScore()*m_pBullet->GetMultiple()*m_pBullet->GetLevel());
                m_oScoreShadowDigit.SetPosition(QcPoint(0,-40));
                m_oScoreShadowDigit.SetVisible(true);
                QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(1.3f, 1.f, 0.2f);
                m_oScoreShadowDigit.AddAnimator(pAni);
                m_oScoreShadowDigit.SetScale(1.3f);
                pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.2f);
                m_oScoreShadowDigit.AddAnimator(pAni);
                m_oScoreShadowDigit.SetCurrentTransparency(0);

                m_oTimesDigit.SetPosition(QcPoint(0,40));
                m_oTimesDigit.SetUnitType(QmObjName(GambleFishWins));

                m_bAcceptControl = true;
                SetTimer(10000, 5, true);                
            }
            break;

        case 2:
            {
                if (m_iSelectId == 0)
                {
                    ExitGamble();
                }
                else
                {
                    QmAssert(m_iSelectId==1 || m_iSelectId==2);

                    if (!m_pDice)
                        m_pDice = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(GambleFishDice));

                    QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(FishScaleS, FishScaleB, 0.5f);
                    AddAnimator(pAni);
                    pAni = QmAnimatorCreator->MakeScaleAnimator2(FishScoreShowScaleB, FishScoreShowScaleS,0.5f);
                    m_pScoreBg->AddAnimator(pAni);
                    pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.5f);
                    m_pScoreBg->AddAnimator(pAni);
                    m_oTimesDigit.AddAnimator(pAni);
                    m_oScoreShadowDigit.AddAnimator(pAni);

                    pAni->SET_ANI_CB(3);
                }
            }
            break;

        case 3:
            {
				if (QmFishAlg->StartGamble(m_pBullet->GetLevel()*m_pBullet->GetMultiple()*GetScore(), 2))
                    GambleSuccess();
#ifdef QmTestVersion
                else if (QmDebugHelper.KillFishFlag() & (1<<3))
                    GambleSuccess();
#endif
                else
                    GambleFail();


                QmPlaySound(GambleFishDice);
            }
            break;

        case 4:
            {
				QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, "比倍成功时", GetPosition());
				QmPlaySound(GambleFishSuccess);
                RemoveSubSceneNode(m_pDice);

                m_pResult->SetVisible(true);
                m_pResult->SetScale(1.2f);
                m_pResult->SetCurrentTransparency(0);
                QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(1.2f, 1, 0.5f);
                m_pResult->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.5f);
                m_pResult->AddAnimator(pAni);

                RemoveAnimator<QcAnimatorTexture>();
                const QcTexturePtrList &tmp = m_pDesc->GetStateParameter(StringToState("004"))->m_pTextureList;
                QcTexturePtrList tmp2(tmp);
                tmp2.insert(tmp2.end(),tmp.begin(),tmp.end());
                pAni = QmAnimatorCreator->MakeTextureAnimator(tmp2, 1.f/24, false);
                AddAnimator(pAni);
                pAni->SET_ANI_CB(5);
				//////////////////////////////////////////////////////////////////////////
				pAni = QmAnimatorCreator->MakeScaleAnimator2(FishScaleB,FishScaleS,0.5f);
				AddAnimator(pAni);
				//////////////////////////////////////////////////////////////////////////
            }
            break;

        case 5:
            {
                RemoveAnimator<QcAnimatorTexture>();
                AddAnimator(MakeTextureAnimator(StringToState("001")));
                QiAnimator *pAni; /*= QmAnimatorCreator->MakeScaleAnimator2(FishScaleB,FishScaleS,0.5f);*/
                //AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeScaleAnimator2(FishScoreShowScaleS, FishScoreShowScaleB, 0.5f);
                pAni->SetStartTime(0.5f);
                m_pScoreBg->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.5f);
                pAni->SetStartTime(0.5f);
                m_pScoreBg->AddAnimator(pAni);
                m_oScoreShadowDigit.AddAnimator(pAni);
                m_oTimesDigit.AddAnimator(pAni);
                pAni->SET_ANI_CB(6);
                
                pAni = QmAnimatorCreator->MakeLineAnimator(
                    QcPoint(),QcPoint(100,0).rotateByRad(m_pOwner->GetRotation()),0.5f);
                pAni->SetStartTime(0.5f);
                m_pSelectUi[0]->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeLineAnimator(
                    QcPoint(),QcPoint(-100,0).rotateByRad(m_pOwner->GetRotation()),0.5f);
                pAni->SetStartTime(0.5f);
                m_pSelectUi[2]->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.3f);
                pAni->SetStartTime(0.5f);
                m_pSelectUi[m_iSelectId]->GetSubSceneNode(0)->AddAnimator(pAni);

                pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.5f);
                m_pResult->AddAnimator(pAni);
            }
            break;

        case 6:
            {
                int index = 0;
                if (m_iWinTimes == 1)
                {
                    QmPlaySound(GambleFishMusic001); 
                    index = RandomInt2(3,4);
                }
                else if (m_iWinTimes == 2)
                {
                    QmPlaySound(GambleFishMusic002);	
                    index = RandomInt2(3,4);
                }
                else if (m_iWinTimes == 3)
                {
                    QmPlaySound(GambleFishMusic003);	
                    index = RandomInt2(3,4);
                }
                else if (m_iWinTimes == 4)
                {
                    QmPlaySound(GambleFishMusic004);
                    index = RandomInt2(3,4);	
                }
                else if (m_iWinTimes == 5)
                {
                    QmPlaySound(GambleFishMusic005);
                    index = RandomInt2(5,6);	
                }
                else if (m_iWinTimes == 6)
                {
                    QmPlaySound(GambleFishMusic006);
                    index = RandomInt2(7,8);		
                }
                else if (m_iWinTimes == 7)
                {
                    QmPlaySound(GambleFishMusic007);
                    index = RandomInt2(7,8);			
                }
                else
                {
                    QmPlaySound(GambleFishMusic008);
                    index = RandomInt2(7,9);			
                }

                QcSound* psTempSound = QmSoundMgr->FindSound(QcGambleCVList[index]);
                if (psTempSound)
                    psTempSound->Play(false);

                QiAnimator *pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.5f);

                m_oTimesDigit.SetVisible(true);
                m_oTimesDigit.SetValue(++m_iWinTimes);
                m_oTimesDigit.SetCurrentTransparency(0);
                m_oTimesDigit.AddAnimator(pAni);

                SetTimer(10003, 1, true);
                SetScore(u32(m_fCurScore*2));
                m_fScoreStep = m_fCurScore / 1.5f;
            }
            break;

        case 7:
            {
                m_oScoreDigit.SetVisible(false);
                u32 iScore = u32(m_fCurScore)*m_pBullet->GetLevel()*m_pBullet->GetMultiple();
                m_oScoreShadowDigit.SetValue(iScore);

                m_iSelectId = GambleDefault;
                QiAnimator *pAni = NULL;

                m_bAcceptControl = true;
                SetTimer(10000, 5, true);
                if (iScore >= QmSysSetting->GetGambleOver())
                {
                    QcEffect *pEff = QmGlobalData.gpResMgr->MakeEffect("ColorWiper");
                    m_pSelectUi[1]->SetRender(pEff);
                    m_pSelectUi[2]->SetRender(pEff);

                    m_pOver->SetVisible(true);
                    m_pOver->SetCurrentTransparency(0);
                    pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.5f);
                    m_pOver->AddAnimator(pAni);

                    m_iSelectId = 0;
                }

                pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.3f);
                m_pSelectUi[m_iSelectId]->GetSubSceneNode(0)->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeScaleAnimator2(1.f,0.9f,0.1f,true);
                pAni->SetStartTime(0.3f);
                m_pSelectUi[m_iSelectId]->AddAnimator(pAni);
            }
            break;

        case 8:
            {
				if(m_pBgm)
				{
					if(m_pBgm->IsPlaying())
						m_pBgm->Stop();
				}
				if(m_pCurrentBgm)
					m_pCurrentBgm->BgmPause(false);
				
                m_pOwner->GetInfo()->AddScore(GetScore()*m_pBullet->GetMultiple()*m_pBullet->GetLevel());
                m_pOwner->UpdateScore();
                QmSceneMgr->GetFishMgr()->OnDead(this);
                QmSceneMgr->LockNormalScene(false);
            }
            break;

        case 9:
            {
				QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, "比倍失败时", GetPosition());
				QmPlaySound(GambleFishFail);
                RemoveSubSceneNode(m_pDice);

                m_pResult->SetVisible(true);
                m_pResult->SetScale(1.2f);
                m_pResult->SetCurrentTransparency(0);
                QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(1.2f, 1, 0.5f);
                m_pResult->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.5f);
                m_pResult->AddAnimator(pAni);

                RemoveAnimator<QcAnimatorTexture>();
                pAni = MakeTextureAnimator(StringToState("001"));
                AddAnimator(pAni);

                pAni = QmAnimatorCreator->MakeScaleAnimator2(FishScaleB, FishScaleS, 0.5f);
                pAni->SetStartTime(0.5f);
                AddAnimator(pAni);

                pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.5f);
                pAni->SetStartTime(1.f);
                m_pResult->AddAnimator(pAni);
                pAni->SET_ANI_CB(10);
            }
            break;

        case 10:
            {
				if(rand()%2)
					QmPlaySound(GambleFishPlayerLose01);
				else
					QmPlaySound(GambleFishPlayerLose02);

                QiAnimator *pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.3f);
                pAni->SetStartTime(1.5f);
                m_pSelectUi[m_iSelectId]->AddAnimator(pAni);
                m_pSelectUi[(m_iSelectId+1)%3]->SetVisible(false);
                m_pSelectUi[(m_iSelectId+2)%3]->SetVisible(false);

                RemoveAnimator<QcAnimatorTexture>();
                pAni = MakeTextureAnimator(StringToState("006"));
                AddAnimator(pAni);
                SetRotation(m_pOwner->GetRotation());
                pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.3f);
                pAni->SetStartTime(2.3f);
                pAni->SET_ANI_CB(11);
                AddAnimator(pAni);

                SendEvent(1, 2, m_pOwner);
            }
            break;

        case 11:
            {
				if(m_pBgm)
				{
					if(m_pBgm->IsPlaying())
						m_pBgm->Stop();
				}
				if(m_pCurrentBgm)
                    m_pCurrentBgm->BgmPause(false);

                QmSceneMgr->GetFishMgr()->OnDead(this);
                QmSceneMgr->LockNormalScene(false);
            }
            break;

        case 12:
            {
                if (m_iSelectId != 0)
                {
                    m_pSelectUi[m_iSelectId]->RemoveAnimator(NULL);
                    m_pSelectUi[m_iSelectId]->SetScale(1);
                    QiAnimator *pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.3f);
                    m_pSelectUi[m_iSelectId]->GetSubSceneNode(0)->AddAnimator(pAni);

                    m_iSelectId = 0;
                    pAni = QmAnimatorCreator->MakeAlphaAnimator2(0,1,0.3f);
                    m_pSelectUi[m_iSelectId]->GetSubSceneNode(0)->AddAnimator(pAni);
                }

                SendEvent(0);
            }
            break;
    }
}

void DcGambleFish::ExitGamble()
{
	QmPlaySound(GambleFishEndAndAddScore);
    if (!m_bShowedCoin && m_pOwner)
    {
        QmSceneMgr->GetAnimationMgr()->MakeCoin(GetScore(), m_pBullet->GetMultiple()*m_pBullet->GetLevel(),
            m_pScoreBg->GetPosition(), m_pOwner->GetAbsCoinEndPt(), m_pOwner->GetRotation(), 0.f);
        m_bShowedCoin = true;
    }

    QiAnimator *pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.3f);
    m_pSelectUi[0]->AddAnimator(pAni);
    m_oScoreShadowDigit.AddAnimator(pAni);
    m_oTimesDigit.AddAnimator(pAni);
    m_pScoreBg->AddAnimator(pAni);

    for (u32 i=1; i<3; ++i)
        m_pSelectUi[i]->SetVisible(false);

    RemoveAnimator<QcAnimatorTexture>();
    pAni = MakeTextureAnimator(StringToState("005"));
    AddAnimator(pAni);
    SetRotation(m_pOwner->GetRotation());
    pAni = QmAnimatorCreator->MakeAlphaAnimator2(1,0,0.3f);
    pAni->SetStartTime(2.3f);
    pAni->SET_ANI_CB(8);
    AddAnimator(pAni);

    SendEvent(1, 2, m_pOwner);
}

void DcGambleFish::GambleSuccess()
{
	int InOutScore = int(m_pBullet->GetLevel()*m_pBullet->GetMultiple()*m_fCurScore);
	m_pOwner->GetInfo()->Gamble(InOutScore);
	m_pOwner->GetInfo()->AddCatchScore(InOutScore*2);

    m_pDice->RemoveAnimator<QcAnimatorTexture>();
    QcTexturePtrList pTextureList(m_pDice->GetDesc()->GetStateParameter(StringToState("000"))->m_pTextureList);
    char tmp[10];
    sprintf_s(tmp, "%0.3d", m_iSelectId==1 ? RandomInt2(1,3) : RandomInt2(4,6));
    const QcTexturePtrList& temp = m_pDice->GetDesc()->GetStateParameter(StringToState(tmp))->m_pTextureList;
    pTextureList.insert(pTextureList.end(), temp.begin(), temp.end());

    QiAnimator *pAni = QmAnimatorCreator->MakeTextureAnimator(pTextureList, 1.f/24, false);
    m_pDice->AddAnimator(pAni);
    pAni->SET_ANI_CB(4);
    AddSubSceneNode(m_pDice);

	SetTimer(10001, 95.f / 24.f, true);

    pAni = MakeTextureAnimator(StringToState("003"));
    AddAnimator(pAni);

	QcPoint pt;
	GetMatrix().transformVect(pt, m_pDice->GetPosition()+QcPoint(-50,0));
	QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, "比倍鱼色子转动时", pt);

    m_pResult->RemoveAnimator<QcAnimatorTexture>();
    m_pResult->AddAnimator(m_pResult->MakeTextureAnimator(m_iSelectId==1?eDying:eLiving));
    m_pResult->SetPosition(m_vPos);
    m_pResult->SetRotation(m_pOwner->GetRotation());
}

void DcGambleFish::GambleFail()
{
	int InOutScore = int(m_pBullet->GetLevel()*m_pBullet->GetMultiple()*m_fCurScore);
	m_pOwner->GetInfo()->Gamble(InOutScore);

    m_pDice->RemoveAnimator<QcAnimatorTexture>();
    QcTexturePtrList pTextureList(m_pDice->GetDesc()->GetStateParameter(StringToState("000"))->m_pTextureList);
    char tmp[10];
    sprintf_s(tmp, "%0.3d", m_iSelectId==2 ? RandomInt2(1,3) : RandomInt2(4,6));
    const QcTexturePtrList& temp = m_pDice->GetDesc()->GetStateParameter(StringToState(tmp))->m_pTextureList;
    pTextureList.insert(pTextureList.end(), temp.begin(), temp.end());

    QiAnimator *pAni = QmAnimatorCreator->MakeTextureAnimator(pTextureList, 1.f/24, false);
    m_pDice->AddAnimator(pAni);
    pAni->SET_ANI_CB(9);
    AddSubSceneNode(m_pDice);
	SetTimer(10001, 95.f / 24.f , true);

    pAni = MakeTextureAnimator(StringToState("003"));
    AddAnimator(pAni);

	QcPoint pt;
	GetMatrix().transformVect(pt, m_pDice->GetPosition()+QcPoint(-50,0));
	QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, "比倍鱼色子转动时", pt);

    m_pResult->RemoveAnimator<QcAnimatorTexture>();
    m_pResult->AddAnimator(m_pResult->MakeTextureAnimator(m_iSelectId==2?eDying:eLiving));
    m_pResult->SetPosition(m_vPos);
    m_pResult->SetRotation(m_pOwner->GetRotation());
}
