#include "pch.hpp"
#include "QcSceneMgr.hpp"
#include "QcBulletMgr.hpp"
#include "QcPlayerMgr.hpp"
#include "QcSceneNode.hpp"
#include "QcBullet.hpp"
#include "QcPlayer.hpp"
#include "QcBackGroundMgr.hpp"
#include "QcSceneAnimationMgr.hpp"
#include "Fish\QcFishMgr.hpp"
#include "Setting\QcSettingMgr.hpp"

#define QmScreenSceneCount (IsTwoScreenMode() ? (QmMaxScreen) : 1)


QcSceneMgr::QcSceneMgr()
: m_eSceneState(eSceneNormal)
, m_bSceneStateTimerOn(false)
, m_iLockNormalScene(false)
, m_pSettingMgr(NULL)
{
	QmGlobalData.gpSceneMgr = this;

	m_IsUionScreenMode = QmSysSetting->IsUionScreenMode();
	m_IsTwoScreenMode = QmSysSetting->IsTwoScreenMode();
}
QcSceneMgr::~QcSceneMgr()
{
}

void QcSceneMgr::Init(QcResManager* pResMgr)
{
	m_nLockStateCount = QmScreenSceneCount;
	m_iLockStateCount = 0;

	m_pSettingMgr = pResMgr->GetSettingMgr();

	m_pPlayerMgr = new QcPlayerMgr();
	GetPlayerMgr()->Init(pResMgr, QmSysSetting->GetPlayerNumber());

	QcPoint vParticleOffsetPos[QmMaxScreen];
	vParticleOffsetPos[eScreen1] = QcPoint(QmGlobalData.gWindowSize.Width * 0.5f, QmGlobalData.gWindowSize.Height * 0.5f);
	if (IsTwoScreenMode())
	{
		vParticleOffsetPos[eScreen2] = vParticleOffsetPos[eScreen1];
		vParticleOffsetPos[eScreen1].X += QmGlobalData.gWindowSize.Width;
	}
	for (int i=0; i<QmScreenSceneCount; ++i)
	{
		QmAutoRevertInit(s32, giScreenHandleing, i);
		m_pFishMgr[i] = (new QcFishMgr());
		m_pBulletMgr[i] = (new QcBulletMgr());
		m_pAnimationMgr[i] = (new QcSceneAnimationMgr(vParticleOffsetPos[i]));
		m_pBackGroundMgr[i] = (new QcBackGroundMgr());

		GetBgMgr()->Init(pResMgr);
		GetFishMgr()->Init(pResMgr, "ResFishConfig.xml");
		GetAnimationMgr()->Init(pResMgr);

		if (!(IsTwoScreenMode()))
		{
			break;
		}
	}

	m_pReadySetting = pResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(SetttingReady));
	m_pReadySetting->SetPosition(QcPoint(QmGlobalData.gScreenRect.left() + m_pReadySetting->GetWidth() * 0.5f, QmGlobalData.gScreenRect.top() - m_pReadySetting->GetHeight() * 0.5f));
	m_pReadySetting->SetVisible(false);

	m_pQuickAccount = pResMgr->MakeSceneNodeInstance<QcQuickAccountShowNode>(QmObjType(QuickAccountBg));
	m_pQuickAccount->SetPosition(QcPoint(QmGlobalData.gScreenRect.left() + m_pQuickAccount->GetWidth() * 0.5f, QmGlobalData.gScreenRect.top() - m_pQuickAccount->GetHeight() * 0.5f));
	m_pQuickAccount->SetVisible(false);

	m_pErrorBg = pResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(ErrorBg));
	m_pErrorBg->SetPosition(QcPoint(.0f,80.f));

	m_pIO1ConnectErrorNode = pResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(ConnectError));
	m_pIO1ConnectErrorNode->SetVisible(false);

	if (QmSysSetting->IsTwoIOMode())
	{
		m_pIO2ConnectErrorNode = pResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(ConnectError));
		m_pIO2ConnectErrorNode->SetVisible(false);
	}

	m_pCardErrorNode = pResMgr->MakeSceneNodeInstance<QcSceneDigital>(QmObjType(CardError));
	QmSetDigitalType(m_pCardErrorNode->GetDigitalNode(), DigitalReBoot);
	m_pCardErrorNode->GetDigitalNode().SetPosition(QcPoint(m_pCardErrorNode->GetWidth() * 0.5f + 10.f, 0.f));
	m_pCardErrorNode->GetDigitalNode().SetAlign(eAlignLeftCenter);
	m_pCardErrorNode->GetDigitalNode().SetVisible(true);
	m_pCardErrorNode->SetVisible(false);

	m_pDaMaErrorNode = pResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(DaMaError));
	m_pDaMaErrorNode->SetVisible(false);

	m_eSceneState = eSceneNormal;
	m_iLockNormalScene = 0;
	m_bSceneStateTimerOn = false;

	m_pVolumnCtrl = pResMgr->MakeGameInstance(QmObjType(VolumnBG))->As<QcVolCtrl*>();
	m_pVolumnCtrl->SetPosition(QcPoint(10.f,0.f));
}

