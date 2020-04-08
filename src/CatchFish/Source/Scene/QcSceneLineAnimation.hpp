#pragma once

#include "Scene\QcSceneAnimation.hpp"
class QcAnimatorMoveLine;
class QcSceneLineAnimation : public QcSceneAnimation
{
	DeclareRuntimeClass(QcSceneLineAnimation);
public:
	QcSceneLineAnimation();

	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	void Reset(const QcPoint& start, const QcPoint& end, f32 timeForWay);
	void Reset2(const QcPoint& start, const QcPoint& end, f32 speed);
	bool IsFinishedLineAnimation() const;
	bool IsFinished() const {return m_bFinished && IsFinishedLineAnimation(); }
	QcAnimatorMoveLine* GetLineAnimator() {return m_pMoveAnimation;} 
protected:
	QcSharedPtr<QcAnimatorMoveLine> m_pMoveAnimation;
};