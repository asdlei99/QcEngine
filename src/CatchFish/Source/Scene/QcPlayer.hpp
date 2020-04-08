#pragma once

#include "Scene\QcSceneNode.hpp"
#include "Scene\QcGunMgr.hpp"
#include "Scene\QcGoldColumnMgr.hpp"
#include "Scene\QcPlayerScoreHelper.hpp"
#include "Scene\QcSceneTrackFishHelper.hpp"
#include "Scene/QcFnKeyHelper.hpp"

class QcPlayerConfig;
class QcPlayerInfo;
class QcPlayerScoreHelper;
class QcScopeBombFish;

class QcPlayer : public QcSceneNode
{
	friend class QcDebugHelper;
	DeclareRuntimeClass(QcPlayer);
public:
	QcPlayer();
	~QcPlayer();

	void Update(int nID, QcPlayerInfo* pInfo);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render, u32 iLayer);
	
	void OnKeyDown(QeKey key, const QsKeyInfo& info);
	void OnKeyUp(QeKey key, const QsKeyInfo& info);
	bool OnFire();
	void UpdateScore();
	void HitFish(QcBullet* pBullet, QcFish* pFish);
	void HitFishNormal(QcBullet* pBullet, QcFish* pFish);
	void HitFishSpecial(QcBullet* pBullet, QcFish* pFish);
	void HitAddScore(u32 iScore, u32 iCaiJinScore, f32 fShowTime = 3.0f);
	void GainSuperGun(const QcPoint& pos);
	void ShowCaiJin(u32 iScore, f32 fShowTime);

	QcFish* GetLockFish() {return GetTrackHelper().GetLockFish();}
	void CancelLockFish() { GetTrackHelper().LockFish(NULL); }
	u32 GetID() const {return m_nID;}
	u32 GetBulletNum() const {return m_nCurrentBullet;}
	const QcVector& GetVertical() { return m_vVec; }
	const QcPoint& GetAbsCoinEndPt() const {return m_vAbsCoinEndPt;}
	const QcPoint& GetAbsGunPos() const {return m_vAbsGunPos; }
	QcGunMgr& GetGunMgr(){return m_oGunMgr;}
	QcGoldColumnMgr& GetGoldMgr() {return m_oGoldCoinMgr;}
	QcPlayerInfo* GetInfo() {return m_pPlayerInfo;}
	const QcColor& GetColorFlag() const {return m_colorFlag;}
	QcSceneTrackFishHelper& GetTrackHelper() {return m_oTrackHelper; }
	//自动发炮
	void AutoFire();								//自动发炮函数
	void TrunOffAutoFire()
	{ 
		m_bPlayerAutoFire = false;
		if(m_pAutoFireLogo)
			m_pAutoFireLogo->SetVisible(false);
	}
	//end
	s32 GetScreenID() const {return m_giScreenID;}
protected:
	void FinishAnimator(u32 iParam);
protected:
	virtual void OnTimer(u32 nTimer, void* pContext);
    virtual void OnEvent(const QsEvent& evt);
	QcPlayerScoreHelper& GetScoreHelper() {return m_oScoreHelper; }
    void BomberAutoExplode();
protected:
	s32 m_giScreenID;
	u32 m_nID;
	u32 m_nCurrentBullet;

	QcColor m_colorFlag;
	QcVector m_vVec;
	QcPoint m_vAbsCoinEndPt;
	QcPoint m_vAbsGunPos;
	QcPoint m_vAbsFishFlagPos;
	QcPoint m_vAbsTrackHelperPos;
	QcPoint m_vCaiJinPos;
	QcGunMgr m_oGunMgr;
	QcGoldColumnMgr m_oGoldCoinMgr;
	QcSceneTrackFishHelper m_oTrackHelper;
	QcPlayerScoreHelper m_oScoreHelper;
	QcSharedPtr<QcSceneDigital> m_pScoreFrame;
	QcSceneNodePtr m_pBaoJiNode;

	QcSceneNodePtr m_pSuperGunMovingFlag; //获得粒子炮动画
	QcSharedPtr<QcSceneAnimation> m_pCaiJinScoreAnimation;
	QcSceneNodePtr m_pSceneNodeQuan;
	QcPlayerInfo* m_pPlayerInfo;
	
	bool m_bScoreDirty;
	bool m_bSuperGunMoving;
	bool m_bIsExitingCoin;
    int  m_iCanGetLZP;

    QcSceneNodePtr                          m_pBomberBtn;
    QcSceneNodePtr                          m_pBomberTips;
    QcSceneNodePtr                          m_pBomberBtnEff;
    QcSceneNodePtr                          m_pBomberBtnArrow;
    QcSceneNodePtr                          m_pTheBomber;
    QcSceneNodePtr                          m_pBomberRoper;
    QcSceneNodePtr                          m_pBomberCounterDown;
    QcSceneNodePtr                          m_pBomberFlag;
    bool                                    m_bCanGetBomber;
    bool                                    m_bBomberLockIo;
    u32                                  m_iExplodeScore;
    QcBulletPtr                             m_pBomberBulletSave;

    PyroParticles::IPyroParticleEmitter     *m_pBombParticle;

    QcSceneNodePtr                          m_pSpecialFish;
	QcFishPtr										m_pRemoteBomberFishSave;
    bool                                    m_bSepcialFishDrawTop;
	//自动发炮;
	bool				m_bPlayerAutoFire;
	QcSceneNodePtr	m_pAutoFireLogo;
	f32				m_fAutoFireGapTime;
	bool				m_bAddStopAutoFire;
	bool				m_bAutoFireAndBomber;
	bool				m_bAutoFireAndGambleFish;
	//end
	QcFnKeyHelper	m_oFnKeyHelper;
	u32		m_iFreeScore;
private:
	QcList<QcFish*> m_pHitFish;
};