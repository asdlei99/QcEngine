#pragma once

#include "Animator\QcAnimatorMove.hpp"
class QcAnimatorMoveBoundBox : public QcAnimatorMove
{
	DeclareRuntimeClass(QcAnimatorMoveBoundBox);
public:
	QcAnimatorMoveBoundBox(){}

	void Reset(const QcRect& box, f32 fSpeed)
	{
		QcAnimatorMove::Reset();
		m_rcBox = box;
		m_fSpeed = fSpeed;
		m_timeVector.X = 1.f;
		m_timeVector.Y = 0.f;
	}

	void SetRotation(f32 fRoration)
	{ 
		m_fRoration = fRoration; 
		m_timeVector.X = 1.0f;
		m_timeVector.Y = 0.f;
		m_timeVector.rotateByRad(m_fRoration);
	}
	void SetRotation(const QcVector& vec)
	{
		m_timeVector = vec;
		m_timeVector.normalize();
		m_fRoration = (f32)vec.getRadAngleTrig();
	}
protected:
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
	virtual void AddToSceneNode(QcSceneNode* pSceneNode);
	void Walk(QcPoint& vPos, f32 fDist);
private:
	f32 m_fRoration;
	QcVector m_timeVector;
	QcRect m_rcBox;
};