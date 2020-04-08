#include "pch.hpp"
#include "QcSceneAnimationDeque.hpp"
#include "QcSceneAnimation.hpp"

void QcSceneAnimationDeque::AddAnimation(const QcSceneAnimationPtr& animation)
{
	m_arAnimation.push_back(QcSceneAnimationPtr(animation));
}

void QcSceneAnimationDeque::Animate(double dCurrentTime,f32 fElapsedTime)
{
	int nSize = m_arAnimation.size();
	if (nSize)
	{
		m_arAnimation.front()->Animate(dCurrentTime, fElapsedTime);
	}
}

void QcSceneAnimationDeque::Render(QcDeviceRender& render)
{
	if (m_arAnimation.size())
	{
		QcSceneAnimationPtr& pAnimation = m_arAnimation.front();
		pAnimation->Render(render);
		if (pAnimation->IsFinished())
		{
			pAnimation->ToDead();
			m_arAnimation.pop_front();
		}
	}
}