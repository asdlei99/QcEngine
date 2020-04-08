#include "pch.hpp"
#include "QcAnimatorScale.hpp"
#include "Scene\QcSceneNode.hpp"

ImplRuntimeClass2(QcAnimatorScale, 100);

QcAnimatorScale::QcAnimatorScale(f32 start, f32 end, f32 speed, bool bPingpong, bool bLoop)
: m_fMin(start), m_fMax(end), m_fSpeed(speed), m_bPingpong(bPingpong), m_bLoop(bLoop)
{
	if (m_fMin > m_fMax)
		swap(m_fMin, m_fMax);
}

void QcAnimatorScale::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	f32 dScale = m_fSpeed * fElapsedTime;
	f32 currentScale = pSceneNode->GetScaleX() + dScale;
	if (m_fSpeed < 0)
	{
		if (currentScale < m_fMin)
		{
			if (m_bLoop)
				currentScale = m_fMax;
			else if (m_bPingpong)
				m_fSpeed = 0 - m_fSpeed;
			else
			{
				m_bFinished = true;
				currentScale = m_fMin;
			}
		}
		else if (currentScale + dScale < m_fMin)
			currentScale = m_fMin;
	}
	else
	{
		if (currentScale > m_fMax)
		{
			if (m_bLoop)
				currentScale = m_fMin;
			else if (m_bPingpong)
				m_fSpeed = 0 - m_fSpeed;
			else
			{
				currentScale = m_fMax;
				m_bFinished = true;
			}
		}
		else if (currentScale + dScale > m_fMax)
			currentScale = m_fMax;
	}
	pSceneNode->SetScale(currentScale);
}