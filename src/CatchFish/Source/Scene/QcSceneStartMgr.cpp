#include "pch.hpp"
#include "QcSceneStartMgr.hpp"
#include "QcSceneAnimation.hpp"
#include "QcScene.hpp"
#include "Res\QcResManager.hpp"
#include "Res\QcResDesc.hpp"
#include "Res\QcPathMgr.hpp"
#include "Res\QcResLoader.hpp"
#include "Res\QcResPlayerLoader.hpp"
#include "Thread\QcLock.hpp"
#include "Regular\QcRegularMgr.hpp"
#include "Animator\QcAnimatorTexture.hpp"
static QcString m_sLoading;

class QcSceneStartLoadingNode : public QcSceneAnimation
{
public:
	QcSceneStartLoadingNode() : m_nCount(0){}
	
	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false)
	{
		QcSceneAnimation::Init(desc, bLoadSubNode);
		QcResManager* pResMgr = QmResMgr;

		QcResImageDesc* pDesc = const_cast<QcResImageDesc*>(pResMgr->GetImageDesc(QmObjName(LoadingPoint)));
		f32 fWidth = 10.f;
		s32 nPoint = QcMath::ceil32(QmGlobalData.gScreenRect.getWidth()/fWidth);
		QcPoint pt(QmGlobalData.gScreenRect.left() + pDesc->GetWidth() * 0.5f, QmGlobalData.gScreenRect.bottom() + pDesc->GetHeight());
		for (int i=0; i<nPoint; ++i)
		{
			QcSceneNode* pNode  = pDesc->MakeInstance();
			pNode->SetPosition(pt);
			pt.X += fWidth;
			m_pLoadingPointList.PushBack(pNode);
		}
		m_pLoadingText = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(loading));
		m_pLoadingText->SetPosition(QcPoint(0.f, QmGlobalData.gScreenRect.bottom() + 100.f));

		m_fTimeCount = 0.f;
	}
	virtual void Animate(double dCurrentTime,f32 fElapsedTime)
	{
		QcSceneAnimation::Animate(dCurrentTime, fElapsedTime);
		m_pLoadingText->Animate(dCurrentTime, fElapsedTime);
		
		if (m_nCount < m_pLoadingPointList.size())
		{
			m_bFinished = false;
			m_fTimeCount-= fElapsedTime;
			if (m_fTimeCount < 0.f)
			{
				m_fTimeCount = m_fBackupTime;
				++m_nCount;
			}
		}
		else
			m_bFinished = true;

		m_pLoadingPointList.Animate(dCurrentTime, fElapsedTime);
	}
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL)
	{
		QcSceneAnimation::RenderSub(render, pWorldMatrix);
		m_pLoadingText->Render(render, pWorldMatrix);
		for (u32 i=0; i<m_nCount; ++i)
		{
			m_pLoadingPointList[i]->Render(render, pWorldMatrix);
		}
	}
	virtual void SetLivingTime(f32 fLivingTime)
	{
		m_fTimeCount = fLivingTime/m_pLoadingPointList.size();
		m_fBackupTime = m_fTimeCount;
		//QcSceneAnimation::SetLivingTime(fLivingTime);
	}
protected:
	f32 m_fTimeCount;
	f32 m_fBackupTime;
	u32 m_nCount;
	QcSceneNodePtrList m_pLoadingPointList;
	QcSceneNodePtr m_pLoadingText;
};

