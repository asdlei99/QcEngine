#pragma once

#include "Animator\QiAnimator.hpp"
class QiAnimatorDeque : public QiAnimator
{
	DeclareRuntimeClass(QiAnimatorDeque);
public:
	QiAnimatorDeque() : m_iCurrent(0) { m_anmatorDeque.reserve(4);}

	virtual bool IsAnimatorDeque() const {return true;}
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
	{
		QiAnimator* pAnimator = CurrentAnimator();
		if (pAnimator != NULL)
		{
			pAnimator->Animate(pSceneNode, dCurrentTime, fElapsedTime);
			if (pAnimator->IsFinished())
			{
				++m_iCurrent;
				if (m_iCurrent == (int)m_anmatorDeque.size())
				{
					m_bFinished = true;
				}
			}
		}
		else
		{
			m_bFinished = true;
		}
	}
	QiAnimator* CurrentAnimator()
	{
		if (m_iCurrent < (int)m_anmatorDeque.size())
			return m_anmatorDeque[m_iCurrent];
		return NULL;
	}
	void AddAnimator(QiAnimator* pAnimator)
	{
		m_anmatorDeque.PushBack(pAnimator);
		pAnimator->m_bInDeque = true;
	}
protected:
	int m_iCurrent;
	QcAnimatorPtrList m_anmatorDeque;
};