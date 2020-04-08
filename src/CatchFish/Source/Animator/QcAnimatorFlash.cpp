#include "pch.hpp"
#include "QcAnimatorFlash.hpp"
#include "Scene\QcSceneNode.hpp"

void QcAnimatorFlash::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	m_fTimeCount += fElapsedTime;
	if (pSceneNode->IsVisible())
	{
		if (m_fTimeCount > m_fTimeShow)
		{
			pSceneNode->SetVisible(false);
			m_fTimeCount = 0.f;
		}
	}
	else
	{
		if (m_fTimeCount > m_fTimeHide)
		{
			pSceneNode->SetVisible(true);
			m_fTimeCount = 0.f;
		}
	}
}