void QcSceneMgr::StartGame()
{
#ifdef QmTestVersion
	QmDebugHelper.StartGame();
#endif

	for (int i=0; i<QmScreenSceneCount; ++i)
	{
		QmAutoRevertInit(s32, giScreenHandleing, i);
		GetBgMgr()->NextScene(true);
	}

	SetTimer(eGameRunningTime, 60.f);
	SetTimer(eSceneCheckErrorTimer, 1.f);

	if (QmSysSetting->GetDaMaFlag() != eDaMaNone)
	{
		QmLog("enter setting from DaMaFlag:%d", QmSysSetting->GetDaMaFlag());
		SendEvent(eEvtSceneStateChange, QcSceneMgr::eSceneSettting);
	}
	else
	{
		for (int i=0; i<QmScreenSceneCount; ++i)
		{
			QmAutoRevertInit(s32, giScreenHandleing, i);
			SendEvent(eEvtSceneStateChange, QcSceneMgr::eSceneNormal);
		}
	}
}

void QcSceneMgr::OnKeyDown(u32 key, const QsKeyInfo& info)
{
	if (m_eSceneState == eSceneSettting)
	{
		m_pSettingMgr->OnKeyDown(key, info);
	}
	else
	{
		if (key == eSmallKeyDown)
		{
			m_pQuickAccount->SetVisible(!m_pQuickAccount->IsVisible());
			m_pQuickAccount->SetShowWords(false);
		}
		if (key == eSmallKeyOk && info.fKeyDownTime > 1.0f && m_eSceneState != eSceneSetttingReady)
		{
			m_pReadySetting->SetVisible(true);
			m_eSceneState = eSceneSetttingReady;
			info.bOnlyOneDown = true;
			info.bIngoreUp = true;
		}
		if (!m_pDaMaErrorNode->IsVisible() && !m_pCardErrorNode->IsVisible())
			GetPlayerMgr()->OnKeyDown(key, info);
		m_pVolumnCtrl->OnKeyDown(key, info);
	}
}

void QcSceneMgr::OnKeyUp(u32 key, const QsKeyInfo& info)
{
	if (m_eSceneState == eSceneSettting)
	{
		m_pSettingMgr->OnKeyUp(key, info);
	}
	else
	{
		if (key == eSmallKeyUp)
		{
			m_pQuickAccount->SetShowWords(true);
			m_pQuickAccount->SetVisible(!m_pQuickAccount->IsVisible());
		}
		if (!m_pDaMaErrorNode->IsVisible() && !m_pCardErrorNode->IsVisible())
			GetPlayerMgr()->OnKeyUp(key, info);
	}
}

void QcSceneMgr::OnEvent(const QsEvent& evt)
{
	switch(evt.m_iEvt)
	{
	case eEvtSceneStateChange:
		{
			switch(evt.m_iParam)
			{
			case eSceneNormal:
				{
					m_eSceneState = eSceneNormal;
					SetTimer(eSceneTimer, QmSysSetting->GetSceneTime(), true);
					GetFishMgr()->Reset(true);
					break;
				}
			case eSceneEnd:
				{
					if (m_eSceneState == eSceneNormal)
						m_eSceneState = eSceneEnd;
					break;
				}
			case eSceneLogoAnimation:
				{
					m_eSceneState = eSceneLogoAnimation;
					for (int i=0; i<QmScreenSceneCount; ++i)
					{
						QmAutoRevertInit(s32, giScreenHandleing, i);
						GetBgMgr()->StartLogoAnimation();
					}
					m_bSceneStateTimerOn = false;
					break;
				}
			case eSceneRegular:
				{
					m_eSceneState = eSceneRegular;

					QcObjType objType = ID_NULL;
					for (int i=0; i<QmScreenSceneCount; ++i)
					{
						QmAutoRevertInit(s32, giScreenHandleing, i);
						GetBgMgr()->NextScene();
						GetAnimationMgr()->Clear();
						GetFishMgr()->Reset(false);
						objType = GetFishMgr()->BeginSceneRegular(objType);
					}
					break;
				}
			case eSceneSettting:
				{
					m_eSceneState = eSceneSettting;
					QmTimerMgr->ClearAllTimer();

					SetTimer(eGameRunningTime, 60.f);
					SetTimer(eSceneCheckErrorTimer, 1.f);

					m_bSceneStateTimerOn = false;
					m_pSettingMgr->EnterSetting();
					break;
				}
			case eSceneSettingEnd:
				{
					m_pSettingMgr->ExitSetting();
					//PostMessage(NULL, WM_QUIT, 0, 0);
					giExitFlag = 1;
					m_eSceneState = eSceneSettingEnd;
					break;
				}
			}	
			break;
		}
	}
};

