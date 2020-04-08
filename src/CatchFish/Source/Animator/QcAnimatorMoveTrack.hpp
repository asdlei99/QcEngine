#pragma once

#include "Animator\QcAnimatorMove.hpp"
#include "Scene\QiSceneNodeMonitor.hpp"

class QcAnimatorMoveTrack : public QcAnimatorMove, public QiSceneNodeMonitor
{
	DeclareRuntimeClass(QcAnimatorMoveTrack);
public:
	QcAnimatorMoveTrack();
	virtual ~QcAnimatorMoveTrack();

	void Reset(QcSceneNode* pTargetNode, f32 speed, bool bSelfRotate = true);
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
	virtual void OnRemoveNodeMonitor(QcSceneNode*);
protected:
	void RemoveMonitor();
protected:
	QcSharedPtr<QcSceneNode> m_pTargetNode;
	bool m_bSelfRotate;
};