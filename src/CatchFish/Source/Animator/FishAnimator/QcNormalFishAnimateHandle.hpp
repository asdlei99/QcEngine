#pragma once

#include "Animator\QcAnimatorSpeed.hpp"

class QcNormalFishAnimateHandle : public QcAnimatorSpeed
{
	DeclareRuntimeClass(QcNormalFishAnimateHandle);
	QcNormalFishAnimateHandle(){}
public:
	QcNormalFishAnimateHandle(f32 fMinSpeed, f32 fMaxSpeed, f32 fMinGapTime, f32 fMaxGapTime, u32 playingFlag);
protected:
	virtual void OnHit(f32 fHitTime);
	virtual bool Animate(QcSceneNode* pSceneNode, f32& fSpeed, double dCurrentTime,f32 fElapsedTime);
protected:
	u32 m_iPlayingFlag;
	f32 m_fPlayingTime;
	f32 m_fPlayingTimeCount;
	f32 m_fSaveTimePerFrame;
	QeState m_eToState;

	f32 m_fDestSpeed;
	f32 m_fGravity;

	f32 m_fMinSpeed;
	f32 m_fMaxSpeed;
	f32 m_fMinGapTime;
	f32 m_fMaxGapTime;
	f32 m_fTimeCount;
};