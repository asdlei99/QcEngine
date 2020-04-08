#include "pch.hpp"
#include "QcAnimatorDither.hpp"

void QcAnimatorDither::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	m_fShakeTime -= fElapsedTime;
	if (m_fShakeTime > 0.f)
	{
		f32 fRot = Randomf(0, TWO_PI);
		QcPoint pt(m_fSourcePt);
		pt.X += cos(fRot) * m_fShakeScope;
		pt.Y += sin(fRot) * m_fShakeScope;
		pSceneNode->SetPosition(pt);
	}
	else
	{
		pSceneNode->SetPosition(m_fSourcePt);
		m_bFinished = true;
	}
}