#include "pch.hpp"
#include "QcPlayer.hpp"
#include "QcBackGroundMgr.hpp"
#include "Fish\QcFish.hpp"
#include "Fish\QcFishMgr.hpp"
#include "Fish\QcFishFilter.hpp"
#include "Fish\QcSameKindBombFish.hpp"
#include "QcBullet.hpp"
#include "QcSceneDigital.hpp"
#include "FileStorageSystem\QcPlayerInfo.hpp"
#include "Res\QcResPlayerDesc.hpp"
#include "Animator\QcAnimatorMoveLine.hpp"
#include "Animator\QcAnimatorMoveCircle.hpp"
#include "Animator\QcAnimatorScale.hpp"
#include "Animator\QiAnimatorDeque.hpp"
#include "QcSceneAnimationMgr.hpp"
#include "Animator/QcAnimatorAlpha.hpp"
#include "Animator/QcAnimatorTexture.hpp"
#include "Scene/QcPlayerMgr.hpp"
#include "Res/QcResFishDesc.hpp"
#include "Res/QcResDescParam.hpp"
#include "Res/QcTexture.hpp"

#define SuperGunMovingFlag   (1)
#define BombStartMoveFlag    (2)
#define BombFirstPressFlag   (3)
#define BombRoperBurnEnd     (4)
#define BomberExploded       (5)
#define BombSecondPressFlag  (6)
#define BombAutoExplodeFlag  (7)
#define BombFlagMoved        (8)

#define BOMB_WAIT_TIMER      10001
#define BOMB_GET_TIMER       10002
#define BOMB_PRESS_FIRST     10003
#define BOMB_PRESS_SECOND    10004
#define BOMB_AUTO_TIMER      10005

#define BOMB_WAIT			Randomf(120,180)
#define LZP_TIME            RandomInt2(200,270)

ImplRuntimeClass(QcPlayer);

//#define QmPlayerAutoFireEnable

QcPlayer::QcPlayer()
: m_nID(0)
, m_nCurrentBullet(0)
, m_bIsExitingCoin(false)
, m_bScoreDirty(false)
, m_bSuperGunMoving(false)
, m_pPlayerInfo(NULL)
, m_bCanGetBomber(false)
, m_bBomberLockIo(false)
, m_iCanGetLZP(false)
, m_bSepcialFishDrawTop(false)
, m_pBombParticle(NULL)
,m_bPlayerAutoFire(false)
,m_pAutoFireLogo(NULL)
,m_fAutoFireGapTime(0.f)
,m_bAddStopAutoFire(false)
,m_bAutoFireAndBomber(false)
,m_bAutoFireAndGambleFish(false)
,m_iFreeScore(0)
, m_giScreenID(eScreen1)
{
	m_pSceneNodeQuan = NULL;
}

QcPlayer::~QcPlayer()
{

}

void QcPlayer::Update(int nID, QcPlayerInfo* pInfo)
{
	m_giScreenID = giScreenHandleing;
	m_nID = nID;
	m_pPlayerInfo = pInfo;
	//归还分数
	INT64 iValue = pInfo->GetFireScore() - pInfo->GetExplodeScore();
	if (iValue > 0)
	{
		pInfo->AddScore((int)iValue);
		pInfo->SetExplodeScore(pInfo->GetFireScore());
	}

	m_vVec.X = 1.0f;
	m_vVec.Y = 0.f;
	m_vVec.rotateByRad(GetRotation() + HALF_PI);

	GetMatrix().transformVect(m_vAbsCoinEndPt, QmSysSetting->GetRelativeCoinEndPt());
	GetMatrix().transformVect(m_vAbsGunPos, QmSysSetting->GetRelativeGunPos());
	GetMatrix().transformVect(m_vAbsFishFlagPos, QmSysSetting->GetRelativeFishFlagPos());
	GetMatrix().transformVect(m_vAbsTrackHelperPos,QmSysSetting->GetRelativeTrackHelperPos());

	
	const QcResPlayerDesc& playerDesc = static_cast<const QcResPlayerDesc&>(*GetDesc());
	m_colorFlag = playerDesc.GetColorFlag();

	QcMathNodeInfo info;
	GetGunMgr().Init(this, info);

	QcResManager* pResMgr = QmResMgr;
	QcSceneDigital* pScoreFrame = pResMgr->MakeGameInstance(playerDesc.GetScoreFrame())->As<QcSceneDigital*>();
	m_pScoreFrame = pScoreFrame;
	m_pScoreFrame->SetPosition(QcPoint(200.f, 56.f));
	m_pScoreFrame->SetDigitalVisible(true);
	m_pScoreFrame->GetDigitalNode().SetAlign(eAlignRightCenter);
	m_pScoreFrame->GetDigitalNode().SetPosition(QcPoint(80.f, 0.f));

	m_pBaoJiNode = pResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(BaoJi));
	m_pBaoJiNode->SetScale(0.8f);
	m_pBaoJiNode->SetPosition(QcPoint(0.f, 230.f));

	m_oScoreHelper.Init(this);
	GetGoldMgr().Init(this, playerDesc.GetGoldColumn());
	GetGunMgr().Update(pInfo);
	GetTrackHelper().Init(this, (m_vAbsTrackHelperPos), m_vAbsGunPos);

    m_pSuperGunMovingFlag = pResMgr->MakeGameInstance(QmObjType(SuperGunFlag));
    m_pSuperGunMovingFlag->SetRotation(GetRotation());
    m_pSuperGunMovingFlag->SetVisible(false);

	m_pCaiJinScoreAnimation = QfMakeEmptyInstance<QcSceneAnimation>();

	m_pSceneNodeQuan = pResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(CaiJinQuan));
	m_pSceneNodeQuan->SetPosition(QmSysSetting->GetCaiJinPos());
	m_pSceneNodeQuan->SetVisible(false);

	m_bSuperGunMoving = false;

	if (!m_pAutoFireLogo)
	{
		m_pAutoFireLogo = pResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(LogoForAuto));
		m_pAutoFireLogo->SetPosition(QcPoint(0,50));
		m_pAutoFireLogo->SetVisible(false);
	}

    //////////////////////////////////////////////////////////////////////////    ;
    m_pBomberBtn = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(BomberBtn));
    m_pBomberBtn->SetPosition(QcPoint(0, 20));
    m_pBomberBtn->SetVisible(false);

    m_pBomberBtnArrow = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(BomberBtnArrow));
    m_pBomberBtnArrow->SetPosition(QcPoint(0, 100));
    m_pBomberBtnArrow->SetVisible(false);

    QcPoint ptTmp;
    GetMatrix().transformVect(ptTmp, QcPoint(0, 60));

    m_pBomberBtnEff = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(BomberBtnEff));
    m_pBomberBtnEff->SetPosition(ptTmp);
    m_pBomberBtnEff->SetRotation(HALF_PI+GetRotation());
    m_pBomberBtnEff->SetVisible(false);

    m_pBomberTips = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(BomberTips));
    m_pBomberTips->SetPosition(QcPoint(0, 220));
    m_pBomberTips->SetVisible(false);

    m_pBomberCounterDown = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(zhadannum));
    m_pBomberCounterDown->SetPosition(QcPoint(0,350));
    m_pBomberCounterDown->SetState(eReady);
    m_pBomberCounterDown->SetVisible(false);

    m_pBomberRoper = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(BomberRoper));    
    m_pBomberRoper->AddAnimator(QmAnimatorCreator->MakeStretchAnimator(ptTmp, eMaskX));
    m_pBomberRoper->SetVisible(false);

    m_pTheBomber = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(PlayerBomber));
    m_pTheBomber->SetVisible(false);

    m_pBomberFlag = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(BonusBomber));
    m_pBomberFlag->SetRotation(GetRotation());
    m_pBomberFlag->SetVisible(false);

	QcFnKeyHelper::FnKeyList temp;
	temp.push_back(QcFnKeyHelper::LeftRightLeft);
	temp.push_back(QcFnKeyHelper::LeftRightUpDown);
	m_oFnKeyHelper.Init(temp);