class QcStartSysInforNode : public QcSceneAnimation
{
public:
	QcStartSysInforNode(){}

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false)
	{
		QcSceneAnimation::Init(desc, bLoadSubNode);
		QcResManager* pResMgr = QmResMgr;

		QcSceneNode* pNode = NULL;;
		QcDigitalNode* pDigitalNode = NULL;
		const QcResImageDesc* pDigitalDesc = pResMgr->GetImageDesc(QmObjName(StartDigital));

 		pNode = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(StartLine));
 		AddSubSceneNode(pNode);

		 pNode = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(StartSysInfor));
		 pNode->SetPosition(QcPoint(0.f, 300.f));
		AddSubSceneNode(pNode);

		//线号.3
		pNode = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(StartLineLabel));
		pNode->SetPosition(QcPoint(-260.f, 140.f));
		AddSubSceneNode(pNode);
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetAlign(eAlignLeftCenter);
		pDigitalNode->SetPosition(QcPoint(80.f, 140.f));
		pDigitalNode->SetValue(QmSysSetting->GetXianHao(), 3);
		AddSubSceneNode(pDigitalNode);

		//机台号
		pNode = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(StartMatchineIdLabel));
		pNode->SetPosition(QcPoint(-238.f, 60.f));
		AddSubSceneNode(pNode);
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetAlign(eAlignLeftCenter);
		pDigitalNode->SetPosition(QcPoint(80.f, 60.f));
		pDigitalNode->SetValue(QmSysSetting->GetJiTaiHao(), 8);
		AddSubSceneNode(pDigitalNode);

		//版本号
		pNode = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(StartVersionLabel));
		pNode->SetPosition(QcPoint(-235.f, -20.f));
		AddSubSceneNode(pNode);
		//V
		pNode = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(StartV));
		pNode->SetPosition(QcPoint(110.f, -20.f));
		AddSubSceneNode(pNode);
		//主版本号
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetAlign(eAlignLeftCenter);
		pDigitalNode->SetPosition(QcPoint(137.f, -20.f));
		pDigitalNode->SetValue(QmSysSetting->GetBanBenHao1());
		AddSubSceneNode(pDigitalNode);
		//-
		pNode = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(StartMinius));
		pNode->SetPosition(QcPoint(275.f, -20.f));
		AddSubSceneNode(pNode);
		//副版本号
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetAlign(eAlignLeftCenter);
		pDigitalNode->SetPosition(QcPoint(300.f, -20.f));
		pDigitalNode->SetValue(QmSysSetting->GetBanBenHao2());
		AddSubSceneNode(pDigitalNode);

		//系统时间
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		pNode = QfMakeSceneNodeInstance<QcSceneNode>(QmObjName(StartTimeFormat));
		pNode->SetPosition(QcPoint(-45.f, -140.f));
		AddSubSceneNode(pNode);
		//Year
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetPosition(QcPoint(-450.f, -140.f));
		pDigitalNode->SetValue(sysTime.wYear);
		AddSubSceneNode(pDigitalNode);
		//month
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetPosition(QcPoint(-265.f, -140.f));
		pDigitalNode->SetValue(sysTime.wMonth);
		AddSubSceneNode(pDigitalNode);
		//day
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetPosition(QcPoint(-130.f, -140.f));
		pDigitalNode->SetValue(sysTime.wDay);
		AddSubSceneNode(pDigitalNode);
		//hour
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetPosition(QcPoint(10.f, -140.f));
		pDigitalNode->SetValue(sysTime.wHour);
		AddSubSceneNode(pDigitalNode);
		//miniue
		pDigitalNode = QfMakeSceneNodeInstance<QcDigitalNode>(pDigitalDesc);
		pDigitalNode->SetPosition(QcPoint(165.f, -140.f));
		pDigitalNode->SetValue(sysTime.wMinute);
		AddSubSceneNode(pDigitalNode);
		//week
		QcResImageDesc* pImageList = pResMgr->GetImageDesc(QmObjName(StartWeek));
		pNode = QfMakeSceneNodeInstance<QcSceneNode>(pImageList->GetSubDescList()[sysTime.wDayOfWeek]);
		pNode->SetPosition(QcPoint(380.f, -140.f));
		AddSubSceneNode(pNode);
	}
};

