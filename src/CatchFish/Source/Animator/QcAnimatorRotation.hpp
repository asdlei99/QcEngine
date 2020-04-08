#pragma once

#include "Animator\QiAnimator.hpp"

class QcAnimatorRotation : public QiAnimator
{
public:
	explicit QcAnimatorRotation(f32 radSpeed, f32 fRad = 0.f);
protected:
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
private:
	f32 m_fRadSpeed;	//旋转速度
	f32 m_fRotateRad;	//旋转幅度
};