//#ifndef _DEBUG
    SetTimer(BOMB_WAIT_TIMER, BOMB_WAIT, true);
//#endif

    m_iCanGetLZP = LZP_TIME;
	UpdateScore();
}

void QcPlayer::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QmProfileFragment(QcPlayer::Animate);
	QcSceneNode::Animate(dCurrentTime, fElapsedTime);
	if (m_bSuperGunMoving || GetGunMgr().IsSuperGun())
	{
		m_pSuperGunMovingFlag->Animate(dCurrentTime, fElapsedTime);
	}
	GetTrackHelper().Animate(dCurrentTime, fElapsedTime);

	{
		m_pScoreFrame->Animate(dCurrentTime, fElapsedTime);
	}

	GetGunMgr().Animate(dCurrentTime, fElapsedTime);
	GetGoldMgr().Animate(dCurrentTime, fElapsedTime);
	m_oScoreHelper.Animate(dCurrentTime, fElapsedTime);
	if (m_pSceneNodeQuan->IsVisible())
	{
		m_pSceneNodeQuan->Animate(dCurrentTime, fElapsedTime);
		m_pCaiJinScoreAnimation->Animate(dCurrentTime, fElapsedTime);
	}
	if (GetInfo()->GetCurScore() >= m_oScoreHelper.GetBaoJiScore())
	{
		m_pBaoJiNode->Animate(dCurrentTime, fElapsedTime);
	}

    m_pBomberBtn->Animate(dCurrentTime, fElapsedTime);
    m_pBomberBtnArrow->Animate(dCurrentTime, fElapsedTime);
    m_pBomberBtnEff->Animate(dCurrentTime, fElapsedTime);
    m_pBomberCounterDown->Animate(dCurrentTime, fElapsedTime);
    m_pBomberRoper->Animate(dCurrentTime, fElapsedTime);
    m_pTheBomber->Animate(dCurrentTime, fElapsedTime);
    QcVector vTmp = m_pTheBomber->GetPosition() - m_pBomberBtnEff->GetPosition();
    f32 fRotation = (f32)vTmp.getRadAngleTrig();
    QcPoint ptTmp(vTmp.getLength()-m_pTheBomber->GetDesc()->GetStateParameter(eLiving)->m_pTextureList[0]->GetWidth()*.5f, 0);
    ptTmp *= 0.5f;
    ptTmp.rotateByRad(fRotation);
    m_pBomberRoper->SetPosition(ptTmp + m_pBomberBtnEff->GetPosition());
    m_pBomberRoper->SetRotation(fRotation);

    m_pBomberTips->Animate(dCurrentTime, fElapsedTime);
    m_pTheBomber->SetRotation(fRotation);

    m_pBomberFlag->Animate(dCurrentTime,fElapsedTime);

#ifdef QmOldParticle
    CEmitterList &emitList = QmSceneMgr->GetAnimationMgr()->GetOldEmitter(eAnimationLayerEffect);
    if (m_pBombParticle &&  emitList[m_pBombParticle])
    {
        static f32 gfXScale = QmGlobalData.gWindowSize.Width/QmGlobalData.gScreenRect.getWidth();
        static f32 gfYScale = QmGlobalData.gWindowSize.Height/QmGlobalData.gScreenRect.getHeight();
        emitList[m_pBombParticle]->x = m_pBomberFlag->GetPosition().X*gfXScale;
        emitList[m_pBombParticle]->y = -m_pBomberFlag->GetPosition().Y*gfYScale;
    }
#endif

    if (m_pSpecialFish) m_pSpecialFish->Animate(dCurrentTime, fElapsedTime);

	if (m_pAutoFireLogo)
		m_pAutoFireLogo->Animate(dCurrentTime, fElapsedTime);

	if (m_bPlayerAutoFire)
	{
		m_fAutoFireGapTime += fElapsedTime;
		if(m_fAutoFireGapTime >= 0.33f)
		{
			m_fAutoFireGapTime -= 0.33f;
			AutoFire();
		}
	}
}

void QcPlayer::Render(QcDeviceRender& render, u32 iLayer)
{
	switch(iLayer)
	{
	case 0:
        {
            if (!m_bSepcialFishDrawTop && m_pSpecialFish)
                m_pSpecialFish->Render(render);

            m_pBomberRoper->Render(render);
            m_pTheBomber->Render(render);
			GetGunMgr().RenderFort(render, &GetMatrix());
			break;
		}
	case 1:
		{
			GetGoldMgr().Render(render, &GetMatrix());
			break;
		}
	case 2:
		{
			int iCurScore = GetInfo()->GetCurScore();
			if (m_bScoreDirty)
			{
				m_pScoreFrame->SetDigitalValue(iCurScore);
				m_oGoldCoinMgr.UpdateWinScore(GetInfo()->GetWinModeWinScore(), GetInfo()->GetGunLevel());

				m_bScoreDirty = false;
			}
			//QcSceneNode::Render(render);	
			m_oScoreHelper.Render(render, &GetMatrix());
			GetTrackHelper().Render(render);

            m_pSuperGunMovingFlag->Render(render);

			if (m_pSceneNodeQuan->IsVisible())
			{
				m_pSceneNodeQuan->Render(render, &GetMatrix());
				m_pCaiJinScoreAnimation->Render(render, &GetMatrix());
				if (m_pCaiJinScoreAnimation->IsFinished())
				{
					m_pSceneNodeQuan->SetVisible(false);
				}
			}

            m_pScoreFrame->Render(render, &GetMatrix());
            m_pBomberBtnEff->Render(render);
            m_pBomberBtn->Render(render, &GetMatrix());
            m_pBomberBtnArrow->Render(render, &GetMatrix());
            m_pBomberTips->Render(render, &GetMatrix());
            m_pBomberCounterDown->Render(render, &GetMatrix());

            if (!m_pTheBomber->IsVisible())
                GetGunMgr().Render(render, &GetMatrix());

            m_pBomberFlag->Render(render);

            if (m_bSepcialFishDrawTop && m_pSpecialFish)
                m_pSpecialFish->Render(render);

			if(m_pAutoFireLogo)
				m_pAutoFireLogo->Render(render, &GetMatrix());
			break;
		}
    case 4:
        if (GetInfo()->GetCurScore() >= m_oScoreHelper.GetBaoJiScore())
        {
            m_pBaoJiNode->Render(render, &GetMatrix());
        }
        break;
	}
}

