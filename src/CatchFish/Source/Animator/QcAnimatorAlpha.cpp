#include "pch.hpp"
#include "QcAnimatorAlpha.hpp"
#include "Scene\QcSceneNode.hpp"

ImplRuntimeClass2(QcAnimatorAlpha, 150);

QcAnimatorAlpha::QcAnimatorAlpha(f32 start, f32 end, f32 speed, bool bPingpong, bool bLoop)
: m_fMin(start * 255), m_fMax(end * 255), m_fSpeed(speed * 255), m_bPingpong(bPingpong), m_bLoop(bLoop)
{
	if (m_fMin > m_fMax)
		swap(m_fMin, m_fMax);
	m_fCurrent = start * 255;
}

void QcAnimatorAlpha::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	if (!QcMath::equals(giFrameID, m_iCurrentFrame))
	{
		m_fCurrent += m_fSpeed * fElapsedTime;
		if (m_fSpeed < 0)
		{
			if (m_fCurrent < m_fMin)
			{
				if (m_bLoop)
					m_fCurrent = m_fMax;
				else if (m_bPingpong)
					m_fSpeed = 0 - m_fSpeed;
				else
				{
					m_fCurrent = m_fMin;
					m_bFinished = true;
				}
			}
		}
		else
		{
			if (m_fCurrent > m_fMax)
			{
				if (m_bLoop)
					m_fCurrent = m_fMin;
				else if (m_bPingpong)
					m_fSpeed = 0 - m_fSpeed;
				else
				{
					m_fCurrent = m_fMax;
					m_bFinished = true;
				}
			}

		}
	}
	pSceneNode->SetCurrentTransparency((u32)(m_fCurrent + 0.5f));
}

void QcAnimatorAlpha::AddToSceneNode(QcSceneNode* pSceneNode)
{
	m_fCurrent = (m_fSpeed < 0 ? m_fMax : m_fMin);
	pSceneNode->SetCurrentTransparency((u32)m_fCurrent);
}