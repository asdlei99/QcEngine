#pragma once

#include "Animator\QcAnimatorMove.hpp"

class QcAnimatorMoveLine : public QcAnimatorMove
{
	DeclareRuntimeClass(QcAnimatorMoveLine);
public:
	explicit QcAnimatorMoveLine();

	void Reset(const QcPoint& start, const QcPoint& end, f32 timeForWay)
	{
		QcAnimatorMove::Reset();
		m_fTotalLen = start.getDistanceFrom(end);
		m_fSpeed = m_fTotalLen/timeForWay;
		m_timeVector = end - start;
		m_timeVector.normalize(m_fTotalLen);
	}
	void Reset2(const QcPoint& start, const QcPoint& end, f32 speed)
	{
		QcAnimatorMove::Reset();
		m_fTotalLen = start.getDistanceFrom(end);
		m_timeVector = end - start;
		m_fSpeed = speed;
		m_timeVector.normalize(m_fTotalLen);
	}
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
private:
	QcVector m_timeVector;
	f32 m_fTotalLen;
};