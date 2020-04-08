#pragma once

#include "Animator\QiAnimator.hpp"

class QcAnimatorScale : public QiAnimator
{
	DeclareRuntimeClass(QcAnimatorScale);
	QcAnimatorScale(){}
public:
	explicit QcAnimatorScale(f32 fMin, f32 fEnd, f32 speed, bool bPingpong = false, bool bLoop = false);
protected:
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
private:
	f32 m_fMin;
	f32 m_fMax;
	f32 m_fSpeed;
	bool m_bPingpong;
	bool m_bLoop;
};