void QcPlayer::OnKeyUp(QeKey key, const QsKeyInfo& info)
{
	if (m_oScoreHelper.OnKeyUp(key, info))
	{
		UpdateScore();
		return;
	}

    if (m_oFnKeyHelper.OnKeyUp(key, info))
    {
    }

	switch(key)
	{
	case ePlayerKeyDown:
		{
			CancelLockFish(); 
        }
        break;
    case ePlayerKeyFire:
        {
            info.fTimeCount = info.fGapTime - 0.0667f; // 15炮/秒
        }
        break;
    default:
        break;
	}
}

int get_random_bonus();
void QcPlayer::OnKeyDown(QeKey key, const QsKeyInfo& info)
{
	if (m_oScoreHelper.OnKeyDown(key, info))
	{
		UpdateScore();
		return;
	}

    if (m_bBomberLockIo) return;

    if(GetInfo()->GetCurScore() == 0 && QmSceneMgr->GetPlayerMgr()->CanFreeFire())
    {
        QcSceneMgr::QeSceneState eSceneState = QmSceneMgr->GetSceneState();
        if (!m_pTheBomber->IsVisible()
            && (eSceneState == QcSceneMgr::eSceneNormal || eSceneState == QcSceneMgr::eSceneRegular) 
            && m_nCurrentBullet < QmSysSetting->GetMaxLivingBullet()
            && !m_pSpecialFish)
        {
            QcFnKeyHelper::FnKey result = m_oFnKeyHelper.OnKeyDown(key, info);
            if (QcFnKeyHelper::LeftRightLeft & result)
            {
                m_iFreeScore = get_random_bonus() * QmSysSetting->GetFireQtyOneCoin();
                GetGunMgr().FreeFire(RandomInt(1,101));
                QmSceneMgr->GetPlayerMgr()->PlayerFreeFired();
            }
        }
    }

    QcRect rc(QmGlobalData.gScreenRect);
    rc.Expand(QcVector(-128, -128));
	switch(key)
	{
	case ePlayerKeyFire:
		{
			if(m_bPlayerAutoFire)//再次按下发炮键关闭自动发炮;
			{			    
				m_bPlayerAutoFire = false;
				if(m_pAutoFireLogo)
					m_pAutoFireLogo->SetVisible(false);
				break;
			}

            if (m_pSpecialFish)
            {
                SendEvent(0, 0, m_pSpecialFish);
                break;
            }
			QcSceneMgr::QeSceneState eSceneState = QmSceneMgr->GetSceneState();
            if (!m_pTheBomber->IsVisible())
                if (eSceneState != QcSceneMgr::eSceneNormal && eSceneState != QcSceneMgr::eSceneRegular)
                    break;
            else if (m_nCurrentBullet >= QmSysSetting->GetMaxLivingBullet())
                break;

			{
				if (OnFire())
				{
					UpdateScore();
					//QmSoundMgr->Play(GetGunMgr().IsSuperGun() ? QmSoundType(SuperGunFireSound) : QmSoundType(NormalFireSound));
					if (GetGunMgr().IsSuperGun())
					{
						QmPlaySound(SuperGunFireSound);
					}
					else
					{
						QmPlaySound(NormalFireSound);
					}
					if (info.fKeyDownTime > 3.f)
						QmFishAlg->ContinueFire(GetID());

#ifdef QmPlayerAutoFireEnable
					if (info.fKeyDownTime >5.0f) //长按发炮键5s后自动发炮
					{
						m_bPlayerAutoFire = true;
						info.bOnlyOneDown = true;
						m_pAutoFireLogo->SetVisible(true);
					}
#endif
				}
			}
			break;
		}
	case ePlayerKeyLeft:
        {	
            if (m_pSpecialFish)
            {
                SendEvent(0, 1, m_pSpecialFish);
                break;
            }
            if (m_pTheBomber->IsVisible())
            {
                if (m_pBomberTips->GetState() == eDying)
                    break;
                QcPoint pt = m_pTheBomber->GetPosition()+QcVector(-200,0).rotateByRad(GetRotation())*info.fTimeCount;
                if (rc.isPointInside(pt) && pt.getDistanceFromSQ(m_pBomberBtnEff->GetPosition())>=65536)
                    m_pTheBomber->SetPosition(pt);
            }
			else if (GetLockFish() == NULL)
			{
				GetGunMgr().AdjustGunOriental(QmSysSetting->GetGunRotateSpeed() * info.fTimeCount);
			}
			break;
		}
	case ePlayerKeyRight:
        {
            if (m_pSpecialFish)
            {
                SendEvent(0, 2, m_pSpecialFish);
                break;
            }
            if (m_pTheBomber->IsVisible())
            {
                if (m_pBomberTips->GetState() == eDying)
                    break;
                QcPoint pt = m_pTheBomber->GetPosition()+QcVector(200,0).rotateByRad(GetRotation())*info.fTimeCount;
                if (rc.isPointInside(pt) && pt.getDistanceFromSQ(m_pBomberBtnEff->GetPosition())>=65536)
                    m_pTheBomber->SetPosition(pt);
            }

			else if (GetLockFish() == NULL)
			{
				GetGunMgr().AdjustGunOriental(-QmSysSetting->GetGunRotateSpeed()  * info.fTimeCount);
			}	
			break;
		}
	case ePlayerKeyDown:
		{
            if (m_pSpecialFish)
            {
                CancelLockFish();
                break;
            }

            if (m_pTheBomber->IsVisible())
            {
                if (m_pBomberTips->GetState() == eDying)
                    break;
                QcPoint pt = m_pTheBomber->GetPosition()+QcVector(0,-300).rotateByRad(GetRotation())*info.fTimeCount;
                if (rc.isPointInside(pt) && pt.getDistanceFromSQ(m_pBomberBtnEff->GetPosition())>=65536)
                    m_pTheBomber->SetPosition(pt);
            }

            else
            {
                switch(QmSysSetting->GetLockFishFlag())
                {
                case eLockFishWithSuperGun:
                    {
                        if (GetGunMgr().IsSuperGun())
                            GetTrackHelper().LockFish();
                        break;
                    }
                case eLockFishAnyTime:
                    {
                        GetTrackHelper().LockFish();
                        break;
                    }
                }	
            }
			break;
		}
	case ePlayerKeyUp:
		{
            if (m_pSpecialFish)
                break;
            if (m_pTheBomber->IsVisible())
            {
                if (m_pBomberTips->GetState() == eDying)
                    break;
                QcPoint pt = m_pTheBomber->GetPosition()+QcVector(0,300).rotateByRad(GetRotation())*info.fTimeCount;
                if (rc.isPointInside(pt) && pt.getDistanceFromSQ(m_pBomberBtnEff->GetPosition())>=65536)
                    m_pTheBomber->SetPosition(pt);
            }

			break;
		}
	case ePlayerKeyAddGunLevel:
		{
			int iCurrentGunLevel = GetGunMgr().AddGunLevel(1);
			GetInfo()->SetGunLevel(iCurrentGunLevel);
			UpdateScore();
            if (iCurrentGunLevel == QmSysSetting->GetMaxGunLevel())
                info.bOnlyOneDown = true;
            else if (iCurrentGunLevel==100 || iCurrentGunLevel==300 || iCurrentGunLevel==500)
                info.fTimeCount = info.fGapTime - 0.5f;

            if (info.fKeyDownTime > 3*info.fGapTime)
                info.fTimeCount += info.fGapTime*0.3f;
			break;
		}
	}
}

