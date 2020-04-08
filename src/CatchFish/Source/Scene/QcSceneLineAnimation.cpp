#include "pch.hpp"
#include "QcSceneLineAnimation.hpp"
#include "Animator\QcAnimatorMoveLine.hpp"

ImplRuntimeClass(QcSceneLineAnimation);

QcSceneLineAnimation::QcSceneLineAnimation()
{
	m_pMoveAnimation = new QcAnimatorMoveLine();
}

void QcSceneLineAnimation::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcSceneAnimation::Animate(dCurrentTime, fElapsedTime);

	if (!m_pMoveAnimation->IsFinished())
	{
		m_pMoveAnimation->Animate(this, dCurrentTime, fElapsedTime);
	}
}

bool QcSceneLineAnimation::IsFinishedLineAnimation() const
{
	return m_pMoveAnimation->IsFinished();
}

void QcSceneLineAnimation::Reset(const QcPoint& start, const QcPoint& end, f32 timeForWay)
{
	QcSceneAnimation::Reset();
	m_pMoveAnimation->Reset(start, end, timeForWay);
	SetPosition(start);
}
void QcSceneLineAnimation::Reset2(const QcPoint& start, const QcPoint& end, f32 speed)
{
	QcSceneAnimation::Reset();
	m_pMoveAnimation->Reset2(start, end, speed);
	SetPosition(start);
}