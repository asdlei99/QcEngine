#pragma once

class QcAnimatorSpeed : public QcObject
{
public:
	QcAnimatorSpeed() : m_iCurrentFrame(0), m_fLastSpeed(0.f){}

	virtual void OnHit(f32 fHitTime){}
	virtual bool Animate(QcSceneNode* /*pSceneNode*/, f32& fSpeed, double dCurrentTime,f32 fElapsedTime) = 0;
protected:
	f32 m_fLastSpeed;
	u64 m_iCurrentFrame;
};

class QcGravitySpeed : public QcAnimatorSpeed
{
	DeclareRuntimeClass(QcGravitySpeed);
protected:
	QcGravitySpeed(){}
public:
	QcGravitySpeed(f32 fFactor, f32 fTime) : m_fFactor(fFactor), m_fTime(fTime){}
protected:
	bool Animate(QcSceneNode* /*pSceneNode*/, f32& fSpeed, double dCurrentTime,f32 fElapsedTime)
	{
		if (!QcMath::equals(giFrameID, m_iCurrentFrame))
		{
			m_iCurrentFrame = giFrameID;

			fSpeed += m_fFactor * fElapsedTime;
			m_fTime -= fElapsedTime;

			m_fLastSpeed = fSpeed;
		}
		else
		{
			fSpeed = m_fLastSpeed;
		}
		//QmAssert(fSpeed > 2.0f);
		return m_fTime < 0.f;
	}
protected:
	f32 m_fFactor;
	f32 m_fTime;
};


typedef QcSharedPtr<QcAnimatorSpeed> QcAnimatorSpeedPtr;