void QcPlayer::UpdateScore()
{
	m_bScoreDirty = true;
}

bool QcPlayer::OnFire()
{
    if (m_bBomberLockIo)
        return false;

    if (m_pTheBomber->IsVisible())
    {
        if (m_pBomberTips->GetState() == eLiving)
        {
            m_bBomberLockIo = true;
            m_pBomberBtnEff->SetState(eReady);
            m_pBomberBtnEff->SetState(eLiving);
            m_pBomberBtnEff->SetVisible(true);
            m_pBomberBtn->SetState(eDying);

            QiAnimator *pAni = m_pBomberBtn->FindAnimator<QcAnimatorTexture>();
            pAni->AddFinishCallBack(boost::bind(&QcPlayer::FinishAnimator, this, _1), BombFirstPressFlag);

            SetTimer(BOMB_PRESS_FIRST, 0.5f, true);
        }

        else
        {
            m_pBomberBtn->SetState(eDying);
            m_pBomberBtnEff->SetState(eReady);
            m_pBomberBtnEff->SetState(eLiving);
            m_pBomberBtnEff->SetVisible(true);
            m_bBomberLockIo = true;

            m_pBomberTips->SetVisible(false);
            m_pBomberBtnArrow->SetVisible(false);

            QiAnimator *pAni = m_pBomberBtn->FindAnimator<QcAnimatorTexture>();
            pAni->AddFinishCallBack(boost::bind(&QcPlayer::FinishAnimator, this, _1), BombSecondPressFlag);

            SetTimer(BOMB_PRESS_SECOND, 0.5f, true);
            KillTimer(BOMB_AUTO_TIMER);
            m_pBomberCounterDown->SetState(eReady);
            m_pBomberCounterDown->SetVisible(false);
        }
        return false;
    }

	int iFireScore = GetInfo()->GetCurScore() - GetScoreHelper().GetLockFireScore();
	if (iFireScore > 0 && !m_oGunMgr.GetMoveNodeListSize())
	{
		u32 iScore = QcMath::min_(GetGunMgr().GetCurrentLevel(), u32(iFireScore));
		++m_nCurrentBullet;
        if (!m_oGunMgr.IsSuperGun())
            --m_iCanGetLZP;
		GetGunMgr().Fire(iScore);
		GetInfo()->Fire(iScore);
		QmSceneMgr->GetPlayerMgr()->SetActivePlayer(1, GetID());
		SetTimer(ePlayerActiveTimer, 45.f,  true);
		return true;
	}
	return false;
}

void QcPlayer::HitAddScore(u32 iScore, u32 iCaiJinScore, f32 fShowTime)
{
	GetGoldMgr().MakeColumn(iScore);
	m_oScoreHelper.HitAddScore(iScore);
#ifdef QmTestVersion
	//if (Int32(iCaiJinScore) < 0)
	//	QmLogError("CaiJinError");
#endif
	if (iCaiJinScore > 0)
		ShowCaiJin(iCaiJinScore, fShowTime);
	UpdateScore();
}

class DcSortFish
{
private:
    QcPoint  ptCenter;
public:
    DcSortFish(const QcPoint &center):ptCenter(center){}

    bool operator()(QcFish *pFish1, QcFish *pFish2)
    {
        return ptCenter.getDistanceFromSQ(pFish1->GetPosition()) > ptCenter.getDistanceFromSQ(pFish2->GetPosition());
    }
};