void QcSceneMgr::OnTimer(u32 nTimer, void* pContext)
{
	switch(nTimer)
	{
	case eSceneStateTimer:
		{
			if (m_eSceneState == eSceneEnd)
				PostEvent(eEvtSceneStateChange, eSceneLogoAnimation);
			else
			{
				QmLog("Enter Setting from QcSceneMgr::OnTimer:%d", m_eSceneState);
				PostEvent(eEvtSceneStateChange, eSceneSettting);
			}
			break;
		}
	//换场景
	case eSceneTimer:
		{
			if (m_eSceneState == eSceneNormal)
				PostEvent(eEvtSceneStateChange, eSceneEnd);
			break;
		}
	case eGameRunningTime:
		{
			QmSysSetting->SetRunningTime(QmSysSetting->GetRunningTime() + 1);
			break;
		}
	case eSceneCheckErrorTimer:
		{
			bool bNeedDaMa = false;
			//打码时间到
			if (QmSysSetting->SpanLeaveTime(1))
			{
				m_pDaMaErrorNode->SetVisible(true);
				bNeedDaMa = true;
			}
#ifdef QmIOUsbOn
			bool bError = false;
			m_pCardErrorNode->SetVisible(false);	
			m_pIO1ConnectErrorNode->SetVisible(false);
			m_pDaMaErrorNode->SetVisible(false);
			if (m_pIO2ConnectErrorNode)
				m_pIO2ConnectErrorNode->SetVisible(false);

			int errorCode = GetCardErrorCode();
			if ( (errorCode == DeviceNeetToReboot || errorCode == DeviceLose) && InitCom())
			{
				errorCode = GetCardErrorCode();
				if (errorCode == Normal)
				{
					QmFishAlg->Init();
				}
			}
			if (errorCode)
			{
				bError = true;
				if (!m_pCardErrorNode->IsVisible())
					QmLog("Card Error: %d", errorCode);
				m_pCardErrorNode->GetDigitalNode().SetValue(errorCode);
				m_pCardErrorNode->SetVisible(true);	
			}
			else if (bNeedDaMa)
			{	
				bError = true;
				m_pDaMaErrorNode->SetVisible(true);
			}
			else
			{
				if (!IO1IsConnected())
				{	
					bError = true;
					m_pIO1ConnectErrorNode->SetVisible(true);
				}
				if (!IO2IsConnected() && m_pIO2ConnectErrorNode)
				{
					bError = true;
					m_pIO2ConnectErrorNode->SetVisible(true);
				}
			}
			if (bError)
			{
				TurnOffAutoFire();
			}
#endif
			break;
		}
	default:
		{
			QcObjectEx::OnTimer(nTimer, pContext);
			break;
		}
	}
}

void QcSceneMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
	m_pQuickAccount->Animate(dCurrentTime,fElapsedTime);

	for (int i=0; i<QmScreenSceneCount; ++i)
	{
		QmAutoRevertInit(s32, giScreenHandleing, i);
		GetBgMgr()->Animate(dCurrentTime, fElapsedTime);
	}
	if (eSceneSettting == m_eSceneState)
	{
		m_pSettingMgr->Animate(dCurrentTime, fElapsedTime);
	}
	else
	{
		for (int i=0; i<QmScreenSceneCount; ++i)
		{
			QmAutoRevertInit(s32, giScreenHandleing, i);
			GetFishMgr()->Animate(dCurrentTime, fElapsedTime);
			GetBulletMgr()->Animate(dCurrentTime, fElapsedTime);
			GetAnimationMgr()->Animate(dCurrentTime, fElapsedTime);
		}
		GetPlayerMgr()->Animate(dCurrentTime, fElapsedTime);
		m_pVolumnCtrl->Animate(dCurrentTime,fElapsedTime);
	}
	QmSoundMgr->Update();
}

