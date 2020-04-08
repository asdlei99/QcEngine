#pragma once

#include "Scene\QcSceneDigital.hpp"

class QcAnimatorTexture;
class QcFish : public QcSceneDigital
{
	friend class QcBombFish;
	typedef QcSceneDigital BaseClass;
	DeclareRuntimeClass(QcFish);
public:
	enum QeDeadWay
	{
		eNone,
		eHitDead,
		eExposeDead,
		eNatureDead,
		eForceDead,
	};
	enum QeFishFlag
	{
		eNormal = 0x1,
		eQueue = 0x2,
		eRegular = 0x4,
		eSceneRegular = 0x8,
		eBottom = 0x20,
		eCalController = 0x40, 
	};
	enum QePlayingFlag
	{
		eRandomPlay = 0x0001,
		eHitPlay = 0x0002,

		eSpeedUp = 0x1000,
		eSpeedDownToUp = 0x2000,
	};
public:
	explicit QcFish();
	virtual ~QcFish();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);

	virtual void AddAnimator(QiAnimator* pAnimator);
	virtual void FinishAnimator(const QiAnimator* pDesc);
	virtual bool ToDie(bool bDieTimer = true);
	virtual void ToDead();
	virtual void HitByBullet(QcBullet* pBullet){}
	virtual void HitToDie(QcBullet* pBullet);
	virtual void ExploseToDie(QcFish* pFish);
	virtual void HitAnimator(QcBullet* pBullet);
	virtual bool IsBombFish() const {return false;}
    virtual bool IsChainLinkFish() const {return false;}
	virtual bool IsDelayCalScore() const {return false;}
	virtual u32 GetExplodeScore() const {return m_iScore;}
	virtual QeState ToShowState(QeState eState);
	virtual void OnEvent(const QsEvent& evt);
	virtual void EnterShelter() { m_bSheltered = true;}
	virtual void LeaveShelter() { m_bSheltered = false;}
	bool IsSheltered() const {return m_bSheltered;}

	QeState TryToPlayingState();
	
	void CalSpecialScore(const QcObjType& objType, u32& iScores);
	void ShowScore(bool bShow){ SetDigitalVisible(bShow);}
	void SetScorePos(const QcPoint& pos){ m_oDigitalNode.SetPosition(pos);}
	void AddLocker(QcPlayer* pLocker);
	void RemoveLocker(QcPlayer* pLocker);
	u32 GetScore() const{ return m_iScore;}
	u32 GetFishType() const {return m_nID;}
	u32 GetLayer() const {return m_iLayer;}
	void SetLayer(u32 iLayer){ m_iLayer = iLayer;}
	void Frozen(bool bFrozen);
	u32 GetFishFlag() const {return m_iFishFlag;}
	void SetFishFlag(u32 eFlag) {m_iFishFlag = eFlag;}
	f32 GetDepth() const {return m_fDeep;}
	void SetDepth(f32 fDepth) { m_fDeep = fDepth;}
	void MakeCoinAnimation();
	QcPoint PushFishAnimation(const QcVector& vec);
    void SetFinishFlag(const QiAnimator* pAnimator) { m_pEndAnimator = pAnimator;}
    QcPlayer* GetOwner() {return m_pOwner;}
    QcBullet* GetBullet(){return m_pBullet;}
    void SetSpecialFlag(QcSceneNode *pObj) {m_pSpecialObj = pObj;}
    QcSceneNode *GetSpecialFlag() {return m_pSpecialObj;};
    virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
#ifdef QmTestVersion
	QcObjType m_PathID;
	void setPathObject(const QcObjType& type){m_PathID = type;}
#endif
protected:
	virtual void ScoreChange(u32 iRaw, u32 iNow);
	void SetScore(u32 iScore)
	{
		if (m_iScore != iScore)
			ScoreChange(m_iScore, iScore);
		m_iScore = iScore;
	}
	void MakeDyingAnimations(u32 iScore, const QcPoint& pos, f32 delaytime, f32 dyingTime = 0);
	void RemoveAllLocker();
	const QcResFishDesc* GetFishDesc() const {return (const QcResFishDesc*)(GetDesc());}
	QcAnimatorMove* FindMoveAnimator();
	virtual void OnTimer(u32 nTimer, void* pContext);
protected:
	f32 m_fDeep;
	u32 m_nID;
	u32 m_iLayer;
	u32 m_iFishFlag;
	u32 m_iPlayingFlag;
	u32 m_iPlayingTime;
	bool m_bSheltered;
	bool m_bShowedCoin;

	const QiAnimator* m_pEndAnimator;
	QeDeadWay m_eDeadWay;
	QeState m_toState;
	QcSharedPtr<QcAnimatorTexture> m_pTextureAnimator;
	QcList<QcPlayer*> m_arLockerList;
	QcPlayer* m_pOwner;
	QcBulletPtr m_pBullet;

    QcSceneNodePtr  m_pSpecialObj;
private:
	u32 m_iScore;
	int giScreenID;
};