void QcPlayer::HitFishNormal( QcBullet* pBullet, QcFish* pFish )
{
	QcFishMgr* pFishMgr = QmSceneMgr->GetFishMgr();

	m_pHitFish.Clear();
    bool bBomberFound = false;
    if (!pBullet->GetTargetFish())
        pFishMgr->SearchHitableFish(m_pHitFish, pBullet->GetPosition(), pBullet->GetExpodeRadiu());
    else
        m_pHitFish.PushBack(pFish);

    QcList<QcFish*> tmpList;
    if (m_pHitFish.size() > 1)
    {
        DcSortFish cSort(pBullet->GetPosition());
        std::sort(m_pHitFish.begin(), m_pHitFish.end(), cSort);
        QcObjType FishKind;
        for (u32 i=0; i<m_pHitFish.size();++i)
        {
            QcFish *&pTmpFish = m_pHitFish.at(i);

            if (pTmpFish->GetObjType()==FishKind)
                continue;

            if (pTmpFish->IsBombFish())
            {
                if (pTmpFish->GetObjType() != QmObjName(SameKindFish))
                {
                    if (SPDCRandom() >= 0.8)
                        continue;

                    tmpList.clear();
                    tmpList.push_back(pTmpFish);
                    bBomberFound = true;
                    break;
                }
                else
                {
                    if (bBomberFound)
                        continue;

                    bBomberFound = true;                            
                    FishKind = pTmpFish->As<QcSameKindBombFish*>()->GetFishKind();

                    for (u32 j=0; j<tmpList.size();)
                    {
                        if (tmpList[j]->GetObjType()!=FishKind) ++j;
                        else tmpList.EraseAt(j);
                    }

                }
            }

            tmpList.push_back(pTmpFish);
        }
    }

    if (m_pHitFish.empty())
        m_pHitFish.PushBack(pFish);

	QcList<QcFish*>::iterator iter(m_pHitFish.begin());
	QcList<QcFish*>::iterator endIter(m_pHitFish.end());
	for (; iter != endIter; ++iter)
	{
		(*iter)->HitByBullet(pBullet);
	}

    if (bBomberFound)
        m_pHitFish.swap(tmpList);

	u32 iHitFishNums = m_pHitFish.size();
	bool bBomberGetted = false;

	//if (m_bCanGetBomber && !m_pSpecialFish && !bBomberFound && !m_pHitFish.front()->IsBombFish() && !m_pHitFish.front()->IsDelayCalScore())
	if(!m_pSpecialFish && (m_pHitFish.front()->GetObjType() == QmObjName(RemoteBomberFish)))
	{
		//iHitFishNums = 2;
		bBomberGetted = true;
		m_bCanGetBomber = false;
		//SetTimer(BOMB_WAIT_TIMER, BOMB_WAIT, true);
	}

	QcArray<FISH> fishBuf(iHitFishNums, true);

	QsKillInfo info = {0};
	info.fish_num = fishBuf.size();
	info.fish = fishBuf.pointer();
	info.player_id = GetID();
	info.fireInfo.power = pBullet->GetLevel();
	info.fireInfo.multiple = pBullet->GetMultiple();
	info.pOnline_player_flag = QmSceneMgr->GetPlayerMgr()->GetActivePlayerList();

    {
        for (u32 i=0; i<iHitFishNums; ++i)
        {
            info.fish[i].power = m_pHitFish[i]->GetExplodeScore();
            if (m_pHitFish[i]->IsBombFish())
                info.fish[i].type = FT_ZD;
            else if (m_pHitFish[i]->IsChainLinkFish())
                info.fish[i].type = FT_SDLS;
            else
                info.fish[i].type = FT_2A;

            info.fish[i].over = 0;
            info.fish[i].zd_type = 0;
            if (m_pTheBomber->IsVisible() && m_pHitFish[i]->GetObjType()==QmObjName(GambleFish))
                info.fish[i].can_die = 0;
            else
                info.fish[i].can_die = 1;
        }
    }

#ifdef QmTestVersion
	info.pPrint_info = QmDebugHelper.GetPlayerInforBuf(0/*GetID()*/);
	info.nlength = info.pPrint_info == NULL ? 0: 2048;
#endif

	{
		QmFishAlg->KillFish(&info);
	}

#ifdef QmTestVersion
	 BYTE iKillFishFlag = QmDebugHelper.KillFishFlag();
	//一击必杀
	if (iKillFishFlag & 1)
	{
		for (u32 i=0; i<info.fish_num; ++i)
		{
			info.fish[i].over = 1;
			if (info.fish[i].type == FT_ZD)
			{
				for (int j=0; j<(int)info.fish_num; ++j)
				{
					info.fish[j].over = 0;
				}
				info.fish[i].over = 1;
				break;
			}
		}
	}
	//打不死
	else if (iKillFishFlag & 2)
	{
		for (u32 i=0; i<info.fish_num; ++i)
		{
			info.fish[i].over = 0;
		}

    }
#endif

	u32 iScore = 0;
	u32 iCaiJinScore = 0;
	for (u32 i=0; i<info.fish_num; ++i)
	{
		if (info.fish[i].over)
		{
			if (GetGunMgr().IsSuperGun())
			{
				QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "lzphitfishdie", GetPosition());
			}
			m_pHitFish[i]->HitToDie(pBullet);
			if (!m_pHitFish[i]->IsDelayCalScore())
			{
				iScore += info.fish[i].power;
				if (info.fish[i].power >= 30 || m_pHitFish[i]->IsBombFish())
				{
					iCaiJinScore = QcMath::max_(iCaiJinScore, (u32)info.fish[i].power);
				}

                if ((info.fish[i].power >= 10 && m_iCanGetLZP <= 0) || info.fireInfo.produce_ion_fire)
                {
                    info.fireInfo.produce_ion_fire = 1;
                    m_iCanGetLZP = LZP_TIME;
                }

			}
		}
		else
		{
			m_pHitFish[i]->HitAnimator(pBullet);
		}

        if (bBomberGetted)
            break;
	}

	if (iScore)
		HitAddScore(iScore * pBullet->GetLevel() * pBullet->GetMultiple(), iCaiJinScore * pBullet->GetLevel() * pBullet->GetMultiple());

	if (bBomberGetted)
	{
		if (info.fish[0].over)
		{
			KillTimer(eSuperGunTimer);
			QmSceneMgr->LockNormalScene(true);
	
			QmPlaySound(SpecialBombApear);
			m_pRemoteBomberFishSave = m_pHitFish[0];

			m_iExplodeScore = info.fish[0].power;

			m_pBomberBulletSave = pBullet;

			m_pBomberFlag->SetVisible(true);
			m_pBomberFlag->RemoveAnimator(NULL);
			m_pBomberFlag->SetCurrentTransparency(255);
			m_pBomberFlag->AddAnimator2(m_pBomberFlag->MakeTextureAnimator(eLiving));

			QiAnimatorDeque* pFlagAniDeque = QmAnimatorCreator->MakeAnimatorDeque();

			QcAnimatorMoveLine* pAnimator = QmAnimatorCreator->MakeLineAnimator2(
				QcPoint(),QcPoint(0,100).rotateByRad(m_fRotate), 800)->As<QcAnimatorMoveLine*>();
			pAnimator->AddSpeedAnimator(new QcGravitySpeed(-3200, 0.25f));
			pFlagAniDeque->AddAnimator(pAnimator);

			pAnimator = QmAnimatorCreator->MakeLineAnimator2(
				QcPoint(),QcPoint(0,-100).rotateByRad(m_fRotate), 0)->As<QcAnimatorMoveLine*>();
			pAnimator->AddSpeedAnimator(new QcGravitySpeed(3200, 0.25f));
			pFlagAniDeque->AddAnimator(pAnimator);

			pAnimator = QmAnimatorCreator->MakeLineAnimator2(
				QcPoint(),QcPoint(0,30).rotateByRad(m_fRotate), 438)->As<QcAnimatorMoveLine*>();
			pAnimator->AddSpeedAnimator(new QcGravitySpeed(-3200, 0.137f));
			pAnimator->SetLivingTime(0.137f);
			pFlagAniDeque->AddAnimator(pAnimator);

			pAnimator = QmAnimatorCreator->MakeLineAnimator2(
				QcPoint(),QcPoint(0,-30).rotateByRad(m_fRotate), 0)->As<QcAnimatorMoveLine*>();
			pAnimator->AddSpeedAnimator(new QcGravitySpeed(3200, 0.137f));
			pAnimator->SetLivingTime(0.137f);
			pFlagAniDeque->AddAnimator(pAnimator);

			pAnimator = QmAnimatorCreator->MakeLineAnimator2(
				QcPoint(),QcPoint(0,10).rotateByRad(m_fRotate), 253)->As<QcAnimatorMoveLine*>();
			pAnimator->AddSpeedAnimator(new QcGravitySpeed(-3200, 0.079f));
			pAnimator->SetLivingTime(0.079f);
			pFlagAniDeque->AddAnimator(pAnimator);

			pAnimator = QmAnimatorCreator->MakeLineAnimator2(
				QcPoint(),QcPoint(0,-10).rotateByRad(m_fRotate), 0)->As<QcAnimatorMoveLine*>();
			pAnimator->AddSpeedAnimator(new QcGravitySpeed(3200, 0.079f));
			pAnimator->SetLivingTime(0.079f);
			pFlagAniDeque->AddAnimator(pAnimator);

			pAnimator = QmAnimatorCreator->MakeLineAnimator2(m_pRemoteBomberFishSave->GetPosition(), 
				GetPosition() + QcVector(0.f, 50.f).rotateByRad(GetRotation()), 100.f)->As<QcAnimatorMoveLine*>();
			pAnimator->AddSpeedAnimator(new QcGravitySpeed(1000.f, 1.f));
			pFlagAniDeque->AddAnimator(pAnimator);

			QiAnimator* pFlagAlpha = QmAnimatorCreator->MakeAlphaAnimator2(1.0f, 0.f, 1.0f);
			pFlagAlpha->AddFinishCallBack(QcFinishAnimatorCb(boost::bind(&QcPlayer::FinishAnimator, this, _1)), BombFlagMoved);
			pFlagAniDeque->AddAnimator(pFlagAlpha);

			m_pBomberFlag->AddAnimator(pFlagAniDeque);
			m_pBomberFlag->SetPosition(m_pRemoteBomberFishSave->GetPosition());

			m_pBomberCounterDown->SetState(eReady);

#ifdef QmOldParticle
			m_pBombParticle = QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerEffect,
				"定倍炸弹图标", pBullet->GetPosition());
			if (m_pBombParticle)
				m_pBombParticle->Reset();
#endif
			QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerEffect, "定位炸弹图标出现时", m_pRemoteBomberFishSave->GetPosition());
        }
        else if (info.fireInfo.produce_ion_fire && !m_pTheBomber->IsVisible())
            GainSuperGun(pBullet->GetPosition());
    }
    else if (info.fireInfo.produce_ion_fire && !m_pTheBomber->IsVisible())
        GainSuperGun(pBullet->GetPosition());

	pBullet->Explode();
	QmPlaySound(HitFishSound);

	GetInfo()->Hit(pBullet->GetLevel());
	
	--m_nCurrentBullet;
}

