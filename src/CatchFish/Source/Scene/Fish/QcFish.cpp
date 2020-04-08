#include "pch.hpp"
#include "QcFish.hpp"
#include "QcFishMgr.hpp"
#include "QcFishFilter.hpp"
#include "Scene\QcSceneMgr.hpp"
#include "Scene\QcGoldCoinMgr.hpp"
#include "Scene\QcBullet.hpp"
#include "Scene\QcPlayer.hpp"
#include "Scene\QcSceneAnimation.hpp"
#include "Scene\QcSceneAnimationMgr.hpp"
#include "Animator\QcAnimatorMoveOffset.hpp"
#include "Animator\QiAnimatorDeque.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "Animator\QcAnimatorMoveLine.hpp"
#include "Animator\QcAnimatorScale.hpp"
#include "Animator\QcAnimatorMoveCircle.hpp"
#include "Animator\QcAnimatorRotation.hpp"
#include "Animator\QcAnimatorDither.hpp"
#include "Res\QcResFishDesc.hpp"
#include "Res\QcTexture.hpp"
#include "EffectRender\QcEffectMgr.hpp"
#include "EffectRender\QcEffect.hpp"

class ScoreAnimatorInfo
{
public:
	ScoreAnimatorInfo()
	{
		QcMathNodeInfo info;
		info.SetPos(QcPoint(0.0f, 0.0f));
		m_offsetInfo.push_back(info);

		info.SetPos(QcPoint(0.0f, 22.0f));
		m_offsetInfo.push_back(info);

		info.SetPos(QcPoint(0.0f, 61.0f));
		m_offsetInfo.push_back(info);

		info.SetPos(QcPoint(0.0f, 66.0f));
		m_offsetInfo.push_back(info);

		info.SetPos(QcPoint(0.0f, 61.0f));
		m_offsetInfo.push_back(info);

		info.SetPos(QcPoint(0.0f, 22.0f));
		m_offsetInfo.push_back(info);

		info.SetPos(QcPoint(0.0f, 0.0f));
		m_offsetInfo.push_back(info);

		m_fTimeOneOffset = 0.05f;
	}
public:
	std::vector<QcMathNodeInfo> m_offsetInfo;
	QcMathNodeInfo m_originInfo;
	f32 m_fTimeOneOffset;
};
static ScoreAnimatorInfo gsScoreMoveInfor;

ImplRuntimeClass2(QcFish, 300);

QcFish::QcFish()
: m_eDeadWay(eNone)
, m_pEndAnimator(NULL)
, m_bShowedCoin(false)
, m_bSheltered(false)
, m_toState(eStateNone)
, m_iFishFlag(0)
, m_pOwner(NULL)
, m_fDeep(0.f)
{

}
QcFish::~QcFish()
{
}

void QcFish::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	giScreenID = giScreenHandleing;

	BaseClass::Init(desc, bLoadSubNode);
	const QcResFishDesc& fishDesc = static_cast<const QcResFishDesc&>(desc);

	m_nID = fishDesc.GetFishType();
	m_iLayer = m_nID;
	m_iPlayingTime = (fishDesc.GetPlayingFlag() & eRandomPlay) ? 1 : 0xFFFFFFFF;

	u32 iScore = fishDesc.GetScore();
	CalSpecialScore(fishDesc.GetObjType(), iScore);
	SetScore(iScore);
	ShowScore(false);

	if (m_oDigitalNode.GetDesc() == NULL)
	{
		//m_oDigitalNode.SetDigitalType(QmObjType(FishScore));
		QmSetDigitalType(m_oDigitalNode, FishScore);
		SetScorePos(QcPoint(0.f, GetHeight() * 0.5f));
		m_oDigitalNode.SetAlign(eAlignCenterBottom);
	}
}

void QcFish::Animate(double dCurrentTime,f32 fElapsedTime)
{
    BaseClass::Animate(dCurrentTime,fElapsedTime);
    if (m_pSpecialObj) m_pSpecialObj->Animate(dCurrentTime, fElapsedTime);
	QmAssert(giScreenID == giScreenHandleing);
}

