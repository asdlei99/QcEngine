#include "Pch.hpp"
#include "QcPathKeyPointHandler.hpp"
#include "QcAnimatorSpeed.hpp"

QcPathKeyPointHandler::QcPathKeyPointHandler(const QsPathKeyPointList& keyPointList, bool bNeedSort) 
: m_arKeyPointList(keyPointList)
, m_fCurrentLen(0.f)
, m_index(0)
, m_fTime(0.f)
, m_fFactor(0.f)
{
	if (bNeedSort)
	{
		std::sort(m_arKeyPointList.begin(), m_arKeyPointList.end(), std::less<QsPathKeyPoint>());
	}
}

bool QcPathKeyPointHandler::Animate(QcSceneNode* pSceneNode, f32& fSpeed, double dCurrentTime,f32 fElapsedTime)
{
	if (!QcMath::equals(m_iCurrentFrame, giFrameID))
	{
		if (m_fTime > 0.f)
		{
			fSpeed += m_fFactor * fElapsedTime;
			m_fTime -= fElapsedTime;
		}

		if (m_index < (int)m_arKeyPointList.size())
		{
			m_fCurrentLen += fSpeed * fElapsedTime;

			while (m_index < (int)m_arKeyPointList.size() && m_fCurrentLen >= m_arKeyPointList[m_index].m_fLen)
			{
				m_fFactor = m_arKeyPointList[m_index].m_fSpeedFactor;
				m_fTime = m_arKeyPointList[m_index].m_fTime;

				if (!QcMath::equals(m_arKeyPointList[m_index].m_fSpeed, 0.f))
					fSpeed = m_arKeyPointList[m_index].m_fSpeed;

				if (m_arKeyPointList[m_index].m_cb)
				{
					m_arKeyPointList[m_index].m_cb(m_arKeyPointList[m_index].m_iParamCb);
				}
				else if (m_arKeyPointList[m_index].m_iParamCb)
				{
					OnCb(m_arKeyPointList[m_index].m_iParamCb);
				}
				++m_index;
			}
		}
		else
		{
			return m_fTime <= 0.f;
		}
	}
	return false;
}