void QcSceneMgr::HandleCollision()
{
	for (int i=0; i<QmScreenSceneCount; ++i)
	{
		QmAutoRevertInit(s32, giScreenHandleing, i);
		QcBulletPtrList& bulletList = GetBulletMgr()->GetBulletList();
		GetFishMgr()->HandleCollision(bulletList);
	}
}

void QcSceneMgr::Render(QcDeviceRender& render)
{
	bool bLogoAnimationFinished = true;

	int iRenderTime = IsUionScreenMode() ? 2 : 1;
	for (int i=0; i<QmScreenSceneCount; ++i)
	{
		QmAutoRevertInit(s32, giScreenHandleing, i);
		if (IsTwoScreenMode())
			render.SwitchToSubScreen(i);

		GetBgMgr()->Render(render);
		if (eSceneSettting == m_eSceneState)
		{
			m_pSettingMgr->Render(render, iRenderTime);
		}
		else
		{
			QcSceneAnimationMgr* pAnimationMgr = GetAnimationMgr();
			pAnimationMgr->Render(render, eAnimationLayerBubble);
			GetFishMgr()->Render(render, 0);
			GetBgMgr()->RenderBgSceneNode(render);

			pAnimationMgr->Render(render, eAnimationLayerSpecial);
			GetFishMgr()->Render(render, 1);
			pAnimationMgr->Render(render, eAnimationLayerNet);
			if (m_eSceneState != eSceneLogoAnimation)
				GetBgMgr()->RenderWave(render);

			GetBulletMgr()->Render(render);
			GetFishMgr()->Render(render, 2);
			GetPlayerMgr()->Render(render, 4);
			GetFishMgr()->Render(render, 3);
			pAnimationMgr->Render(render, eAnimationLayerDyingFish);
			pAnimationMgr->Render(render, eAnimationLayerScore);
			pAnimationMgr->Render(render, eAnimationLayerNormal);
			pAnimationMgr->Render(render, eAnimationLayerEffect);
			switch(m_eSceneState)
			{
			case eSceneLogoAnimation:
				{
					bLogoAnimationFinished &= GetBgMgr()->RenderLogoAnimation(render);
					break;
				}
			}
			GetPlayerMgr()->Render(render, 0);
			GetPlayerMgr()->Render(render, 1);
			GetPlayerMgr()->Render(render, 2);
			pAnimationMgr->Render(render, eAnimationLayerGunEffect);

			for (int j=0; j<iRenderTime; ++j)
			{
				if (IsUionScreenMode())
					render.SwitchToSubScreen(j);
#ifdef QmTestVersion
				if (QmDebugHelper.IsShowIOError())
#endif
				{
					if (m_pCardErrorNode->IsVisible())
					{
						m_pCardErrorNode->Render(render);
					}
					else if (m_pDaMaErrorNode->IsVisible())
					{
						m_pErrorBg->Render(render);
						m_pDaMaErrorNode->Render(render);
					}
					else
					{
						if ((i + j) == eScreen1 || NULL==m_pIO2ConnectErrorNode)
						{
							if (m_pIO1ConnectErrorNode->IsVisible())
							{
								m_pErrorBg->Render(render);
								m_pIO1ConnectErrorNode->Render(render);
							}
						}
						else if (m_pIO2ConnectErrorNode && m_pIO2ConnectErrorNode->IsVisible())
						{
							m_pErrorBg->Render(render);
							m_pIO2ConnectErrorNode->Render(render);
						}
					}	
				}
				m_pQuickAccount->Render(render);
				m_pVolumnCtrl->Render(render);
				m_pReadySetting->Render(render);
			}
			if (IsUionScreenMode())
				render.SwitchToSubScreen(-1);
		}
	}
	render.SwitchToSubScreen(-1);

	if (eSceneEnd == m_eSceneState || eSceneSetttingReady == m_eSceneState)
	{
		if (!IsLockNormalScene() && !m_bSceneStateTimerOn)
		{
			if (!GetPlayerMgr()->IsLockNormalScene())
			{
				SetTimer(eSceneStateTimer, 1.5f, true);
				m_bSceneStateTimerOn = true;
			}
		}
	}
	else if (bLogoAnimationFinished && m_eSceneState == eSceneLogoAnimation)
		PostEvent(eEvtSceneStateChange, eSceneRegular);
}

u32 QcSceneMgr::GetSceneId()
{
	return GetBgMgr()->GetSenceId();
}

void QcSceneMgr::TurnOffAutoFire()
{
	QmEachPlayerCall(GetPlayerMgr(), TrunOffAutoFire());
}