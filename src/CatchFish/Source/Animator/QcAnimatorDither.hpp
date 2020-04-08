#pragma once

#include "Animator\QiAnimator.hpp"
class QcAnimatorDither : public QiAnimator
{
public:
	QcAnimatorDither(){}

	void Reset(f32 fShakeTime, f32 fShakeScope, const QcPoint& orgPt)
	{
		QiAnimator::Reset();
		m_fShakeTime = fShakeTime;
		m_fShakeScope = fShakeScope;
		m_fSourcePt = orgPt;
	}
protected:
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
protected:
	f32 m_fShakeTime;
	f32 m_fShakeScope;
	QcPoint m_fSourcePt;
};