void QcFish::ToDead()
{
	QmAssert(giScreenID == giScreenHandleing);
	RemoveAllLocker();
	MakeCoinAnimation();
	BaseClass::ToDead();
	QmSceneMgr->GetFishMgr()->OnDead(this);
    SetFishFlag(0);
}

inline void QcFish::MakeCoinAnimation()
{
	if (!m_bShowedCoin && m_pOwner)
	{
		QmSceneMgr->GetAnimationMgr()->MakeCoin(GetScore(), m_pBullet->GetMultiple()*m_pBullet->GetLevel(),
            GetPosition(), m_pOwner->GetAbsCoinEndPt(), m_pOwner->GetRotation(), 0.f);
		m_bShowedCoin = true;
	}
}

void QcFish::AddAnimator(QiAnimator* pAnimator)
{
	if (pAnimator->IsTextureAnimator())
		m_pTextureAnimator = static_cast<QcAnimatorTexture*>(pAnimator);
	BaseClass::AddAnimator(pAnimator);
}

void QcFish::FinishAnimator(const QiAnimator* pAnimator)
{
	if (IsLiving())
	{
		if ( (m_pEndAnimator == NULL && pAnimator->IsMoveAnimator()) || (pAnimator == m_pEndAnimator) )
		{
			ToDead();
			m_pEndAnimator = NULL;
		}
	}

	if (pAnimator == m_pTextureAnimator)
	{
		//m_pTextureAnimator = NULL;
		if (m_toState != eStateNone)
		{
			PostEvent(eEvtFishStateChange);
		}
		else
		{
			switch(m_eCurrentState)
			{
			case ePlaying:
			case eDownToLiving:
			case eUpToLiving:
				{
					m_toState = eLiving;
					PostEvent(eEvtFishStateChange);
					break;
				}
			case eLivingToDown:
				{
					m_toState = eDowning;
					PostEvent(eEvtFishStateChange);
					break;
				}
			case eLivingToUp:
				{
					m_toState = eUping;
					PostEvent(eEvtFishStateChange);
					break;
				}
			}
		}
		return;
	}
	BaseClass::FinishAnimator(pAnimator);
}

QcAnimatorMove* QcFish::FindMoveAnimator()
{
	QcAnimatorPtrListIter iter(m_arAnimatorList.begin());
	QcAnimatorPtrListIter endIter(m_arAnimatorList.end());
	for (; iter != endIter; ++iter)
	{
		if (*iter && (*iter)->IsReady())
		{
			if ((*iter)->IsMoveAnimator())
				return static_cast<QcAnimatorMove*>(iter->Get());
			else if ((*iter)->IsAnimatorDeque())
			{
				QiAnimator* pAnimator = static_cast<QiAnimatorDeque*>(iter->Get())->CurrentAnimator();
				if (pAnimator && pAnimator->IsReady() && pAnimator->IsMoveAnimator())
					return static_cast<QcAnimatorMove*>(pAnimator);
			}
		}
	}
	return NULL;
}

void QcFish::Frozen(bool bFrozen)
{
	QcAnimatorMove* pMove = FindMoveAnimator();
	if (pMove)
		pMove->Suspend(bFrozen);
}

bool QcFish::ToDie(bool bDieTimer)
{
	QmAssert(m_bIsLiving);
	RemoveAllLocker();
	RemoveAnimator(NULL);
	m_pTextureAnimator = NULL;
	return QcSceneDigital::ToDie(bDieTimer);
}