//////////////////////////////////////////////////////////////////////////
class QcStartLogoLoadingNode : public QcSceneAnimation
{
public:
	QcStartLogoLoadingNode() : m_nCount(0), m_MyTime(0){}
	virtual void SetLivingTime(f32 fLivingTime)
	{
		m_fTimeCount = fLivingTime/100.f;
		m_fBackupTime = m_fTimeCount;
	}
	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false)
	{
		f32 fScreenWidth = QmGlobalData.gScreenRect.getWidth();
		f32 fScreenHeight = QmGlobalData.gScreenRect.getHeight();
		if (QmSysSetting->IsUionScreenMode())
		{
			fScreenWidth /= 2.f;
		}
		QcSceneAnimation::Init(desc, bLoadSubNode);
		QcResManager* pResMgr = QmResMgr;
		QcResImageDesc* pDesc = const_cast<QcResImageDesc*>(pResMgr->GetImageDesc(QmObjName(StartLogo)));
		m_pLogoLoading = QfMakeSceneNodeInstance<QcSceneAnimation>(pDesc);
		m_pLogoLoading->ScaleTo(fScreenWidth, fScreenHeight);
		//m_pLogoLoading->FindAnimator<QcAnimatorTexture>()->SetLoop(false);

		//进度条;
		pDesc = const_cast<QcResImageDesc*>(pResMgr->GetImageDesc(QmObjName(LoadingText)));
		m_pLoadingText = QfMakeSceneNodeInstance<QcSceneAnimation>(pDesc);
		//QcPoint pt(QmGlobalData.gScreenRect.left() + pDesc->GetWidth() * 0.5f + (QmGlobalData.gScreenRect.getWidth() *0.8f), QmGlobalData.gScreenRect.bottom() + pDesc->GetHeight() * 2.f);
		QcPoint pt(610.f,-430.f);
		m_pLoadingText->SetPosition(pt);
		m_pLoadingText->SetScale(0.8f);
		//m_pLoadingText->FindAnimator<QcAnimatorTexture>()->Reset(15);

		m_pNumAnimation = QfMakeEmptyInstance<QcSceneAnimation>();
		//m_pNumAnimation->SetDigitalType(QmObjName(loadingDigital));
		QmSetDigitalType(m_pNumAnimation->GetDigitalNode(), loadingDigital);
		char buf[256];
		sprintf_s(buf,"%d%%",m_nCount);
		m_pNumAnimation->GetDigitalNode().SetValue((BYTE*)buf, strlen(buf));
		//m_pNumAnimation->SetDigitalValue(m_nCount);

		m_pNumAnimation->SetDigitalVisible(true);
		QcPoint NumPos = pt;
		//NumPos += QcPoint(50.f,0.f);
		m_pNumAnimation->SetPosition(NumPos);
		m_pNumAnimation->SetScale(0.8f);
	}
	virtual void Animate(double dCurrentTime,f32 fElapsedTime)
	{
		QcSceneAnimation::Animate(dCurrentTime,fElapsedTime);
		m_pLogoLoading->Animate(dCurrentTime,fElapsedTime);
		m_pNumAnimation->Animate(dCurrentTime,fElapsedTime);
		m_pLoadingText->Animate(dCurrentTime,fElapsedTime);
		
		if (m_nCount < 100)
		{
			m_bFinished = false;
			m_fTimeCount += fElapsedTime;
			//////////////////////////////////////////////////////////////////////////
			if (m_pLoadingText->IsFinished() && m_nCount == 99)
			{
				m_nCount = 100;
				char buf[256];
				sprintf_s(buf,"%d%%",m_nCount);
				m_pNumAnimation->GetDigitalNode().SetValue((BYTE*)buf, strlen(buf));
			}
			//////////////////////////////////////////////////////////////////////////

			if(m_pLoadingText->IsFinished() && m_nCount != 100)
			{
				QiAnimator* temp = QmAnimatorCreator->MakeTextureAnimator(m_pLoadingText->GetDesc()->GetStateParameter());
				m_pLoadingText->AddAnimator(temp);
			}
			if (m_fTimeCount > m_fBackupTime && m_nCount < 99)
			{
				m_fTimeCount -= m_fBackupTime;
				++m_nCount;

				char buf[256];
				sprintf_s(buf,"%d%%",m_nCount);
				m_pNumAnimation->GetDigitalNode().SetValue((BYTE*)buf, strlen(buf));
			}
 		}
		else
		{
			if(m_pLoadingText->IsFinished())
			{
				m_MyTime += fElapsedTime;
				m_bFinished = m_MyTime > 1.5f;
			}
		}
	}
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL)
	{
		QcSceneAnimation::RenderSub(render, pWorldMatrix);
		m_pLogoLoading->Render(render,pWorldMatrix);
		m_pLoadingText->Render(render,pWorldMatrix);
		m_pNumAnimation->Render(render,pWorldMatrix);
	}
	
protected:
	f32 m_MyLivingTime;
	f32 m_MyTime;
	f32 m_fTimeCount;
	f32 m_fBackupTime;
	u32 m_nCount;				//用于显示百分比;
	QcSceneNodePtr m_pLogoLoading;
	QcSceneAnimationPtr m_pLoadingText;
	QcSceneAnimationPtr m_pNumAnimation;
};
//////////////////////////////////////////////////////////////////////////

QcSceneStartMgr::QcSceneStartMgr(QcResManager* pResMgr)
: m_pResMgr(pResMgr)
, m_bIsFinished(false)
{
}

