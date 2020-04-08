#pragma once

#include "Animator\QiAnimator.hpp"
class QcAnimatorAlpha : public QiAnimator
{
	DeclareRuntimeClass(QcAnimatorAlpha);
	QcAnimatorAlpha(){}
public:
	explicit QcAnimatorAlpha(f32 start, f32 end, f32 speed, bool bPingpong = false, bool bLoop = false);
protected:
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
	virtual void AddToSceneNode(QcSceneNode* pSceneNode);
private:
	f32 m_fMin;
	f32 m_fMax;
	f32 m_fCurrent;
	f32 m_fSpeed;
	bool m_bPingpong;		
	bool m_bLoop;
};