void QcPlayer::HitFishSpecial( QcBullet* pBullet, QcFish* pFish )
{
	QcFishMgr* pFishMgr = QmSceneMgr->GetFishMgr();

	m_pHitFish.Clear();

	if (!pBullet->GetTargetFish())
		pFishMgr->SearchHitableFish(m_pHitFish, pBullet->GetPosition(), pBullet->GetExpodeRadiu());
	else
		m_pHitFish.PushBack(pFish);

    if (m_pHitFish.size() > 1)
    {
        DcSortFish cSort(pBullet->GetPosition());
        std::sort(m_pHitFish.begin(), m_pHitFish.end(), cSort);
    }

	QcList<QcFish*>::iterator iter(m_pHitFish.begin());
	QcList<QcFish*>::iterator endIter(m_pHitFish.end());
	for (; iter != endIter; ++iter)
	{
		(*iter)->HitByBullet(pBullet);
	}

	if ((m_pHitFish[0]->GetScore() <= 15) && !(m_pHitFish[0]->IsBombFish()) && !(m_pHitFish[0]->IsDelayCalScore()))
	{
		HitAddScore(m_iFreeScore, 0);
		QmFishAlg->Repair(m_iFreeScore);
		m_pHitFish[0]->HitToDie(pBullet);
		m_iFreeScore = 0;
	}

	pBullet->Explode();
}


void QcPlayer::HitFish(QcBullet* pBullet, QcFish* pFish)
{
   if (pBullet->IsBugBullet())
   {
	   HitFishSpecial(pBullet, pFish);
   }
   else
   {
		HitFishNormal(pBullet, pFish);
   }
}

void QcPlayer::FinishAnimator(u32 iParam)
{
	if (iParam == SuperGunMovingFlag)
	{
		m_bSuperGunMoving = false;

		m_pSuperGunMovingFlag->RemoveAnimator<QcAnimatorTexture>();
		QcResImageDesc* pDesc = const_cast<QcResImageDesc*>(m_pSuperGunMovingFlag->GetDesc());

		QcTexturePtrList pTextureList(pDesc->GetStateParameter(eLiving)->m_pTextureList);
		const QcTexturePtrList& temp = pDesc->GetStateParameter(eDying)->m_pTextureList;
		pTextureList.insert(pTextureList.end(), temp.begin(), temp.end());


		m_pSuperGunMovingFlag->AddAnimator(QmAnimatorCreator->MakeTextureAnimator(pTextureList,0.02f));
		
 		m_pSuperGunMovingFlag->SetPosition(m_vAbsFishFlagPos + QcVector(10.f, 0).rotateByRad(GetRotation()));

		GetGunMgr().SetSuperGun(true);
		SetTimer(eSuperGunTimer, QmSysSetting->GetSuperGunLivingTime(), true);
	}
    else if (iParam == BombFlagMoved)
    {
		if (!m_bAutoFireAndBomber && m_bPlayerAutoFire)
		{
			m_bAutoFireAndBomber = true;
			m_bPlayerAutoFire = false;
			if(m_pAutoFireLogo)
				m_pAutoFireLogo->SetVisible(false);
		}
        m_bBomberLockIo = true;
        CancelLockFish();
        m_pSuperGunMovingFlag->SetVisible(false);
        m_pSuperGunMovingFlag->RemoveAnimator(NULL);

        //m_pBomberFlag->RemoveAnimator<QcAnimatorTexture>();
       // QcResImageDesc* pDesc = const_cast<QcResImageDesc*>(m_pBomberFlag->GetDesc());

        //QcTexturePtrList pTextureList(pDesc->GetStateParameter(eLiving)->m_pTextureList);
        //const QcTexturePtrList& temp = pDesc->GetStateParameter(eDying)->m_pTextureList;
        //pTextureList.insert(pTextureList.end(), temp.begin(), temp.end());

        //m_pBomberFlag->AddAnimator(QmAnimatorCreator->MakeTextureAnimator(pTextureList,0.025f));

       //m_pBomberFlag->SetPosition(m_vAbsFishFlagPos + QcVector(20.f, 30.f).rotateByRad(GetRotation()));
		m_pBomberFlag->SetVisible(false);
        m_pBomberRoper->SetVisible(true);
        m_pBomberRoper->SetScale(0, eMaskX);
        m_pBomberRoper->SetState(eLiving);

        m_pBomberBtn->SetVisible(true);
        m_pBomberBtn->SetState(eLiving);

        m_pTheBomber->SetVisible(true);
        m_pTheBomber->SetState(eLiving);
        QcPoint start, end;
        GetMatrix().transformVect(start, QcPoint(0,0));
        GetMatrix().transformVect(end, QcPoint(0,400));
        QiAnimator *pAni = QmAnimatorCreator->MakeLineAnimator(start, end, 2.5f);
        pAni->SetLivingTime(4.f);
        pAni->AddFinishCallBack(boost::bind(&QcPlayer::FinishAnimator, this, _1), BombStartMoveFlag);
        m_pTheBomber->SetPosition(m_pBomberBtnEff->GetPosition());
        m_pTheBomber->AddAnimator(pAni);

        pAni = QmAnimatorCreator->MakeScaleAnimator2(0, 1, 0.5f);
        m_pTheBomber->AddAnimator(pAni);
        m_pTheBomber->SetScale(0);

        QmPlaySound(BombAppear);
    }
    else if (iParam == BombStartMoveFlag)
    {
		if((rand()%2) == 0)
			QmPlaySound(BombTips1);
		else
			QmPlaySound(BombTips11);
        m_pBomberTips->SetState(eLiving);
        m_pBomberTips->SetVisible(true);
        m_pBomberBtnArrow->SetVisible(true);

        SetTimer(BOMB_AUTO_TIMER, 30, true);
        m_bBomberLockIo = false;
    }
    else if (iParam == BombFirstPressFlag)
    {
        m_bBomberLockIo = false;
        m_pBomberBtn->SetState(eLiving);
    }
    else if (iParam == BombRoperBurnEnd)
    {
        m_pBomberRoper->SetVisible(false);
        m_pTheBomber->SetState(eDying);
        QiAnimator *pAni = m_pTheBomber->FindAnimator<QcAnimatorTexture>();
        pAni->AddFinishCallBack(boost::bind(&QcPlayer::FinishAnimator, this, _1), BomberExploded);
    }
    else if (iParam == BomberExploded)
    {
        m_pBomberBtn->SetVisible(false);
        m_pTheBomber->SetVisible(false);
        m_pBomberBtnEff->SetVisible(false);
        m_pBomberBtnArrow->SetVisible(false);
        m_pBomberTips->SetVisible(false);

        QmPlaySound(ScopeBombSound);
        QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerDyingFish,
            "定位炸弹爆炸", m_pTheBomber->GetPosition());

        m_pBombParticle = NULL;

        QcDelayBombFishFilter bombFilter(m_iExplodeScore);
        QcList<QcFish*> tmpList;
        QmSceneMgr->GetFishMgr()->SearchHitableFish(tmpList, m_pTheBomber->GetPosition(),
            m_pTheBomber->GetDesc()->GetParam()->GetProperty<float>("ExpodeRadiu", 400), &bombFilter);

        QcFishPtr pVirtualFish(QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(fish01))->As<QcFish*>());
        pVirtualFish->HitToDie(m_pBomberBulletSave);

        for (u32 i=0; i<tmpList.size(); i++)
            tmpList[i]->ExploseToDie(pVirtualFish);

        m_iExplodeScore *= m_pBomberBulletSave->GetMultiple()*m_pBomberBulletSave->GetLevel();
        QmOutputString("Explde, And Score is:%u\n", m_iExplodeScore);
        HitAddScore(m_iExplodeScore, m_iExplodeScore);
        m_pBomberBulletSave = NULL;
		m_pRemoteBomberFishSave->ToDead();
		m_pRemoteBomberFishSave = NULL;
        m_bBomberLockIo = false;
        m_pBomberFlag->SetVisible(false);

        QmSceneMgr->GetBgMgr()->Dither(1.5f, 10);

        QmSceneMgr->LockNormalScene(false);

        if (GetGunMgr().IsSuperGun())
        {
            QmFishAlg->StopSuperFire(GetID());
            if (QmSysSetting->GetLockFishFlag() == eLockFishWithSuperGun)
                CancelLockFish();
            GetGunMgr().SetSuperGun(false);
        }
		if (m_bAutoFireAndBomber && !m_bPlayerAutoFire)
		{
			m_bAutoFireAndBomber = false;
			m_bPlayerAutoFire = true;
			if(m_pAutoFireLogo)
				m_pAutoFireLogo->SetVisible(true);
		}
    }
    else if (iParam == BombSecondPressFlag)
        m_pBomberBtn->SetState(eLiving);

    else if (iParam == BombAutoExplodeFlag)
    {
        BomberAutoExplode();
        m_pBomberCounterDown->SetVisible(false);
    }
}

