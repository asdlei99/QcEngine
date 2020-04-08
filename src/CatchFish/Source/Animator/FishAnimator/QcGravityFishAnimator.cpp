#include "Pch.hpp"
#include "QcGravityFishAnimator.hpp"

ImplRuntimeClass(QcGravityFishAnimator);

QcGravityFishAnimator::QcGravityFishAnimator()
: m_index(0)
, m_fFactor(0.f)
, m_fCurrentLen(0.f)
, m_fTime(0.f)
{

}

void QcGravityFishAnimator::SetKeyPontList(const QsPathKeyPointList& keyPointList)
{
	m_arKeyPointList = keyPointList;
	m_index = 0;
	m_fCurrentLen = 0.f;
	m_fFactor = 0.f;
	m_fTime = 0.f;
}

void QcGravityFishAnimator::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	if (!QcMath::equals(giFrameID, m_iCurrentFrame))
	{
		if (m_index < (int)m_arKeyPointList.size())
		{
			m_fCurrentLen += m_fSpeed * fElapsedTime;

			if (m_fCurrentLen >= m_arKeyPointList[m_index].m_fLen)
			{
				m_fFactor = m_arKeyPointList[m_index].m_fSpeedFactor;
				m_fTime = m_arKeyPointList[m_index].m_fTime;

				if (!QcMath::equals(m_arKeyPointList[m_index].m_fSpeed, 0.f))
					m_fSpeed = m_arKeyPointList[m_index].m_fSpeed;

				if (m_arKeyPointList[m_index].m_iParamCb == 1)
				{
					pSceneNode->ToShowState(eLiving);
				}
				else if (m_arKeyPointList[m_index].m_iParamCb == 2)
				{
 					pSceneNode->ToShowState(ePlaying);
				}
				++m_index;
			}
		}
		if (m_fTime > 0.f)
		{
			m_fSpeed += m_fFactor * fElapsedTime;
			m_fTime -= fElapsedTime;
		}
	}
	QcAnimatorMovePath::AnimateNode(pSceneNode, dCurrentTime, fElapsedTime);
}
