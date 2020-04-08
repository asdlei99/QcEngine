#pragma once

#include "Animator\QiAnimator.hpp"
#include "Animator\QcAnimatorSpeed.hpp"
#include "Animator\QcPathKeyPointHandler.hpp"

class QcPathKeyPointHandler;
class QcResPathDesc;
class QcAnimatorMove : public QiAnimator
{
protected:
	QcAnimatorMove() : m_fSpeed(0.f){}
public:

	virtual void OnHit(f32 fHitTime){}
	virtual void Reset(const QcResPathDesc* pDesc, f32 fSpeed){}
	virtual bool IsMoveAnimator() const {return true;}
	void Reset()
	{
		QiAnimator::Reset();
		m_vOffset.X = 0.f;
		m_vOffset.Y = 0.f;
		m_fSpeed = 0.f;
		m_arGravityDeque.clear();
	}

	void SetSpeed(f32 fSpeed){ m_fSpeed = fSpeed;}
	void AddSpeedAnimator(QcAnimatorSpeed* pAnimator){ m_arGravityDeque.push_back(QcAnimatorSpeedPtr(pAnimator));}
protected:
	bool HandleSpeed(QcSceneNode* pSceneNode, double dCurrentTime, f32 fElapsedTime)
	{
		if (!QcMath::equals(giFrameID, m_iCurrentFrame))
		{
			if (m_arGravityDeque.size())
			{
				if (m_arGravityDeque.front()->Animate(pSceneNode, m_fSpeed, dCurrentTime, fElapsedTime))
					m_arGravityDeque.pop_front();
			}
			return true;
		}
		return false;
	}
protected:
	f32 m_fSpeed;
	QcVector m_vOffset;
	std::deque<QcAnimatorSpeedPtr> m_arGravityDeque;
};