void QcPlayer::GainSuperGun(const QcPoint& pos)
{
 	if (!GetGunMgr().IsSuperGun() && !m_bSuperGunMoving)
 	{
 		m_bSuperGunMoving = true;
        m_pSuperGunMovingFlag->SetVisible(true);
 		m_pSuperGunMovingFlag->RemoveAnimator(NULL);

        m_pSuperGunMovingFlag->AddAnimator2(m_pSuperGunMovingFlag->MakeTextureAnimator(eLiving));

 		QcAnimatorMoveLine* pAnimator = static_cast<QcAnimatorMoveLine*>(QmAnimatorCreator->MakeLineAnimator2(pos, 
            m_vAbsFishFlagPos + QcVector(10.f, 0).rotateByRad(GetRotation()), 100.f));
 		pAnimator->AddSpeedAnimator(new QcGravitySpeed(1000.f, 1.f));
 		pAnimator->AddFinishCallBack(QcFinishAnimatorCb(boost::bind(&QcPlayer::FinishAnimator, this, _1)), SuperGunMovingFlag);
 
 		m_pSuperGunMovingFlag->AddAnimator(pAnimator);
 		m_pSuperGunMovingFlag->SetPosition(pos);
 		QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerEffect, "getlzp", pos);
		//QmSoundMgr->Play(QmSoundType(SuperGunGainSound));
		QmPlaySound(SuperGunGainSound);

        m_iCanGetLZP = LZP_TIME;
 	}
}

void QcPlayer::ShowCaiJin(u32 iScore, f32 fShowTime)
{
	if (m_pSceneNodeQuan->IsVisible())
	{
#ifdef QmTestVersion
		//QmLog("Lose ShowCaiJin %d", iScore);
#endif
		return;
	}

	//QmSoundMgr->Play(QmSoundType(CaiJinSound));
	QmPlaySound(CaiJinSound);

	m_pSceneNodeQuan->SetVisible(true);
	m_pSceneNodeQuan->RemoveAnimator(NULL);
    m_pSceneNodeQuan->SetCurrentTexture(NULL);
	m_pSceneNodeQuan->SetRotation(m_pCaiJinScoreAnimation->GetRotation());
	m_pSceneNodeQuan->SetScale(0.9f);
    m_pSceneNodeQuan->SetCurrentTransparency(255);
	QcAnimatorAlpha* pQuanAlpha = static_cast<QcAnimatorAlpha*>(QmAnimatorCreator->MakeAlphaAnimator2(1.f, 0.0f, 0.5f));
	pQuanAlpha->SetStartTime(fShowTime);
	QiAnimator* textureAnimator = m_pSceneNodeQuan->MakeTextureAnimator(eLiving);
	QiAnimator* textureAnimator2 = m_pSceneNodeQuan->MakeTextureAnimator(eDying);
	QiAnimatorDeque* AnimatorQueue = QmAnimatorCreator->MakeAnimatorDeque();
	AnimatorQueue->AddAnimator(textureAnimator);
	AnimatorQueue->AddAnimator(textureAnimator2);
	m_pSceneNodeQuan->AddAnimator(AnimatorQueue);
	m_pSceneNodeQuan->AddAnimator(pQuanAlpha);

	m_pCaiJinScoreAnimation->Reset();
	m_pCaiJinScoreAnimation->RemoveAnimator(NULL);
	//m_pCaiJinScoreAnimation->SetDigitalType(QmObjType(CaiJinDigital));
	QmSetDigitalType(m_pCaiJinScoreAnimation->GetDigitalNode(), CaiJinDigital);
	m_pCaiJinScoreAnimation->SetDigitalVisible(true);
	m_pCaiJinScoreAnimation->SetLivingTime(fShowTime + 0.5f);
	m_pCaiJinScoreAnimation->SetVisible(true);

	QcPoint Score_Pos = QmSysSetting->GetCaiJinPos();
	Score_Pos.Y -= 5.f;
	m_pCaiJinScoreAnimation->SetPosition(Score_Pos);
#ifdef QmTestVersion
    if (iScore > 100000000 && m_pBomberBulletSave)
    {
        QmLog("Player%d Show CaiJIn Error---%u, Explode Score---%u, Bullet Level---%u, Bullet Multi---%u\n",
            GetID(), iScore, m_iExplodeScore, m_pBomberBulletSave->GetLevel(), m_pBomberBulletSave->GetMultiple());
        QmLogError("CaiJin Error");
    }

#endif
	m_pCaiJinScoreAnimation->SetDigitalValue(iScore);
	m_pCaiJinScoreAnimation->SetScale(4.5f);
	QcAnimatorScale* pSocorecSmaller = static_cast<QcAnimatorScale*>(QmAnimatorCreator->MakeScaleAnimator2(4.5f, 0.9f, 0.4f));
	m_pCaiJinScoreAnimation->AddAnimator(pSocorecSmaller);

	QcAnimatorAlpha* pSocorecAlpha = static_cast<QcAnimatorAlpha*>(QmAnimatorCreator->MakeAlphaAnimator2(0.1f, 1.0f, 0.4f));
	m_pCaiJinScoreAnimation->AddAnimator(pSocorecAlpha);

	QcAnimatorAlpha* pSocorecAlpha2 = static_cast<QcAnimatorAlpha*>(QmAnimatorCreator->MakeAlphaAnimator2(1.0f, 0.0f, 0.5f));
	pSocorecAlpha2->SetStartTime(fShowTime -0.5f);
	m_pCaiJinScoreAnimation->AddAnimator(pSocorecAlpha2);
	m_pCaiJinScoreAnimation->SetFinishFlag(pSocorecAlpha2);
	m_pCaiJinScoreAnimation->SetCurrentTransparency(25);
	m_pCaiJinScoreAnimation->SetStartTime(0.5f);
}

