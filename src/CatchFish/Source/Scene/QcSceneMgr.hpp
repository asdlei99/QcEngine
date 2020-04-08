#pragma once

#include "Scene\QcQuickAccountShowNode.hpp"
#include "Scene\QcVolCtrl.hpp"

class QcDeviceRender;
class QcResManager;
class QcFishMgr;
class QcBulletMgr;
class QcPlayerMgr;
class QcBackGroundMgr;
class QcSceneAnimationMgr;
class QcSettingMgr;

class QcSceneMgr : public QcObjectEx
{
public:
	enum QeSceneState
	{
		eSceneNormal,
		eSceneEnd,
		eSceneLogoAnimation,
		eSceneRegular,
		eSceneSetttingReady,
		eSceneSettting,
		eSceneSettingEnd,
	};
	explicit QcSceneMgr();
	virtual ~QcSceneMgr();
	virtual void OnEvent(const QsEvent& evt);
	virtual void OnTimer(u32 nTimer, void* pContext);

	void Init(QcResManager* pResMgr);
	void StartGame();

 	void OnKeyDown(u32 key, const QsKeyInfo& info);
 	void OnKeyUp(u32 key, const QsKeyInfo& info);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render);
	void HandleCollision();
	QeSceneState GetSceneState() const {return m_eSceneState;}
	void LockNormalScene(bool bLock) 
	{
		bLock ? ++m_iLockNormalScene : --m_iLockNormalScene;
	}

	u32 GetSceneId();
	QcBackGroundMgr* GetBgMgr(){return m_pBackGroundMgr[giScreenHandleing]; }
	QcFishMgr* GetFishMgr(){return m_pFishMgr[giScreenHandleing].Get();}
	QcBulletMgr* GetBulletMgr() {return m_pBulletMgr[giScreenHandleing].Get(); }
	QcSceneAnimationMgr* GetAnimationMgr(){return m_pAnimationMgr[giScreenHandleing].Get(); }

	QcPlayerMgr* GetPlayerMgr() {return m_pPlayerMgr.Get();}
	void TurnOffAutoFire();
protected:
	bool IsLockNormalScene() const {return m_iLockNormalScene != 0;}
	bool LockState() 
	{ 
		++m_iLockStateCount; 
		if (m_iLockStateCount == m_nLockStateCount) 
		{
			m_iLockStateCount = 0;
			return true;
		}
		return false;
	}

	bool IsUionScreenMode() const {return m_IsUionScreenMode;}
	bool IsTwoScreenMode() const {return m_IsTwoScreenMode;} 
	bool m_IsUionScreenMode;
	bool m_IsTwoScreenMode;
private:
	QeSceneState m_eSceneState;
	QcSharedPtr<QcBackGroundMgr> m_pBackGroundMgr[QmMaxScreen];
	QcSharedPtr<QcSceneAnimationMgr> m_pAnimationMgr[QmMaxScreen];
	QcSharedPtr<QcFishMgr> m_pFishMgr[QmMaxScreen];
	QcSharedPtr<QcBulletMgr> m_pBulletMgr[QmMaxScreen];
	QcSharedPtr<QcPlayerMgr> m_pPlayerMgr;

	QcSettingMgr* m_pSettingMgr;
	QcSceneNodePtr m_pReadySetting;
	QcSharedPtr<QcQuickAccountShowNode> m_pQuickAccount;
	QcSceneNodePtr m_pErrorBg;
	QcSharedPtr<QcSceneDigital> m_pCardErrorNode;
	QcSceneNodePtr m_pDaMaErrorNode;
	QcSceneNodePtr m_pIO1ConnectErrorNode;
	QcSceneNodePtr m_pIO2ConnectErrorNode;
	QcSharedPtr<QcVolCtrl> m_pVolumnCtrl;

	bool		   m_bSceneStateTimerOn;
	s32		   m_iLockNormalScene;
	int			   m_iLockStateCount;
	int			   m_nLockStateCount;
};