void QcFish::HitToDie(QcBullet* pBullet)
{
	m_pOwner = pBullet->GetOwner();
	m_pBullet = pBullet;
	ToDie();

	QcVector vec(1.0f, 0.f);
	vec.rotateByRad(m_pBullet->GetRotation());
	QcPoint end_pos = PushFishAnimation(vec);

	//注：大鱼自转起来幅度太大，根据分数不让大鱼自转
	if (GetScore() < 30)
	{
		QcPoint Circle_pos = end_pos;
		Circle_pos.X += 15 * cos(m_pOwner->GetRotation() + HALF_PI);
		Circle_pos.Y += 15 * sin(m_pOwner->GetRotation() + HALF_PI);
		QcAnimatorMoveCircle* pFishCricle = static_cast<QcAnimatorMoveCircle*>(QmAnimatorCreator->MakeCircleAnimator(Circle_pos,2*TWO_PI,true,false));
		pFishCricle->SetStartTime(GetFishDesc()->GetDyingTime() * 0.2f);
		AddAnimator(pFishCricle);
	}
	if (GetScore() >= 30)
	{
		if (RandomInt(0,10) < 8)
		{
			static const QcObjType QcCVList[] = 
			{
				QmObjName(mcv01),
				QmObjName(mcv02),
				QmObjName(mcv03),
				QmObjName(mcv04),
				QmObjName(mcv05),
				QmObjName(mcv06),
				QmObjName(mcv07),
				QmObjName(mcv08),
				QmObjName(mcv09),
				QmObjName(wcv01),
				QmObjName(wcv02),
				QmObjName(wcv03),
				QmObjName(wcv04),
				QmObjName(wcv05),
				QmObjName(wcv06),
				QmObjName(wcv07),
				QmObjName(wcv08),
				QmObjName(wcv09),
			};
			QcSound* psTempSound = QmSoundMgr->FindSound(QcCVList[RandomInt(0, QmCountOf(QcCVList))]);
			if (psTempSound)
				psTempSound->Play(false);
		}
	}
	

	if (GetObjType() != QmObjName(ScopeBomb) && GetObjType() != QmObjName(DelayScopeBomb))
	{
		MakeDyingAnimations(pBullet->GetMultiple() * pBullet->GetLevel() * GetScore(), end_pos, GetFishDesc()->GetDyingTime() * 0.2f);
	}

	const QcString& dieSound = GetFishDesc()->GetDieSound();
	if (dieSound.size())
	{
		QmSoundMgr->Play(dieSound);
	}
	else
	{
		QmPlaySound(FishDieSound);
	}

	const QcString& bombEffect = GetFishDesc()->GetBombEffect();
	if (bombEffect.size())
		QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, bombEffect.c_str(), GetPosition());

	if(GetObjType() == QmObjName(fish16) || GetObjType() == QmObjName(fish17) || GetObjType() == QmObjName(fish18))
	{
		QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerDyingFish, "打死大型鱼00", GetPosition());
		if(GetObjType() == QmObjName(fish18))
			SetTimer(eTimerParticleGap, 0.1f, true);
	}
}

