#include "Pch.hpp"
#include "DcChainFish.hpp"
#include "Res\QcResFishDesc.hpp"
#include "Scene\QcBullet.hpp"
#include "Scene\QcPlayer.hpp"
#include "Scene\QcSceneAnimationMgr.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "QcFishFilter.hpp"
#include "QcFishMgr.hpp"
#include "Animator\QiAnimatorDeque.hpp"

ImplRuntimeClass(DcChainLinkFish);

#define DRAG_DELAY_TIMER        10001
#define END_DRAG_TIMER          10002
#define FIND_NEXT_DELAY_TIMER   10003
#define FIND_AGAIN_DELAY_TIMER  10004

#define FIND_AND_DRAG_NOTICE    10001
#define STOP_DRAG_NOTICE        10002

#define ANI_CALLBACK boost::bind(&DcChainLinkFish::AnimatorFinishCallBack, this, _1)

DcChainLinkFish::DcChainLinkFish()
{

}
void DcChainLinkFish::Init( const QcResImageDesc& desc, bool bLoadSubNode )
{
    QcFish::Init(desc, false);

    if (!m_pFish)
    {
        static QcObjType FishTypeList[] = {
            QmObjName(fish01),
            QmObjName(fish02),
            QmObjName(fish03),
            QmObjName(fish04),
            QmObjName(fish05),
            QmObjName(fish06),
            QmObjName(fish07),
            QmObjName(fish08),
            QmObjName(fish09),
        };

        QcResFishDesc *pDesc = QmGlobalData.gpResMgr->GetImageDesc(FishTypeList[RandomInt(0,QmCountOf(FishTypeList))])->As<QcResFishDesc*>();
        if (!pDesc)
            QmLogError("ChainLink Fish Type Not Found!");

        m_pFish = QmGlobalData.gpResMgr->MakeSceneNodeInstance<QcSceneNode>(pDesc);
        SetScore(pDesc->GetScore());
        m_iChainScore = RandomInt2(30,60);
    }

    m_pChainShell = QmGlobalData.gpResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjName(ChainShell));
    m_pChainRope = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(ChainRope));
    m_pChainRope->SetVisible(false);

    m_pChainIdBg = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(ChainFishId));
    m_pChainIdBg->SetVisible(false);

    m_iCurScore = 0;

    f32 fSize = QcMath::max_(m_pFish->GetWidth(),m_pFish->GetHeight()) + 50.f;
    m_pChainShell->ScaleTo(fSize, fSize);
    m_pChainShell->SetWidth(fSize);
    m_pChainShell->SetHeight(fSize);
    SetWidth(fSize);
    SetHeight(fSize);
}

