#include "pch.hpp"
#include "QcAnimatorMoveOffset.hpp"
#include "Scene\QcSceneNode.hpp"

void QcAnimatorMoveOffset::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	if (!m_bFinished && HandleSpeed(pSceneNode, dCurrentTime, fElapsedTime))
	{
		m_fTimeCount += fElapsedTime;
		if (m_fTimeCount > m_fTime)
		{
			m_fTimeCount = 0.f;
			++m_iCurrent;
			if (m_iCurrent < m_offsetList.size())
			{
				pSceneNode->SetPosition(m_originInfo.GetPos() + m_offsetList[m_iCurrent].GetPos());
				pSceneNode->SetRotation(m_originInfo.GetRotate() + m_offsetList[m_iCurrent].GetRotate());
				pSceneNode->SetScale(m_originInfo.GetScaleX() * m_offsetList[m_iCurrent].GetScaleX(), eMaskX);
				pSceneNode->SetScale(m_originInfo.GetScaleY() * m_offsetList[m_iCurrent].GetScaleY(), eMaskY);
			}
			else
			{
				m_bFinished = true;
			}
		}
	}
}