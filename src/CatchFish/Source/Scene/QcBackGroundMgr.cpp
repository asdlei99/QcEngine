#include "pch.hpp"
#include "QcBackGroundMgr.hpp"
#include "QcBackGround.hpp"
#include "Res\QcResManager.hpp"
#include "Scene\QcSceneLogo.hpp"
#include "Animator\QcAnimatorDither.hpp"
#include "EffectRender\QcEffect.hpp"


QcBackGroundMgr::QcBackGroundMgr()
: m_iCurrent(0)
, m_bChanging(false)
{
}

QcBackGroundMgr::~QcBackGroundMgr()
{

}

bool QcBackGroundMgr::Init(QcResManager* pResManager)
{
	m_pDitherAnimator = static_cast<QcAnimatorDither*>(QmAnimatorCreator->MakeDitherAnimator(2.0f, 5.0f, QcPoint()));

	for (int i=0; i<QmCountOf(QaSceneList); i++)
	{
		QcBackGround* pBg = pResManager->MakeGameInstance(QaSceneList[i])->As<QcBackGround*>();
		m_arSceneNodeList.PushBack(pBg);
	}
	m_oSceneLogo = pResManager->MakeGameInstance(QmObjType(SceneLogo))->As<QcSceneLogo*>();
	MakeWave(m_iCurrent);

	if (QmSysSetting->IsUionScreenMode())
	{
		for (int i=0; i<QmCountOf(QaSceneList); i++)
		{
			QcBackGround* pBg = pResManager->MakeGameInstance(QaSceneList[i])->As<QcBackGround*>();
			pBg->MirrorBgNode();
			m_arMirrorBgList.PushBack(pBg);
		}
	}
	return true;
}

void QcBackGroundMgr::StartLogoAnimation()
{
	if (!m_bChanging)
	{
		m_bChanging = true;
		m_oSceneLogo->Start(GetBackGound(GetNext()), GetMirrorBg(GetNext()));
		int i = GetNext() % m_arSceneNodeList.size();
		MakeWave(i);
		QmSoundMgr->Stop(GetBackGound(m_iCurrent)->GetMusic());
		QmPlaySound(LogoSound1);
		//QmSoundMgr->Play(QmSoundType(LogoSound1));
	}
}

void QcBackGroundMgr::MakeWave(int iScene)
{
	if (GetBackGound(iScene)->GetWave())
	{
		m_pWave = QmResMgr->MakeGameInstance(GetBackGound(iScene)->GetWave());
		f32 xyScale[] = {QmGlobalData.gScreenRect.getWidth()/m_pWave->GetWidth(), QmGlobalData.gScreenRect.getHeight()/m_pWave->GetHeight()};
		m_pWave->SetScale(xyScale[0], eMaskX);
		m_pWave->SetScale(xyScale[1], eMaskY);
		m_pWave->SetCurrentTransparency(u32(GetBackGound(iScene)->GetWaveAlpha() * 255.f));
		if (m_pWave->GetRender())
			m_pWave->GetRender()->SetEffectFloatArray("uvScale", xyScale, QmCountOf(xyScale));
	}
}

void QcBackGroundMgr::NextScene(bool bStartGame)
{
	if (!bStartGame)
	{
		m_bChanging = false;
		m_iCurrent = GetNext();
	}
	QmSoundMgr->Play(GetBackGound(m_iCurrent)->GetMusic(), true);
}

void QcBackGroundMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
	GetBackGound(m_iCurrent)->Animate(dCurrentTime, fElapsedTime);
	if (GetCurrentMirrorBg())
	{
		GetCurrentMirrorBg()->Animate(dCurrentTime, fElapsedTime);
	}

	if (m_bChanging && m_oSceneLogo)
	{
		m_oSceneLogo->Animate(dCurrentTime, fElapsedTime);
	}
	if (m_pWave)
		m_pWave->Animate(dCurrentTime, fElapsedTime);
}

void QcBackGroundMgr::Render(QcDeviceRender& render)
{
	if (GetCurrentMirrorBg())
	{
		render.SwitchToSubScreen(eScreen1);
		GetBackGound(m_iCurrent)->Render(render);
		render.SwitchToSubScreen(eScreen2);
		GetCurrentMirrorBg()->Render(render);
		render.SwitchToSubScreen(-1);
	}
	else
	{
		GetBackGound(m_iCurrent)->Render(render);
	}
}

void QcBackGroundMgr::RenderBgSceneNode(QcDeviceRender& render)
{
	if (GetCurrentMirrorBg())
	{
		render.SwitchToSubScreen(eScreen1);
		GetBackGound(m_iCurrent)->RenderBgSceneNode(render);
		render.SwitchToSubScreen(eScreen2);
		GetCurrentMirrorBg()->RenderBgSceneNode(render);
		render.SwitchToSubScreen(-1);
	}
	else
	{
		GetBackGound(m_iCurrent)->RenderBgSceneNode(render);
	}
}

bool QcBackGroundMgr::RenderLogoAnimation(QcDeviceRender& render)
{
	bool bRet = false;
	if (m_oSceneLogo)
	{
		m_oSceneLogo->Render(render);
		RenderWave(render);
		bRet = m_oSceneLogo->IsFinished();
	}
	return bRet;
}

void QcBackGroundMgr::RenderWave( QcDeviceRender& render)
{
	if (m_pWave)
	{
		m_pWave->Render(render);
	}
}

void QcBackGroundMgr::Dither(f32 fShakeTime, f32 fShakeScope)
{
	m_pDitherAnimator->Reset(fShakeTime, fShakeScope, QcPoint(0,0));
	GetBackGound(m_iCurrent)->AddAnimator2(m_pDitherAnimator);
	if (GetCurrentMirrorBg())
	{
		GetCurrentMirrorBg()->AddAnimator2(m_pDitherAnimator);
	}
}


