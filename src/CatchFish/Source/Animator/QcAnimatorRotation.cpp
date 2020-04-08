#include "pch.hpp"
#include "QcAnimatorRotation.hpp"
#include "Scene\QcSceneNode.hpp"

QcAnimatorRotation::QcAnimatorRotation(f32 radSpeed, f32 fRad)
: m_fRadSpeed(radSpeed), m_fRotateRad(fRad)
{
	m_fRotateRad = abs(m_fRotateRad);
}

void QcAnimatorRotation::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	f32 fRad = m_fRadSpeed * fElapsedTime;
	if (m_fRotateRad > ROUNDING_ERROR_f32)
	{
		m_fRotateRad -= abs(fRad);
		if (m_fRotateRad < ROUNDING_ERROR_f32)
		{
			fRad += m_fRotateRad;
			m_bFinished = true;
		}
	}
	pSceneNode->SetRotation(pSceneNode->GetRotation() + fRad);
}