#pragma once

#include "Scene\QcSceneDigital.hpp"

class QcSceneAnimation : public QcSceneDigital
{
	typedef QcSceneDigital QcBaseClass;
	DeclareRuntimeClass(QcSceneAnimation);
public:
	explicit QcSceneAnimation() 
	{ 
		Reset();
	}

	void Reset()
	{
		m_fLivingTime = 0.f;
		m_bFinished = false;
		m_pEndAnimator = NULL;
	}

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void FinishAnimator(const QiAnimator* pAnimator);
	virtual void SetLivingTime(f32 fLivingTime);
	void SetFinishFlag(const QiAnimator* pAnimator) { m_pEndAnimator = pAnimator;}
	bool IsFinished() const {return m_bFinished; }
	
	virtual void AddAnimator(QiAnimator* pMonitor);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
protected:
	const QiAnimator* m_pEndAnimator;
	f32 m_fLivingTime;
	bool m_bFinished;
};