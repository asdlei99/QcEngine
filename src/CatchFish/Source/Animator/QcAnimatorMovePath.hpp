#pragma once

#include "Animator\QcAnimatorMove.hpp"

class QcAnimatorMovePath : public QcAnimatorMove
{
	DeclareRuntimeClass(QcAnimatorMovePath);
public:
	QcAnimatorMovePath() : m_pResPathDesc(NULL){}
	~QcAnimatorMovePath();

	virtual void Reset(const QcResPathDesc* pDesc, f32 fSpeed);
protected:
	void Walk(f32 fDist);
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
protected:
	QcResPathDesc* m_pResPathDesc;
	u32 m_iIndex;
	f32 m_fCurrentLeft;
};