void QcPlayer::OnTimer(u32 nTimer, void* pContext)
{
    switch (nTimer)
    {
    case eSuperGunTimer:
		QmFishAlg->StopSuperFire(GetID());
        m_pSuperGunMovingFlag->SetVisible(false);
		if (QmSysSetting->GetLockFishFlag() == eLockFishWithSuperGun)
			CancelLockFish();
		GetGunMgr().SetSuperGun(false);
        break;

    case ePlayerActiveTimer:
		QmSceneMgr->GetPlayerMgr()->SetActivePlayer(0, GetID());
        break;

    case BOMB_WAIT_TIMER:
        //SetTimer(BOMB_GET_TIMER, Randomf(50,100), true);
        m_bCanGetBomber = true;
        break;

    case BOMB_GET_TIMER:
        SetTimer(BOMB_WAIT_TIMER, 50, true);
        m_bCanGetBomber = false;
        break;

    case BOMB_PRESS_FIRST:
		if((rand()%2) ==0)
			QmPlaySound(BombTips2);
		else
			QmPlaySound(BombTips21);
        m_pBomberTips->SetState(eDying);
        break;

    case BOMB_PRESS_SECOND:
        {
            QmAssert(m_pBomberRoper->GetState()==eLiving);
            m_pBomberRoper->SetState(eDying);
            QmPlaySound(BombRopeBurn);
            //const QsStateParameter *pStateParam = m_pBomberRoper->GetDesc()->GetStateParameter(eDying);
            QcAnimatorTexture *pAni = m_pBomberRoper->FindAnimator<QcAnimatorTexture>();
            //pAni->Reset(pStateParam->m_pTextureList, pStateParam->m_fPlayTime*m_pBomberRoper->GetScaleX()/pStateParam->m_nQty, 
            //    false);

            pAni->AddFinishCallBack(boost::bind(&QcPlayer::FinishAnimator, this, _1), BombRoperBurnEnd);
            QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerNormal, "定位炸弹01",
                m_pTheBomber->GetPosition());
        }
        break;

    case BOMB_AUTO_TIMER:
        {
            if (!pContext)
            {
                m_pBomberCounterDown->SetState(eLiving);
                m_pBomberCounterDown->SetVisible(true);

                QiAnimator *pTexAni = m_pBomberCounterDown->FindAnimator<QcAnimatorTexture>();
                pTexAni->AddFinishCallBack(boost::bind(&QcPlayer::FinishAnimator, this, _1), BombAutoExplodeFlag);
                QmPlaySound(BombCounter10);
                SetTimer(BOMB_AUTO_TIMER, 5,true, (void*)5);
            }
            else
            {
                int i = (int)pContext;
                QmOutputString("Counter %d\n", i);
                if (i == 1)
                {
                    QmPlaySound(BombCounter1);
                }
                else
                {
                    switch (i)
                    {
                    case 2:
                        QmPlaySound(BombCounter2);
                        break;
                    case 3:
                        QmPlaySound(BombCounter3);
                        break;
                    case 4:
                        QmPlaySound(BombCounter4);
                        break;
                    case 5:
                        QmPlaySound(BombCounter5);
                        break;
                    default:
                        break;
                    }
                    --i;
                    SetTimer(BOMB_AUTO_TIMER, 1, true, (void*)i);
                }
            }
        }
        break;

	case ePlayerStopAutoFire:
		  {
			  m_bAddStopAutoFire = false;
			  m_bPlayerAutoFire = false;
			  if(m_pAutoFireLogo)
				  m_pAutoFireLogo->SetVisible(false);
		  }
		  break;

    default:
		QcSceneNode::OnTimer(nTimer, pContext);
        break;
    }
}

void QcPlayer::BomberAutoExplode()
{
    m_pBomberBtn->SetState(eDying);
    m_pBomberBtnEff->SetState(eReady);
    m_pBomberBtnEff->SetState(eLiving);
    m_pBomberBtnEff->SetVisible(true);
    m_bBomberLockIo = true;

    m_pBomberTips->SetState(eDying);
    m_pBomberTips->SetVisible(false);
    m_pBomberBtnArrow->SetVisible(false);

    QiAnimator *pAni = m_pBomberBtn->FindAnimator<QcAnimatorTexture>();
    pAni->AddFinishCallBack(boost::bind(&QcPlayer::FinishAnimator, this, _1), BombSecondPressFlag);

    SetTimer(BOMB_PRESS_SECOND, 0.5f, true);
}

void QcPlayer::OnEvent( const QsEvent& evt )
{
    switch (evt.m_iEvt)
    {
    case 0:
        {
            //GetInfo()->AddSDLS_Score(evt.m_iParam);
            QmFishAlg->Repair((int)evt.m_iParam);
        }
        break;
    case 1:
        {
            if (evt.m_iParam == 1)
            {
                m_pSpecialFish = dynamic_cast<QcSceneNode*>(evt.m_pSender);
                m_bSepcialFishDrawTop = true;
				if(!m_bAutoFireAndGambleFish && m_bPlayerAutoFire)
				{	
					m_bAutoFireAndGambleFish = true;
					m_bPlayerAutoFire = false;
					if(m_pAutoFireLogo)
						m_pAutoFireLogo->SetVisible(false);
				}
            }
            else if (m_pSpecialFish)
			{
                QcSceneAnimation *pAnimation = QfMakeEmptyInstance<QcSceneAnimation>();
                pAnimation->AddSubSceneNode(m_pSpecialFish);
                pAnimation->SetLivingTime(10);
                QmSceneMgr->GetAnimationMgr()->AddAnimation(pAnimation, eAnimationLayerDyingFish);
                m_pSpecialFish = NULL;
				if (m_bAutoFireAndGambleFish && !m_bPlayerAutoFire)
				{
					m_bAutoFireAndGambleFish = false;
					m_bPlayerAutoFire = true;
					if(m_pAutoFireLogo)
						m_pAutoFireLogo->SetVisible(true);
				}
			}
        }
        break;
    case 2:
        {
            m_pSpecialFish = NULL;
        }
        break;
    }
}

void QcPlayer::AutoFire()
{
	QcSceneMgr::QeSceneState eSceneState = QmSceneMgr->GetSceneState();
	if ((eSceneState == QcSceneMgr::eSceneNormal || eSceneState == QcSceneMgr::eSceneRegular) && m_nCurrentBullet < QmSysSetting->GetMaxLivingBullet())
	{
		if (OnFire())
		{
			UpdateScore();
			KillTimer(ePlayerStopAutoFire);
			m_bAddStopAutoFire = false;
			if (GetGunMgr().IsSuperGun())
			{
				QmPlaySound(SuperGunFireSound);
			}
			else
			{
				QmPlaySound(NormalFireSound);
			}
		}
	}		
	if (GetInfo()->GetCurScore() <= 0 && !m_bAddStopAutoFire)
	{
		m_bAddStopAutoFire = true;
		SetTimer(ePlayerStopAutoFire, 10.0f, true);
	}
}

int get_random_bonus()
{
    return 1 + (int)(3 * SPDCRandom());
}