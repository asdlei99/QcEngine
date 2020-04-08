#pragma once

#include "Animator\QcAnimatorMovePath.hpp"
class QcDefaultFishAnimator : public QcAnimatorMovePath
{
	DeclareRuntimeClass(QcDefaultFishAnimator);
public:
	QcDefaultFishAnimator();

protected:
	virtual void OnHit(f32 fHitTime);
	virtual void Reset(const QcResPathDesc* pDesc, f32 fSpeed);
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
protected:
	f32 m_fHitTime;
};