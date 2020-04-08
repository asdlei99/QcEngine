#include "pch.hpp"
#include "QcAnimatorMoveCircle.hpp"
#include "Scene\QcSceneNode.hpp"

QcAnimatorMoveCircle::QcAnimatorMoveCircle()
{
}

void QcAnimatorMoveCircle::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
    f32 fOldSpd = m_fSpeed;
	if (HandleSpeed(pSceneNode, dCurrentTime, fElapsedTime))
	{
		m_fTurnRad = (m_fSpeed+fOldSpd)*0.5f * fElapsedTime;
		if (m_fRad > ROUNDING_ERROR_f32)
		{
			m_fRad -= m_fTurnRad;
			if (m_fRad < ROUNDING_ERROR_f32)
			{
				m_fTurnRad += m_fRad;
				m_bFinished = true;
			}
		}
	}

	QcVector vec(m_ptCenter, pSceneNode->GetPosition());
	vec.rotateByRad(m_bAnticlockwise ? m_fTurnRad : -m_fTurnRad);
	vec += m_ptCenter;

	pSceneNode->SetPosition(vec);
	if (m_bSelfRotate)
	{
		//对象的方向与旋转线法向垂直
		pSceneNode->SetRotation((f32)(vec.getRadAngleTrig()) + (m_bAnticlockwise ? HALF_PI: - HALF_PI));
	}
}