void DcChainLinkFish::Animate( double dCurrentTime,f32 fElapsedTime )
{
    QcFish::Animate(dCurrentTime, fElapsedTime);

    if (m_pChainEff)
        m_pChainEff->Animate(dCurrentTime, fElapsedTime);

    m_pChainRope->Animate(dCurrentTime, fElapsedTime);
    m_pChainShell->Animate(dCurrentTime, fElapsedTime);
    m_pChainIdBg->Animate(dCurrentTime, fElapsedTime);
    m_oChainFishIdDigital.Animate(dCurrentTime, fElapsedTime);

    m_pFish->Animate(dCurrentTime, fElapsedTime);

    m_arChainList.Animate(dCurrentTime, fElapsedTime);

    if (m_pOwner)
    {
        QcPoint tmpPt(0, 100);
        tmpPt.rotateByRad(m_pOwner->GetRotation());
        tmpPt += m_pOwner->GetPosition();
        QcVector tmpVec = m_vPos - tmpPt;
        f32 fRopeRotation = (f32)tmpVec.getRadAngleTrig();
        f32 fLen = tmpVec.getLength();
        m_pChainRope->SetRotation(fRopeRotation);
        m_pChainRope->SetPosition(m_vPos.getInterpolated(tmpPt, (fLen-GetWidth()*.5f)*.5f / fLen));
        tmpVec.normalize(fLen);
        m_pChainIdBg->SetPosition(m_vPos - tmpVec*GetWidth()*.5f);
    }

    for (u32 i=0; i<m_arChainList.size(); i++)
    {
        DcChainLinkFish *pNex = m_arChainList[i].Get()->As<DcChainLinkFish*>();
        if (pNex->FindMoveAnimator())
        {
            if (pNex->m_pChainShell->IsVisible())
            {
                QcFish *pCur = i==0 ? this : m_arChainList[i-1].Get();
                if (pNex->m_pChainRope->IsVisible())
                    pNex->m_pChainRope->SetPosition(pCur->GetPosition().getInterpolated(pNex->GetPosition(), 0.5f));
                if (pNex->m_pChainIdBg->IsVisible())
                {
                    QcVector vec = pCur->GetPosition() - pNex->GetPosition();
                    vec.normalize();
                    pNex->m_pChainIdBg->SetPosition(pNex->GetPosition()+vec*pNex->GetWidth()*.5f);
                }

                if (m_arChainList.size() > 1)
                {
                    for (u32 j=0; j<m_arChainList.size()-1; j++)
                    {
                        QcFish *pFish = j==0 ? this : m_arChainList[j].Get();
                        f32 fLen = (pNex->GetWidth()+pFish->GetWidth())*0.5f;
                        if (pNex->GetPosition().getDistanceFromSQ(pFish->GetPosition()) < fLen*fLen)
                        {
                            PostEvent(STOP_DRAG_NOTICE);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void DcChainLinkFish::Render( QcDeviceRender& render, const QcMatrix* pWorldMatrix )
{
    QcMatrix mat = pWorldMatrix ? (*pWorldMatrix) * GetMatrix() : GetMatrix();

    m_pChainRope->Render(render, pWorldMatrix);
    m_pFish->Render(render, &mat);
    m_pChainShell->Render(render, &mat);
    m_pChainIdBg->Render(render, pWorldMatrix);
    if (m_pChainEff)
        m_pChainEff->Render(render, &mat);

    m_oChainFishIdDigital.SetPosition(m_pChainIdBg->GetPosition());
    m_oChainFishIdDigital.Render(render, pWorldMatrix);

    m_arChainList.Render(render, pWorldMatrix);
}

void DcChainLinkFish::HitToDie( QcBullet* pBullet )
{
    QmSceneMgr->LockNormalScene(true);
    ToDie(false);

    m_pBullet = pBullet;
    m_pOwner = pBullet->GetOwner();

    MakeStartDragAni();

    QcPoint tmpPt(0, 400 + GetWidth()*.5f);
    tmpPt.rotateByRad(m_pOwner->GetRotation());
    tmpPt += m_pOwner->GetPosition();
    QiAnimator *pAni = QmAnimatorCreator->MakeLineAnimator2(m_vPos, tmpPt, 300);
    pAni->AddFinishCallBack(ANI_CALLBACK, 0);
    pAni->SetStartTime(0.5f);
    AddAnimator(pAni);

    //m_pChainId->SetVisible(true);
    //m_pChainId->SetRotation(m_pOwner->GetRotation());
    //m_pChainId->SetDigitalValue(1);

    QcPoint tmpPt2(0, 100);
    tmpPt2.rotateByRad(m_pOwner->GetRotation());
    tmpPt2 += m_pOwner->GetPosition();
    pAni = QmAnimatorCreator->MakeStretchAnimator(tmpPt2);
    m_pChainRope->AddAnimator(pAni);
    m_pChainRope->SetVisible(true);
    m_pChainRope->SetScale(0);

    m_pChainIdBg->SetRotation(GetRotation());
    m_pChainIdBg->SetVisible(true);

    m_oChainFishIdDigital.SetDigitalType(QmObjName(ChainFishIdDigital));
    m_oChainFishIdDigital.SetRotation(m_pOwner->GetRotation());
    m_oChainFishIdDigital.SetValue(1);

    QcResImageDescPtr pEmptyDesc = QmGlobalData.gpResMgr->GetEmptyImageDesc();
    m_pChainLogo = QmGlobalData.gpResMgr->MakeSceneNodeInstance<QcSceneAnimation>(pEmptyDesc);
    m_pChainLogo->SetPosition(m_pOwner->GetPosition());
    m_pChainLogo->SetRotation(m_pOwner->GetRotation());

    QcSceneAnimation *pSceneAni  = QmGlobalData.gpResMgr->MakeSceneNodeInstance<QcSceneAnimation>(QmObjName(ChainLogo));
    pSceneAni->SetPosition(QcPoint(0, -256));
    pSceneAni->SetRotation(PI);
    pAni = QmAnimatorCreator->MakeCircleAnimator(QcPoint(), TWO_PI+PI, false, true, TWO_PI+PI);
    pAni->AddFinishCallBack(ANI_CALLBACK, 5);
    pSceneAni->AddAnimator(pAni);

    m_pChainLogo->AddSubSceneNode(pSceneAni);

    QmSceneMgr->GetAnimationMgr()->AddAnimation(m_pChainLogo, eAnimationLayerGunEffect);
    QmPlaySound(ChainLinkStart);
}

void DcChainLinkFish::AnimatorFinishCallBack( u32 iParam )
{
    switch(iParam)
    {
    case 0:
        {
			m_pChainShell->SetState(eDying);
            //m_pChainEff = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(ChainEff));
            SetTimer(DRAG_DELAY_TIMER, 0.5f, true);
        }
        break;

    case 1:
        {
            DcChainLinkFish *pNextChain = m_arChainList.back().Get()->As<DcChainLinkFish*>();
            pNextChain->MakeStartDragAni();

            DcChainLinkFish *pReference = m_arChainList.size() > 1 ? (*(m_arChainList.end()-2)).Get()->As<DcChainLinkFish*>() : this;

            QcPoint DragPt(pNextChain->GetPosition().getDistanceFrom(pReference->GetPosition())
                -pNextChain->GetWidth()*.5f-pReference->GetWidth()*.5f, 0);
            DragPt.rotateByRad(pNextChain->m_pChainRope->GetRotation());
            QiAnimator *pAni = QmAnimatorCreator->MakeLineAnimator2(DragPt, QcPoint(), 300);
            pAni->SetStartTime(0.5f);
            pAni->AddFinishCallBack(ANI_CALLBACK, 2);
            pNextChain->AddAnimator(pAni);
            AnimatorFinishCallBack(5);
        }
        break;

    case 2:
        {
            DcChainLinkFish *pNextChain = m_arChainList.back().Get()->As<DcChainLinkFish*>();
            pNextChain->m_pChainRope->SetVisible(false);
			if(pNextChain)
				pNextChain->m_pChainShell->SetState(eDying);
            SetTimer(DRAG_DELAY_TIMER, 0.5f, true);
        }
        break;

    case 3:
        {
            if (m_arChainList.empty())
                m_pChainShell->SetState(eDying);
        }
        break;

    case 4:
        m_pChainEff->SetVisible(false);
        break;

    case 5:
        {
            if (!m_pChainScaleDigitShadow)
            {
                m_pChainScaleDigitShadow = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(ChainFishScale))->As<QcSceneDigital*>();

                m_pChainScaleDigitShadow->SetPosition(QcPoint(0, 65));
                m_pChainLogo->GetSubSceneNode(QmObjName(ChainLogo))->AddSubSceneNode(m_pChainScaleDigitShadow);
            }


            u32 iScore = GetScore();
            for (u32 i=0; i<m_arChainList.size(); ++i)
                iScore += m_arChainList[i]->GetScore();

            m_pChainScaleDigitShadow->SetDigitalValue(iScore);
            m_pChainScaleDigitShadow->SetScale(5.5f);
            m_pChainScaleDigitShadow->SetCurrentTransparency(0);
            QiAnimator *pAni = QmAnimatorCreator->MakeScaleAnimator2(5.5f, 1.0, 0.5f);
            pAni->AddFinishCallBack(ANI_CALLBACK, 6);
            m_pChainScaleDigitShadow->AddAnimator(pAni);
            pAni = QmAnimatorCreator->MakeAlphaAnimator2(0.0f, 1.0f, 0.5f);
            m_pChainScaleDigitShadow->AddAnimator(pAni);
        }
        break;

    case 6:
        {
            u32 iScore = GetScore();
            for (u32 i=0; i<m_arChainList.size(); ++i)
                iScore += m_arChainList[i]->GetScore();

            if (!m_pChainScaleDigit)
            {
                m_pChainScaleDigit = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(ChainFishScale))->As<QcSceneDigital*>();

                m_pChainScaleDigit->SetPosition(QcPoint(0, 65));
                m_pChainLogo->GetSubSceneNode(QmObjName(ChainLogo))->AddSubSceneNode(m_pChainScaleDigit);
            }

            m_pChainScaleDigit->SetDigitalValue(iScore);
        }

    default:
        break;
    }
}

void DcChainLinkFish::OnEvent( const QsEvent& evt )
{
    switch (evt.m_iEvt)
    {
    case FIND_AND_DRAG_NOTICE:
        FindFishForDrag();
        break;

    case STOP_DRAG_NOTICE:
        {
            DcChainLinkFish *pFish = m_arChainList.back().Get()->As<DcChainLinkFish*>();
            pFish->FinishAnimator(pFish->FindMoveAnimator());
            pFish->m_pChainRope->SetVisible(false);
            //SetTimer(FIND_NEXT_DELAY_TIMER, 1, true);
            AnimatorFinishCallBack(2);
        }
        break;

    default:
        break;
    }
}

//DcChainLinkFish* DcChainLinkFish::MakeChainLinkFish( const QcObjType& objType )
//{
//    return MakeChainLinkFish(QmGlobalData.gpResMgr->GetImageDesc(objType)->As<QcResFishDesc*>());
//}

//DcChainLinkFish* DcChainLinkFish::MakeChainLinkFish( const QcResFishDesc* pFishDesc )
//{
//    static const QcResFishDesc* pChainLinkDesc = QmResMgr->GetImageDesc(QmObjName(ChainLinkFish))->As<QcResFishDesc*>();
//    if (pChainLinkDesc)
//    {
//        DcChainLinkFish *pFish = new DcChainLinkFish();
//        pFish->m_pFish = dynamic_cast<QcFish*>(pFishDesc->MakeInstance(true));
//        pFish->Init(*pChainLinkDesc, false);
//        pFish->SetScore(pFish->m_pFish->GetScore());
//        pFish->m_iChainScore = RandomInt2(30,60);
//        ++g_iChainLinkCount;
//        return pFish;
//    }
//
//    return NULL;
//}

DcChainLinkFish::~DcChainLinkFish()
{
    //if (--g_iChainLinkCount < 0)
    //    g_iChainLinkCount = 0;
}

void DcChainLinkFish::OnTimer( u32 nTimer, void* pContext )
{
    switch (nTimer)
    {
    case DRAG_DELAY_TIMER:
        {
            if (m_iChainScore-m_iCurScore >= 2)
                PostEvent(FIND_AND_DRAG_NOTICE);
            else
                SetTimer(END_DRAG_TIMER, 0.5f, true);
        }
        break;

    case END_DRAG_TIMER:
        {
            m_pFish->SetState(eDying);
            m_pChainRope->SetVisible(false);
            m_pChainShell->SetVisible(false);

            m_arChainList.back()->m_pChainIdBg->SetVisible(false);
            m_arChainList.back()->m_oChainFishIdDigital.SetVisible(false);

            u32 iCoef = m_pBullet->GetLevel()*m_pBullet->GetMultiple();
            int iTotalScore = GetScore()*iCoef;

            MakeDyingAnimations(iTotalScore, m_vPos, 0, m_pFish->GetDesc()->GetDyingTime());

            for (u32 i=0; i<m_arChainList.size(); ++i)
            {
                DcChainLinkFish *pChainFish = m_arChainList[i].Get()->As<DcChainLinkFish*>();
                pChainFish->m_pFish->SetState(eDying);
                pChainFish->m_pChainShell->SetVisible(false);
                pChainFish->m_pChainRope->SetVisible(false);

                pChainFish->m_pOwner = this->m_pOwner;
                u32 iScore = pChainFish->GetScore()*iCoef;
                pChainFish->MakeDyingAnimations(iScore, pChainFish->GetPosition(), 0, pChainFish->m_pFish->GetDesc()->GetDyingTime());
                iTotalScore += iScore;
            }

            SetTimer(eDyingTimer, m_pFish->GetDesc()->GetDyingTime(), true);
            m_pOwner->HitAddScore(iTotalScore, 0);

            QiAnimator *pAni = QmAnimatorCreator->MakeLineAnimator(QcPoint(), QcPoint(0, 30), 0.5f);
            m_pChainLogo->GetSubSceneNode(QmObjName(ChainLogo))->AddAnimator(pAni);

            QcSceneNode *pSceneNode = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(ChainLinkScoreTips));
            QcSceneDigital *pSceneDigital = pSceneNode->GetSubSceneNode(QmObjName(ChainLinkScore))->As<QcSceneDigital*>();
            pSceneDigital->SetDigitalValue(iTotalScore);
            QiAnimatorDeque *pAniDeq = QmAnimatorCreator->MakeAnimatorDeque();
            for (int i=0; i<3; i++)
            {
                pAni = QmAnimatorCreator->MakeLineAnimator(QcPoint(), QcPoint(0, 30), 0.5f);
                pAniDeq->AddAnimator(pAni);
                pAni = QmAnimatorCreator->MakeLineAnimator(QcPoint(), QcPoint(0,-30), 0.5f);
                pAniDeq->AddAnimator(pAni);
            }
            pSceneNode->AddAnimator(pAniDeq);
            m_pChainLogo->AddSubSceneNode(pSceneNode);
            m_pChainLogo->SetLivingTime(3.f);

            iTotalScore -= GetExplodeScore()*iCoef;
            SendEvent(0, iTotalScore, m_pOwner);

            QmPlaySound(ChainEnd);
        }
        break;

    //case FIND_NEXT_DELAY_TIMER:
    //    {
    //        AnimatorFinishCallBack(2);
    //    }
    //    break;

    case FIND_AGAIN_DELAY_TIMER:
        {
            FindFishForDrag();
        }
        break;

    case eDyingTimer:
        {
            MakeCoinAnimation();
            for (u32 i=0; i<m_arChainList.size(); ++i)
            {
                m_arChainList[i]->m_pBullet = m_pBullet;
                m_arChainList[i]->m_pOwner = m_pOwner;
                m_arChainList[i]->MakeCoinAnimation();
            }

            ToDead();
            QmSceneMgr->LockNormalScene(false);
        }
        break;
    default:
        break;
    }
}

void DcChainLinkFish::FindFishForDrag()
{
    QcPoint OrgPos = m_vPos;
    f32 fOrgHalfWidth = GetWidth() * 0.5f;
    if (!m_arChainList.empty())
    {
        OrgPos = m_arChainList.back()->GetPosition();
        fOrgHalfWidth = m_arChainList.back()->GetWidth() * 0.5f;
    }

    QcList<QcFish*> FishList;

    int iSelect = RandomInt2(3, QcMath::Int32_min(20, m_iChainScore-m_iCurScore));
    DcChainLinkFishFilter Filter(iSelect,iSelect);
    QmSceneMgr->GetFishMgr()->SearchHitableFish(FishList, QmGlobalData.gScreenRect, &Filter);

    if (FishList.empty())
    {
        if (m_iChainScore-m_iCurScore <= 2)
            SetTimer(END_DRAG_TIMER, 0.5f, true);
        else
            SetTimer(FIND_AGAIN_DELAY_TIMER, 0.2f, true);
        return;
    }

    int iRandId = RandomInt()%FishList.size();
    QcFish *pFish = NULL;
    for (u32 i=0; i<FishList.size(); i++)
    {
        QcFish *pSearchFish = FishList[(iRandId+i)%FishList.size()];
        if (pSearchFish->GetPosition().getDistanceFromSQ(OrgPos) >= 65536)
        {
            pFish = pSearchFish;
            m_iCurScore += pFish->GetExplodeScore();
            break;
        }
    }

    if (!pFish)
    {
        SetTimer(FIND_AGAIN_DELAY_TIMER, 0.2f, true);
        return;
    }

    QcSharedPtr<DcChainLinkFish> pNextChain(new DcChainLinkFish);
    pNextChain->m_pFish = pFish;
    pNextChain->Init(*m_pDesc, false);
    pNextChain->SetPosition(pFish->GetPosition());
    pNextChain->SetRotation(pFish->GetRotation());
    pNextChain->SetScore(pFish->GetScore());
    pFish->ToDie(false);
    pFish->SetPosition(QcPoint());
    pFish->SetRotation(0);
    pFish->SetState(eLiving);
    QmSceneMgr->GetFishMgr()->OnDead(pFish);

    QcPoint TarPos = pNextChain->GetPosition();
    QcVector tmpVec = TarPos-OrgPos;
    f32 fLen = tmpVec.getLength() - pNextChain->GetWidth()*.5f-fOrgHalfWidth;
    f32 fRotation = (f32)tmpVec.getRadAngleTrig();

    QcPoint RopeBegPos(fOrgHalfWidth, 0);
    RopeBegPos.rotateByRad(fRotation);
    //RopeBegPos += OrgPos;
    RopeBegPos = OrgPos;
	RopeBegPos.X += fOrgHalfWidth * cos(fRotation);
	RopeBegPos.Y += fOrgHalfWidth * sin(fRotation);
    QiAnimator *pAni = QmAnimatorCreator->MakeStretchAnimator(RopeBegPos);
    pNextChain->m_pChainRope->SetVisible(true);
    pNextChain->m_pChainRope->SetPosition(RopeBegPos);
    pNextChain->m_pChainRope->AddAnimator(pAni);
    pNextChain->m_pChainRope->SetScale(0);

    QcPoint RopeMovePos(fLen*0.5f, 0);
    RopeMovePos.rotateByRad(fRotation);
	RopeMovePos = TarPos;

	RopeMovePos.X -= pNextChain->GetWidth()*.5f * cos(fRotation);
	RopeMovePos.Y -= pNextChain->GetWidth()*.5f * sin(fRotation);
	QcPoint end((RopeBegPos.X+TarPos.X)*0.5f,(RopeBegPos.Y+TarPos.Y)*0.5f);
	 pAni = QmAnimatorCreator->MakeLineAnimator(RopeBegPos, end, 0.3f);
   // pAni = QmAnimatorCreator->MakeLineAnimator(QcPoint(), RopeMovePos, 1.0f);
    pAni->AddFinishCallBack(ANI_CALLBACK, 1);
    pNextChain->m_pChainRope->AddAnimator(pAni);
    pNextChain->m_pChainRope->SetRotation(fRotation);

    tmpVec.normalize();
    pAni = QmAnimatorCreator->MakeLineAnimator(RopeBegPos, TarPos-tmpVec*pNextChain->GetWidth()*.5f, 0.3f);
    pNextChain->m_pChainIdBg->AddAnimator(pAni);
    pNextChain->m_pChainIdBg->SetVisible(true);
    pNextChain->m_pChainIdBg->SetRotation(pNextChain->GetRotation());
    pNextChain->m_pChainIdBg->SetPosition(RopeBegPos);

    pNextChain->m_pChainShell->SetVisible(false);

    if (m_arChainList.empty())
    {
        m_pChainIdBg->SetVisible(false);
        m_oChainFishIdDigital.SetVisible(false);
    }
    else
    {
        m_arChainList.back()->m_pChainIdBg->SetVisible(false);
        m_arChainList.back()->m_oChainFishIdDigital.SetVisible(false);
    }

    m_arChainList.push_back(pNextChain);

    pNextChain->m_oChainFishIdDigital.SetDigitalType(QmObjName(ChainFishIdDigital));
    pNextChain->m_oChainFishIdDigital.SetRotation(m_pOwner->GetRotation());
    pNextChain->m_oChainFishIdDigital.SetValue(m_arChainList.size()+1);


    QmPlaySound(NextChain);
}

void DcChainLinkFish::MakeStartDragAni()
{
    QiAnimator  *pAni = QmAnimatorCreator->MakeScaleAnimator2(2.0f, 1.0f, 0.5f);
    //pAni->AddFinishCallBack(ANI_CALLBACK, 3);
    QiAnimatorDeque *pAniDeq = QmAnimatorCreator->MakeAnimatorDeque();
    pAniDeq->AddAnimator(pAni);
    pAni = QmAnimatorCreator->MakeCircleAnimator(QcPoint(-10, 0), TWO_PI, true, false); 
    pAniDeq->AddAnimator(pAni);
    m_pChainShell->SetScale(2.0f);
    m_pChainShell->AddAnimator(pAniDeq);
    pAni->SetStartTime(1.0f);
    m_pFish->AddAnimator(pAni);

    pAni = QmAnimatorCreator->MakeRotationAnimator(TWO_PI);
    m_pFish->AddAnimator(pAni);

    m_pChainShell->SetVisible(true);
    m_pChainEff = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(ChainEff));
    pAni = m_pChainEff->FindAnimator<QcAnimatorTexture>();
    pAni->AddFinishCallBack(ANI_CALLBACK, 4);
}

u32 DcChainLinkFish::GetExplodeScore() const
{
    return GetScore() + m_iChainScore;
}

//int DcChainLinkFish::g_iChainLinkCount = 0;