bool QcSceneStartMgr::Init()
{
	m_pResMgr->SetLoadingFlag(QcResManager::eProcessBarRes);
	LoadProgressResource(m_pResMgr);

	QcSceneAnimation* pAnimation = NULL;

	f32 fScreenWidth = QmGlobalData.gScreenRect.getWidth();
	f32 fScreenHeight = QmGlobalData.gScreenRect.getHeight();
	if (QmSysSetting->IsUionScreenMode())
	{
		fScreenWidth /= 2.f;
	}

	pAnimation = QfMakeSceneNodeInstance<QcSceneAnimation>(QmObjType(warning_ch));
	pAnimation->ScaleTo(fScreenWidth, fScreenHeight);
	pAnimation->SetLivingTime(6.f);
	m_arAnimationList.AddAnimation(QcSceneAnimationPtr(pAnimation));

	pAnimation = QfMakeSceneNodeInstance<QcSceneAnimation>(QmObjType(warning_en));
	pAnimation->ScaleTo(fScreenWidth, fScreenHeight);
	pAnimation->SetLivingTime(6.f);
	m_arAnimationList.AddAnimation(QcSceneAnimationPtr(pAnimation));
// 
 	pAnimation = QfMakeSceneNodeInstance<QcStartSysInforNode>(m_pResMgr->GetEmptyImageDesc());
 	pAnimation->SetLivingTime(6.f);
 	m_arAnimationList.AddAnimation(QcSceneAnimationPtr(pAnimation));

	//记录最后一个动画，防止动画播放完后资源还未加载完, 期间会出现空白帧
	m_pLastAnimation = m_arAnimationList.GetAnimationDeque().back();

	DXUTGetGlobalTimer()->Reset();
	return true;
}

bool QcSceneStartMgr::LoadProgressResource(QcResManager* pResManager)
{
	QmProfilerLoading(eAllLoadingTime);
	QcResLoader resLoad(pResManager);
	QmAutoRevertInit(s32, QmGlobalData.giForceTexInVideoMem, -1);
	resLoad.LoadFromFile("ResProgressbar.xml");
	return true;
}

int QcSceneStartMgr::Run()
{
	QmProfilerLoading(eAllLoadingTime);
	m_pResMgr->SetLoadingFlag(QcResManager::eSceneRes);
	LoadSceneResource(m_pResMgr);
	QmSence->InitScene();

	m_bIsFinished = true;
	return 0;
}

bool QcSceneStartMgr::LoadSceneResource(QcResManager* pResManager)
{
	QcResLoader resLoad(pResManager, this);
	{
		QmProfilerLoading(eLoadingSoundTime);
		QmSoundMgr->Init(DXUTGetHWND(), "ResSound.xml");
	}
	{
		QmProfilerLoading(eLoadingPathTime);
		pResManager->GetPathMgr()->LoadFromFile(QmSysSetting->IsUionScreenMode() ? "ResPathUionScreen.xml" : "ResPath.xml");
	}
	{
		QmProfilerLoading(eLoadingBg);
		resLoad.LoadFromFile("ResBackGound.xml");
	}
	{
		QmProfilerLoading(eLoadingNormalImage);
		resLoad.LoadFromFile("ResImage.xml");
		resLoad.LoadFromFile("ResDigital.xml");
	}
	{
		QmProfilerLoading(eLoadingFish);
		resLoad.LoadFromFile("ResFish.xml");
		//resLoad.LoadFromFile("ResSpecialFish.xml");
		pResManager->GetRegularMgr()->LoadFromFile("ResFishRegular.xml");
	}
	{
		QmProfilerLoading(eLoadingPlayer);
		resLoad.LoadFromFile("ResGun.xml");
		resLoad.LoadFromFile("ResBullet.xml");
		resLoad.LoadFromFile("ResPlayer.xml");
	}
	{
		QmProfilerLoading(eLoadingSetting);
		QmAutoRevertInit(s32, QmGlobalData.giForceTexInVideoMem, -1);
		resLoad.LoadFromFile("ResSetting.xml");
	}
	return true;
}



void QcSceneStartMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
	if (m_arAnimationList.Size())
		m_arAnimationList.Animate(dCurrentTime, fElapsedTime);
	else
		m_pLastAnimation->Animate(dCurrentTime, fElapsedTime);
}

void QcSceneStartMgr::Render(QcDeviceRender& render)
{
	int nCount = 1;
	if (QmSysSetting->IsUionScreenMode() || QmSysSetting->IsTwoScreenMode())
	{
		nCount = QmMaxScreen;
	}
	for (int i=0; i<nCount; ++i)
	{
		QmAutoRevertInit(s32, giScreenHandleing, i);
		render.SwitchToSubScreen(i);

		if (m_arAnimationList.Size())
		{
			m_arAnimationList.Render(render);
			if (m_arAnimationList.Size() == 0)
			{
				m_pLastAnimation->Reset();
				m_pLastAnimation->SetState(eLiving);
			}
		}
		else
		{
			m_pLastAnimation->Render(render);
		}
	}

#ifdef QmTestVersion
	render.RenderText(m_sLoading.c_str(), QfMakeScreenRect(0.f, 0.f, 500, 200));
#endif
}

void QcSceneStartMgr::CallBack(const QcResDesc* pDesc)
{
#ifdef QmTestVersion
	m_sLoading = pDesc->GetObjType();
#endif
}


