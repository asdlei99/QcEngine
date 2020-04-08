#include "pch.hpp"
#include "QcAnimatorMoveLine.hpp"
#include "Scene\QcSceneNode.hpp"

ImplRuntimeClass2(QcAnimatorMoveLine, 1000);

QcAnimatorMoveLine::QcAnimatorMoveLine()
{
}

void QcAnimatorMoveLine::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
    f32 fOldSpd = m_fSpeed;
	if (HandleSpeed(pSceneNode, dCurrentTime, fElapsedTime))
	{
		f32 fLen = QcMath::min_(abs((m_fSpeed+fOldSpd)*0.5f) * fElapsedTime, m_fTotalLen);
		m_vOffset = fLen * m_timeVector;
		m_fTotalLen -= fLen;
		if (m_fTotalLen < ROUNDING_ERROR_f32)
		{
			m_bFinished = true;
		}
	}
	pSceneNode->SetPosition(pSceneNode->GetPosition() + m_vOffset);
}
