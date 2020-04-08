#pragma once

#include "Animator\QiAnimator.hpp"

class QcAnimatorFlash : public QiAnimator
{
public:
	QcAnimatorFlash(){}

	void Reset(f32 fTimeShow,f32 fTimeHide)
	{
		m_fTimeCount = 0.f;
		m_fTimeShow = fTimeShow;
		m_fTimeHide = fTimeHide;
		m_bSuspend = false;
	}
protected:
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
protected:
	f32 m_fTimeShow;
	f32 m_fTimeHide;
	f32 m_fTimeCount;
};