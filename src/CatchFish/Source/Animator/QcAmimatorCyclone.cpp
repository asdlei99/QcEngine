#include "Pch.hpp"
#include "Animator/QcAmimatorCyclone.hpp"
#include "Scene\QcSceneNode.hpp"

QcAnimatorCyclone::QcAnimatorCyclone()
{

}

void QcAnimatorCyclone::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
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
		m_fTurnRadiu = m_fRadiuSpeed * fElapsedTime;
		if (m_fTime > ROUNDING_ERROR_f32)
		{
			m_fTime -= fElapsedTime;
			if (m_fTime < 0.f)
			{
				m_bFinished = true;
			}
		}
	}

	QcVector vec(m_ptCenter, pSceneNode->GetPosition());
	f32 fRadiu = vec.getLength();
	if (QcMath::equals(fRadiu, 0.f))
	{
		vec.X = cos(pSceneNode->GetRotation() + HALF_PI);
		vec.Y = sin(pSceneNode->GetRotation() + HALF_PI);
	}
	else
	{
		vec.normalize(fRadiu);
		vec.rotateByRad(m_bAnticlockwise ? m_fTurnRad : -m_fTurnRad);
	}
	fRadiu += m_fTurnRadiu;

	pSceneNode->SetPosition(m_ptCenter + vec * QcMath::clamp(fRadiu, m_fMinRadiu, m_fMaxRadiu));
	if(m_bOutScreenDied)
	{
		f32 fHalfMaxLength = QcMath::max_(pSceneNode->GetWidth() * 0.5f, pSceneNode->GetHeight() * 0.5f);
		if(	   pSceneNode->GetPosition().X < -(QmGlobalData.gScreenRect.right() + fHalfMaxLength)
			|| pSceneNode->GetPosition().X > (QmGlobalData.gScreenRect.right() + fHalfMaxLength)
			|| pSceneNode->GetPosition().Y > (QmGlobalData.gScreenRect.top() + fHalfMaxLength)
			|| pSceneNode->GetPosition().Y < -(QmGlobalData.gScreenRect.top() + fHalfMaxLength) )
		{
			pSceneNode->SetVisible(false);
		}
	}
	if (m_bSelfRotate)
	{
		//对象的方向与旋转线法向垂直
		pSceneNode->SetRotation((f32)(vec.getRadAngleTrig()) + (m_bAnticlockwise ? HALF_PI: - HALF_PI));
	}
}
