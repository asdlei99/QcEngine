#include "pch.hpp"
#include "QcAnimatorStretch.hpp"

void QcAnimatorStretch::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
 	f32 fLen = m_fixedPos.getDistanceFrom(pSceneNode->GetPosition());
	pSceneNode->SetScale( (fLen * 2.0f)/pSceneNode->GetWidth(), m_mxMask);
}