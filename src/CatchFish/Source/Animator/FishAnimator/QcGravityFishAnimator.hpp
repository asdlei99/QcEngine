#pragma once

#include "Animator\QcAnimatorMovePath.hpp"
class QcGravityFishAnimator : public QcAnimatorMovePath
{
	DeclareRuntimeClass(QcGravityFishAnimator);
public:
	QcGravityFishAnimator();

	void SetKeyPontList(const QsPathKeyPointList& keyPointList);
protected:
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
protected:
	int m_index;
	f32 m_fCurrentLen;
	f32 m_fFactor;
	f32 m_fTime;
	QsPathKeyPointList m_arKeyPointList;
};