void QcFish::ExploseToDie(QcFish* pFish)
{
	m_pBullet = pFish->GetBullet();
	m_pOwner = pFish->GetOwner();
    if (m_bIsLiving)
    {
        MakeDyingAnimations(m_pBullet->GetMultiple() * m_pBullet->GetLevel() * GetScore(), GetPosition(), 0.f);
        ToDie(true);
    }
}
void QcFish::MakeDyingAnimations(u32 iScore, const QcPoint& pos, f32 delaytime, f32 dyingTime)
{
	QcPlayer* pPlayer = m_pOwner;
	QcSceneAnimationMgr* pAnimationMgr = QmSceneMgr->GetAnimationMgr();
	f32 fPlayerRotation = pPlayer->GetRotation();

	QcSceneAnimation* pAnimation = QfMakeEmptyInstance<QcSceneAnimation>();
	pAnimation->SetRenderState(QmMakeTexState(gNumTexState));
	//分数运动
	QcPoint CircleCenter = pos;
	CircleCenter.X += 60 * cos(fPlayerRotation + HALF_PI);
	CircleCenter.Y += 60 * sin(fPlayerRotation + HALF_PI);
	QcPoint temp_pos = CircleCenter;
	f32 randRotayion = Randomf(0.f, TWO_PI);
	temp_pos.X += 10 * cos(randRotayion);
	temp_pos.Y += 10 * sin(randRotayion);
	QcAnimatorMoveCircle* pScoreMoveCircle = static_cast<QcAnimatorMoveCircle*>(QmAnimatorCreator->MakeCircleAnimator(CircleCenter, TWO_PI,true,false,2.8f*TWO_PI));
	
	f32 scale = 1.0f;
	f32 big = 1.0f;
	if (GetScore() <= 10)
	{
		scale = 0.4f;
		big = 1.0f;
	}else if (GetScore() <= 50)
	{
		scale = 0.5f;
		big = 1.0f;
	}else
	{
		scale = 0.6f;
		big = 2.0f;
	}
	pAnimation->SetScale(0.1f);
	QcAnimatorScale* pScoreBigger = static_cast<QcAnimatorScale*>(QmAnimatorCreator->MakeScaleAnimator2(0.1f, big,0.23f));
	QcAnimatorScale* pScoreSmaller = static_cast<QcAnimatorScale*>(QmAnimatorCreator->MakeScaleAnimator2(big, scale,0.23f));
	QiAnimatorDeque* ScoreAnimatorQueue = QmAnimatorCreator->MakeAnimatorDeque();
	ScoreAnimatorQueue->AddAnimator(pScoreBigger);
	ScoreAnimatorQueue->AddAnimator(pScoreSmaller);
	ScoreAnimatorQueue->AddAnimator(pScoreMoveCircle);

	pAnimation->SetStartTime(delaytime);
    if (dyingTime == 0)
        dyingTime = GetFishDesc()->GetDyingTime();
	pAnimation->SetLivingTime(dyingTime);
	pAnimation->AddAnimator(ScoreAnimatorQueue);
	pAnimation->SetFinishFlag(ScoreAnimatorQueue);
	//pAnimation->SetDigitalType(QmObjType(getscore));
	QmSetDigitalType(pAnimation->GetDigitalNode(), getscore);
	pAnimation->SetDigitalValue(iScore);
	pAnimation->SetPosition(temp_pos);
	pAnimation->SetRotation(fPlayerRotation);
	pAnimation->SetDigitalVisible(true);
	QcDigitalNode& tempDigitalNode = pAnimation->GetDigitalNode();
	tempDigitalNode.SetColor(pPlayer->GetColorFlag());
	pAnimationMgr->AddAnimation(pAnimation, eAnimationLayerGunEffect);
}

void QcFish::ScoreChange(u32 iRaw, u32 iNow)
{
	SetDigitalValue(iNow);
}

void QcFish::AddLocker(QcPlayer* pLocker)
{
	int iPos = m_arLockerList.Find((QcPlayer*)(NULL));
	if (iPos != -1)
	{
		m_arLockerList[iPos] = pLocker;
	}
	else
	{
		m_arLockerList.PushBack(pLocker); 
	}
}
void QcFish::RemoveLocker(QcPlayer* pLocker)
{ 
	int iPos = m_arLockerList.Find(pLocker);
	QmAssert(iPos != -1);
	m_arLockerList[iPos] = NULL;
}

void QcFish::RemoveAllLocker()
{
	QcList<QcPlayer*>::iterator iter(m_arLockerList.begin());
	QcList<QcPlayer*>::iterator endIter(m_arLockerList.end());
	for (; iter != endIter; ++iter)
	{
		if (*iter)
		{
			(*iter)->CancelLockFish();
			(*iter) = NULL;
		}
	}
	m_arLockerList.Clear();
}

QeState QcFish::ToShowState(QeState eState) 
{ 
	m_toState = GetDesc()->GetStateParameter(eState) != NULL ? eState : eStateNone;
	if (m_toState != eStateNone)
	{
		if (NULL == m_pTextureAnimator)
		{
			m_eCurrentState = eStateNone;
			SetState(m_toState);
			m_toState = eStateNone;
			return m_eCurrentState;
		}
		else
		{
			m_pTextureAnimator->SetLoop(false);
		}
	}
	return m_toState;
}

QeState QcFish::TryToPlayingState()
{
	if (m_iPlayingTime > 0)
	{
		--m_iPlayingTime;
		return ToShowState(ePlaying);
	}
	return eStateNone;
}

void QcFish::OnEvent(const QsEvent& evt)
{
	switch(evt.m_iEvt)
	{
	case eEvtFishStateChange:
		{
			if (m_toState != eStateNone)
			{
				StateChange(m_eCurrentState, m_toState);
				m_eCurrentState = m_toState;
				m_toState = eStateNone;
			}
			break;
		}
	}
}

