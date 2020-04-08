#pragma once

#include "Animator\QiAnimator.hpp"
class QcAnimatorStretch : public QiAnimator
{
public:
	QcAnimatorStretch(const QcPoint& fixedPos, QeMatrixMask StretchType = eMaskXYZ) : m_fixedPos(fixedPos), m_mxMask(StretchType){}

protected:
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
protected:
	QcPoint m_fixedPos;
    QeMatrixMask m_mxMask;
};