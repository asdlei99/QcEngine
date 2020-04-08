#include "pch.hpp"
#include "QcSceneAnimation.hpp"

ImplRuntimeClass2(QcSceneAnimation, 500);

void QcSceneAnimation::SetLivingTime(f32 fLivingTime)
{
	m_bFinished = false;
	m_fLivingTime = fLivingTime;
}

void QcSceneAnimation::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcBaseClass::Init(desc, bLoadSubNode);
	SetDigitalVisible(false);
}

void QcSceneAnimation::AddAnimator(QiAnimator* pMonitor)
{
	QcBaseClass::AddAnimator(pMonitor);
	m_bFinished = false;
}

void QcSceneAnimation::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcBaseClass::Animate(dCurrentTime, fElapsedTime);
	if (m_fLivingTime > ROUNDING_ERROR_f32)
	{
		m_bFinished = false;
		m_fLivingTime -= fElapsedTime;
		if (m_fLivingTime < ROUNDING_ERROR_f32)
		{
			m_bFinished = true;
		}
	}
}

void QcSceneAnimation::FinishAnimator(const QiAnimator* pAnimator)
{
	QcBaseClass::FinishAnimator(pAnimator);
	if (m_pEndAnimator)
	{
		if (pAnimator == m_pEndAnimator)
		{
			m_bFinished = true;
			m_pEndAnimator = NULL;
		}
	}
	else
	{
		m_bFinished = true;
		QcAnimatorPtrListIter iter(m_arAnimatorList.begin());
		QcAnimatorPtrListIter endIter(m_arAnimatorList.end());
		for (; iter != endIter; ++iter)
		{
			if (*iter && !((*iter)->IsFinished() || (*iter)->IsTextureAnimator()))
			{
				m_bFinished = false;
				break;
			}
		}
	}
}