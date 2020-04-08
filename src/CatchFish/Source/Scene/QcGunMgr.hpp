#pragma once

class QcGun;
class QcDigitalNode;
typedef QcSharedPtr<QcGun> QcGunPtr;
class QcGunMgr : public QcObject
{
public:
	QcGunMgr();
	~QcGunMgr();

	void Init(QcPlayer* pPlayer, const QcMathNodeInfo& gunPos);
	void Update(QcPlayerInfo* pInfo);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void RenderFort(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);

	bool IsSuperGun() const {return m_bSetSuperGun; }
	void SetSuperGun(bool bSuperGun){ if (m_bSetSuperGun != bSuperGun) { m_bSetSuperGun = bSuperGun; SwitchGun(m_iCurrentGunLevel, true); }}
	u32 AddGunLevel(u32 iAdd);
	void Fire(u32 iScore);
	void FreeFire(u32 iScore);							//bug·¢ÅÚº¯Êý
	void AdjustGunOriental(f32 fAddRotation);
	void AdjustGunOriental(const QcPoint& vec);
	void AdjustGunOriental(const QcPoint& vec ,f32 fMinRotation);

	u32 GetCurrentLevel() const {return m_iCurrentGunLevel; }
	s32 GetMoveNodeListSize(){ return m_arMoveNodeList.size(); }
	s32 GetCurrentGunNum() { return m_iCurrentGun; }
protected:
	QcGun* GetCurrentGun() {return m_arGun[m_iCurrentGun];}
	const QcGunPtr& GetGun(u32 index) const {return m_arGun[index];}
	void MakeMoveAnimation(s32 iGun, bool bMoveDown = true);
	f32 GetRotation() const {return m_fOriental; }
	void SetRotation(f32 foriental);
	void SwitchGun(u32 iLevel, bool switchToSuperGun = false);
	s32 FindGun(u32& iLevel);
protected:
	QcPlayer* m_pPlayer;
	QcSceneAnimationPtr m_pEnergyAnimation;
	QcSceneNodePtr m_pNormalFort;
	QcSceneNodePtr m_pSuperFort;
	QcSharedPtr<QcSceneDigital> m_pGunLevel;
	QcTexturePtrList m_pGunLevelBgList;
	QcPoint m_vOffsetTotal;

	bool m_bMoveDown;
	f32 m_fMoveLen;
	f32 m_fMoveSpeed;
	f32 m_fGravitySpeed;
	QcVector m_vMoveVec;
	QcPoint m_vOffsetPoint;
	std::vector<QcSceneNode*> m_arMoveNodeList;

	QcList<QcGunPtr> m_arGun;
	bool m_bSetSuperGun;
	s32 m_iCurrentGun;
	u32 m_iCurrentGunLevel;
	u32 m_iMinLevel;
	float m_fOriental;
};