void QcFish::HitAnimator(QcBullet* pBullet)
{
 	if (m_pEffectRender)
 	{
 		m_pEffectRender->OnHit();
 	}
	QcAnimatorMove* pMove = FindMoveAnimator();
	if (pMove && Randomf(0,1) < 0.4f)
	{
		const QsStateParameter* pPlayingState = GetDesc()->GetStateParameter(ePlaying);
		if (pPlayingState)
			pMove->OnHit(pPlayingState->m_fPlayTime);
	}
}

QcPoint QcFish::PushFishAnimation(const QcVector& vec)
{
	QcVector my_vec = vec;
	my_vec.normalize();

	f32 fTime = GetFishDesc()->GetDyingTime();
	QcPoint showscorepos = GetPosition();
	f32 delaytime = 0.f;
	if (!QcMath::equals(fTime, 0.f))
	{
		//QcVector vec(1.0f, 0.f);
		//vec.rotateByRad(m_pBullet->GetRotation());

		fTime *= 0.2f;

		f32 fLen = 10.f + QcMath::max_(50.f - GetScore(), 0.f) * 3.f;
		f32 fGravitySpeed = fLen * 2.0f/(fTime * fTime);
		f32 fSpeed = fGravitySpeed * fTime;

		QcPoint startPos = GetPosition();
		QcPoint endPos = startPos + my_vec * fLen;
		if (!QmGlobalData.gScreenRect.isPointInside(endPos))
		{
			QmGlobalData.gScreenRect.IntersectWith(QcLine(startPos, endPos), &endPos);
		}
		showscorepos = endPos;

		delaytime = fTime;

		QcGravitySpeed* pGravitySpeed = new QcGravitySpeed(-fGravitySpeed, fTime * 0.85f);
		QcAnimatorMove* pAnimator = QmAnimatorCreator->MakeLineAnimator2(startPos, endPos, fSpeed);
		pAnimator->AddSpeedAnimator(pGravitySpeed);
		AddAnimator(pAnimator);
	}
	return showscorepos;
}

void QcFish::CalSpecialScore(const QcObjType& objType, u32& iScores)
{
	if (objType == QmObjName(fish19))
	{ 
        double ran;
        int i;
        const double prob[] = {0.23, 0.23, 0.22, 0.22, 0.06, 0.04};
        int multi;

        ran = SPDCRandom();
        i = 0;
        while(i < sizeof(prob) && ran >= prob[i])
        {
            ran -= prob[i];
            i++;
        }

        if (i < 5)
        {
            multi = 100 + i * 50 + int(SPDCRandom() * 50);
        }
        else
        {
            multi = 350 + int(SPDCRandom() * 51);
        }

        if (multi > 400)
        {
            multi = 400;
        }

        iScores = multi;

	}
	//else if (objType == QmObjName(fish17))
	//{
	//	iScores = RandomInt(50,100);
	//}
}

void QcFish::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eTimerParticleGap)
		QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerDyingFish, "打死大型鱼02", GetPosition());
		//QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerDyingFish, "打中大鱼后", GetPosition());
	else
		QcSceneDigital::OnTimer(nTimer, pContext);
}

void QcFish::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	QmAssert(giScreenID == giScreenHandleing);
    if (m_pSpecialObj)
	{
        m_pSpecialObj->Render(render, pWorldMatrix ? &(*pWorldMatrix * GetMatrix()) : &GetMatrix());
	}
	BaseClass::Render(render,pWorldMatrix);

#ifdef QmTestVersion
	if (/*IsSheltered()&&*/QmDebugHelper.IsShowPathIDOn())
	{
		char buf[1024];
		sprintf_s(buf,"Fish:%s%s%s%s%s\n",m_PathID.c_str(),m_PathID.c_str(),m_PathID.c_str(),m_PathID.c_str(),m_PathID.c_str());
		//OutputDebugStringA(buf);
		const QcPoint& fish_pos = GetPosition();
		render.RenderText(buf,QfMakeScreenRect(fish_pos.X + QmGlobalData.gWindowSize.Width * 0.5f,-fish_pos.Y+QmGlobalData.gWindowSize.Height * 0.5f,50.f,50.f));
    }
#endif
}