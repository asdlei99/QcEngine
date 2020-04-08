#pragma once

#include "Animator\QcAnimatorMove.hpp"

class QcAnimatorMoveOffset : public QcAnimatorMove
{
public:
	QcAnimatorMoveOffset()
	{
	}

	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime = 0.f);
	void Reset(const std::vector<QcMathNodeInfo>& offsetList, const QcMathNodeInfo& beginPt, f32 timeOneOffset = 0.f)
	{
		m_offsetList = offsetList;
		m_originInfo = beginPt;
		m_fTime = timeOneOffset;
		m_fTimeCount = 0.f;
		m_iCurrent = 0;
		QcAnimatorMove::Reset();
	}
protected:
	std::vector<QcMathNodeInfo> m_offsetList;
	QcMathNodeInfo m_originInfo;
	f32 m_fTime;
	f32 m_fTimeCount;
	u32 m_iCurrent;
};