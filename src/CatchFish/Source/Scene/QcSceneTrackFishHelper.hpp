#pragma once

class QcAnimatorMoveCircle;
class QcAnimatorMoveTrack;
class QcSceneTrackFishHelper : public QcObject
{
	DeclareRuntimeClass(QcSceneTrackFishHelper);
public:
	QcSceneTrackFishHelper();

	void Init(QcPlayer* pPlayer, const QcPoint& vFishFlagPos, const QcPoint& vGunPos);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);

	void LockFish();
	void LockFish(QcFish* pLockFish);
	QcFish* GetLockFish() const {return m_pTargetFish; };
protected:
	QcSceneNode* MakeFishFlagNode(QcFish* pFish);
	QcPlayer* GetPlayer() {return m_pPlayer;}
protected:
	//锁定牌列表不能放在FishMgr中，当多个玩家锁定同一类鱼时，锁定牌的位置错乱
	QcSharedPtr<QcAnimatorMoveCircle> m_pMoveCircle;
	QcSharedPtr<QcAnimatorMoveTrack> m_pMoveTrack;			//跟踪动画
	QcSceneNodePtr m_gunPosNode;								//辅助点
	QcSceneAnimationPtr m_pPlayerIDFlag;						//玩家标记
	QcSharedPtr<QcSceneAnimation> m_pPlayerIdMovingFlag;	//追踪标记
	QcSceneNodePtr m_pFishFlag;									//锁定牌
	QcFish* m_pTargetFish;										//锁定的鱼
	QcPlayer* m_pPlayer;

	QcPoint m_vAbsFishFlagPos;
};