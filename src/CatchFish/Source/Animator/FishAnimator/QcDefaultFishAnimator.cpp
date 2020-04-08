#include "Pch.hpp"
#include "QcDefaultFishAnimator.hpp"

ImplRuntimeClass2(QcDefaultFishAnimator, 200);


QcDefaultFishAnimator::QcDefaultFishAnimator()
: m_fHitTime(0.f)
{

}

void QcDefaultFishAnimator::Reset(const QcResPathDesc* pDesc, f32 fSpeed)
{
	QcAnimatorMovePath::Reset(pDesc, fSpeed);
}

void QcDefaultFishAnimator::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	QcAnimatorMovePath::AnimateNode(pSceneNode, dCurrentTime, fElapsedTime);
}

void QcDefaultFishAnimator::OnHit(f32 fHitTime)
{
	if (m_arGravityDeque.size())
	{
		m_arGravityDeque.front()->OnHit(fHitTime);
	}
}