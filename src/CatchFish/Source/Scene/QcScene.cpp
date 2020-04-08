#include "pch.hpp"
#include "QcScene.hpp"
#include "QcSceneStartMgr.hpp"
#include "QcSceneMgr.hpp"
#include "QcSceneAnimationMgr.hpp"
#include "QcBackGroundMgr.hpp"
#include "Fish\QcFishMgr.hpp"
#include "QcPlayerMgr.hpp"
#include "QcSceneNode.hpp"
#include "Res\QcResManager.hpp"
#include "FileStorageSystem\QcFileStorageMgr.hpp"
#include "FileStorageSystem\QcPlayerInforList.hpp"
#include "FileStorageSystem\QcSystemSetting.hpp"


QcScene::QcScene()
{
	m_pGameFileMgr.Set(new QcFileStorageMgr());
	m_pGameFileMgr->RegistryFileForSave(QmSysSetting, QmSysSettingFile);
}

QcScene::~QcScene()
{
	m_pStartMgr = NULL;
	m_pSceneMgr = NULL;
	m_pResMgr = NULL;
	m_pGameFileMgr = NULL;
}

bool QcScene::InitApp(QcAppConfig& appConfig)
{
	if (QmSysSetting->IsDirty())
		m_pGameFileMgr->SaveFile(QmSysSettingFile);
	return true;
}

bool QcScene::ExitApp()
{
	return true;
}

bool QcScene::OnCreateDevice(IDirect3DDevice9* pd3dDevice)
{
	bool bRet = m_render.Create(pd3dDevice);
	if (bRet)
	{
		m_pResMgr = new QcResManager(m_render);
		m_pResMgr->Init();
	}
	return bRet;
}

bool QcScene::LoadResouce()
{
	m_pResMgr->LoadParticle(m_render);
	m_pStartMgr = new QcSceneStartMgr(m_pResMgr);
	//加载开机画面资源
	m_pStartMgr->Init();
	//启动线程加载资源
	m_pStartMgr->Create(false);

	return true;
}

bool QcScene::OnDestroyDevice()
{
	return m_render.OnDestroy();
}

bool QcScene::OnResetDevice(IDirect3DDevice9* pd3dDevice)
{
	if (pd3dDevice == m_render.GetDevice())
		m_render.Reset();
	return true;
}

bool QcScene::OnLostDevice()
{
	return m_render.OnLost();
}

bool QcScene::InitScene()
{
#ifdef QmIOUsbOn
	if (!InitCom())
		QmLogError("InitCom");
#endif
	m_pSceneMgr = new QcSceneMgr();

	QmProfilerLoading(eInitSceneTime);
	m_pSceneMgr->Init(m_pResMgr);

	GetGameFileMgr()->RegistryFileForSave(QmFishAlg, QmAlgorithmFile);
	GetGameFileMgr()->ReadFile(QmAlgorithmFile);
	QmFishAlg->Init();


	GetGameFileMgr()->RegistryFileForSave(m_pSceneMgr->GetPlayerMgr()->GetPlayerInfoList(), QmPlayerInfoFile);
	GetGameFileMgr()->ReadFile(QmPlayerInfoFile);
	m_pSceneMgr->GetPlayerMgr()->Update();

	//启动文件线程
	GetGameFileMgr()->Create(false);
#ifdef QmLogOn
	GetInstance<QcLogEngine>()->Resume();
#endif
	QmSysSetting->RepairLeaveTime();
	return true;
}

void QcScene::OnKeyDown(u32 key, const QsKeyInfo& info)
{
	if (m_pSceneMgr)
		m_pSceneMgr->OnKeyDown(key, info);
}
void QcScene::OnKeyUp(u32 key, const QsKeyInfo& info)
{
    if (m_pSceneMgr)
        m_pSceneMgr->OnKeyUp(key, info);
}

bool QcScene::StartFrame(f64 fTime, f32 fElapsedTime)
{
	RecoveryNode();
	if (m_pStartMgr->IsFinished())
	{
		static unsigned __int64 giCount = 0;
		if (giCount == 0)
		{
			++giCount;
			m_pSceneMgr->StartGame();
			QmGlobalData.goApp->FinishLoading();
			m_pResMgr->FinishLoading();
		}
		QmTimerMgr->HandleTimer(fTime, fElapsedTime);
		QmProfileFragment(OnFrameMove);
		m_pSceneMgr->Animate(fTime, fElapsedTime);
		HandleCollision();
	}
	else
	{
		QmTimerMgr->HandleTimer(fTime, fElapsedTime);
		m_pStartMgr->Animate(fTime, fElapsedTime);
	}
	return true;
}

void QcScene::HandleCollision()
{
	m_pSceneMgr->HandleCollision();
}

bool QcScene::Draw(float fElapsedTime)
{
	if (m_pStartMgr->IsFinished())
	{
		m_pSceneMgr->Render(m_render);
	}
	else
	{
		m_pStartMgr->Render(m_render);
	}
#ifdef QmTestVersion
    if (QmDebugHelper.IsStartCaptureScreen())
    {
        static int tmp = 0;
        char f[100] = {0};
        sprintf_s(f, "C:\\Capture%.3d.png", tmp++);
        m_render.ScreenCapture(f);
    }
	QmDebugHelper.Render(m_render, fElapsedTime);
#endif
	return true;
}