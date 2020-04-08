#include "pch.hpp"
#include "QcSceneLogo.hpp"
#include "QcBackGround.hpp"
#include "EffectRender/QcEffectMgr.hpp"
#include "EffectRender/DcWaterEffect.hpp"
#include "Animator/QcAnimatorAlpha.hpp"
#include "Animator/QiAnimatorDeque.hpp"

ImplRuntimeClass(QcSceneLogo);

void QcSceneLogo::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcBaseClass::Init(desc, bLoadSubNode);

    m_pWaterEffect = QmResMgr->GetEffectMgr()->MakeEffect("Water")->As<DcWaterEffect*>();
}

void QcSceneLogo::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcBaseClass::Animate(dCurrentTime, fElapsedTime);
    m_pNextBackGround->Animate(dCurrentTime, fElapsedTime);
	if (m_pNextMirrorBackGround)
		m_pNextMirrorBackGround->Animate(dCurrentTime, fElapsedTime);
	//m_pStencilNode->Animate(dCurrentTime, fElapsedTime);
	m_pWaterEffect->OnAnimate(NULL, dCurrentTime, fElapsedTime);
}

void QcSceneLogo::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (IsVisible())
	{
        //render logo.
        QmAutoRevertAlpha(render);
        render.SetMaterialColor(m_color);
        RenderSub(render, &GetMatrix());

		//render background
		if (m_pNextMirrorBackGround)
		{
			render.SwitchToSubScreen(eScreen1);
			m_pNextBackGround->Render(render);
			m_pNextBackGround->RenderBgSceneNode(render, true);

			render.SwitchToSubScreen(eScreen2);
			m_pNextMirrorBackGround->Render(render);
			m_pNextMirrorBackGround->RenderBgSceneNode(render, true);

			render.SwitchToSubScreen(-1);
		}
		else
		{
			m_pNextBackGround->Render(render);
			m_pNextBackGround->RenderBgSceneNode(render, true);
		}
		m_pWaterEffect->OnRender(render, NULL);
	}
}

void QcSceneLogo::Start(const QcBackGround* pSceneNode, const QcBackGround* pMirrorBg)
{
	m_bFinished = false;

	m_pNextBackGround.Set(pSceneNode);
    QiAnimator* pAnimator = QmAnimatorCreator->MakeAlphaAnimator2(0, 1, 3);
    pAnimator->SetStartTime(6);
    m_pNextBackGround->SetCurrentTransparency(0);
    m_pNextBackGround->AddAnimator(pAnimator);

	m_pNextMirrorBackGround = pMirrorBg;
	if (pMirrorBg)
	{
		QiAnimator* pAnimator = QmAnimatorCreator->MakeAlphaAnimator2(0, 1, 3);
		pAnimator->SetStartTime(6);
		m_pNextMirrorBackGround->SetCurrentTransparency(0);
		m_pNextMirrorBackGround->AddAnimator(pAnimator);
	}
	
	QiAnimatorDeque* animatorDeque = QmAnimatorCreator->MakeAnimatorDeque();
    pAnimator = QmAnimatorCreator->MakeAlphaAnimator2(0, 1, 1);
    AddAnimator(pAnimator);
    //pAnimator = QmAnimatorCreator->MakeScaleAnimator2(5, 1, 1);
    //AddAnimator(pAnimator);
	//////////////////////////////////////////////////////////////////////////
	pAnimator = QmAnimatorCreator->MakeScaleAnimator2(5, 1, 1);
	animatorDeque->AddAnimator(pAnimator);
	pAnimator = QmAnimatorCreator->MakeScaleAnimator2(1, 1.03f, 0.25f);
	animatorDeque->AddAnimator(pAnimator);
	pAnimator = QmAnimatorCreator->MakeScaleAnimator2(1.03f, 0.98f, 0.2f);
	animatorDeque->AddAnimator(pAnimator);
	pAnimator = QmAnimatorCreator->MakeScaleAnimator2(0.98f, 1.02f, 0.2f);
	animatorDeque->AddAnimator(pAnimator);
	pAnimator = QmAnimatorCreator->MakeScaleAnimator2(1.02f, 0.99f, 0.3f);
	animatorDeque->AddAnimator(pAnimator);
	pAnimator = QmAnimatorCreator->MakeScaleAnimator2(0.99f, 1.f, 0.3f);
	animatorDeque->AddAnimator(pAnimator);
	AddAnimator(animatorDeque);
	//////////////////////////////////////////////////////////////////////////
	pAnimator->AddFinishCallBack(QcFinishAnimatorCb(boost::bind(&QcSceneLogo::FinishAnimator, this, _1)), 0);

    SetLivingTime(15);

    SetScale(5);
    SetCurrentTransparency(0);

    m_pWaterEffect->SetWaterDamp(0.999f);
    m_pWaterEffect->SetUpdateTick(0.004f);
    m_pWaterEffect->SetRenderTime(3);

    SetTimer(eSceneLogoTimer, 1, true);

    switch (QmSysSetting->GetPlayerNumber())
    {
    case eMachine_4_2:
    case eMachine_3_2:
        break;
    default:
        SetRotation(-RandomInt()%2 * PI);
        break;
    }
}

void QcSceneLogo::FinishAnimator(u32 iParm)
{
	//QmSoundMgr->Play(QmSoundType(LogoSound2));
	QmPlaySound(LogoSound2);
}

void QcSceneLogo::OnTimer( u32 nTimer, void* pContext )
{
    switch (nTimer)
    {
    case eSceneLogoTimer:
        {
            m_pWaterEffect->SetWaterForce(1);
            m_pWaterEffect->SetWaterPos(D3DXVECTOR3(0,0,0));
            SetTimer(eSceneWaterInterTimer, 1.5f);
            SetTimer(eSceneWaterTotalTime, 5, true);
        }
        break;

    case eSceneWaterInterTimer:
        {
            int num = RandomInt2(3, 8);
            m_pWaterEffect->SetUpdateTick(0.03f);
            m_pWaterEffect->SetRenderTime(7.4f);
            m_pWaterEffect->SetWaterDamp(Randomf(0.96f, 0.975f));
            m_pWaterEffect->SetWaterForce(Randomf(0.01f, 1));
            for (int i=0; i<num; i++)
                m_pWaterEffect->SetWaterPos(D3DXVECTOR3(Randomf(-960,960), Randomf(-540,540), 0));
            SetTimer(eSceneWaterInterTimer, Randomf(0.05f, 0.2f));
        }
        break;

    case eSceneWaterTotalTime:
        {
            KillTimer(eSceneWaterInterTimer);
        }
        break;

    default:
        break;
    }
}
