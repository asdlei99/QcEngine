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
	//�������б��ܷ���FishMgr�У�������������ͬһ����ʱ�������Ƶ�λ�ô���
	QcSharedPtr<QcAnimatorMoveCircle> m_pMoveCircle;
	QcSharedPtr<QcAnimatorMoveTrack> m_pMoveTrack;			//���ٶ���
	QcSceneNodePtr m_gunPosNode;								//������
	QcSceneAnimationPtr m_pPlayerIDFlag;						//��ұ��
	QcSharedPtr<QcSceneAnimation> m_pPlayerIdMovingFlag;	//׷�ٱ��
	QcSceneNodePtr m_pFishFlag;									//������
	QcFish* m_pTargetFish;										//��������
	QcPlayer* m_pPlayer;

	QcPoint m_vAbsFishFlagPos;
};