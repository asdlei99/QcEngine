#include "Pch.hpp"
#include "QcShelterHandle.hpp"
#include "Scene\Fish\QcFish.hpp"

QcShelterHandle::QcShelterHandle(const std::vector<f32>& boundInfo)
: m_boundInfo(boundInfo)
, m_iBoundIdx(0)
, m_bEnterShelter(false)
,m_fGravitySpeed(0.f)
,m_fDestSpeed(0.f)
,m_fDestScale(0.f)
,m_fScaleSpeed(0.f)
,m_fCurrentLen(0.f)
{

}

bool QcShelterHandle::Animate(QcSceneNode* pSceneNode, f32& fSpeed, double dCurrentTime,f32 fElapsedTime)
{
	if (m_iBoundIdx >= (int)m_boundInfo.size())
		return true;

	m_fCurrentLen += fSpeed * fElapsedTime;

	QcFish* pFish = static_cast<QcFish*>(pSceneNode);
	f32 fGap = 0.f;
	if (m_fCurrentLen > m_boundInfo[m_iBoundIdx])
	{
		switch(m_iBoundIdx % 8)
		{
		case 0:
			{
				m_fSpeedSave = fSpeed;

				//1취
				f32 fGapTime = 1.0f;
				fGap = -0.001f;

				m_bEnterShelter = true;
				pSceneNode->ToShowState(eLivingToDown);
				pFish->SetDepth(0.f);

				m_fDestScale = pSceneNode->GetScaleX() * 0.9f;
				m_fScaleSpeed = -0.1f/fGapTime;

				//vt + 0.5 * a * t * t
				m_fGravitySpeed = ((m_boundInfo[m_iBoundIdx + 1] - m_fCurrentLen) - (fSpeed * fGapTime))/(0.5f * fGapTime * fGapTime);
				m_fDestSpeed = QcMath::max_(10.f, fSpeed + (m_fGravitySpeed * fGapTime));
				break;
			}
		case 1:
			{
				//3취
				f32 fGapTime = 2.0f;
				fGap = -0.001f;

				m_fDestScale *= 0.65f;
				m_fScaleSpeed = -0.3f/fGapTime;

				fSpeed = (m_boundInfo[m_iBoundIdx + 1] - m_fCurrentLen)/fGapTime;
				m_fGravitySpeed = 0.f;

				pFish->EnterShelter();
				break;
			}
		case 2:
			{
				//1취
				f32 fGapTime = 1.0f;
				fGap = -0.001f;

				m_fDestScale *= 0.7f;
				m_fScaleSpeed = -0.1f/fGapTime;

				m_fGravitySpeed = ((m_boundInfo[m_iBoundIdx + 1] - m_fCurrentLen) - (fSpeed * fGapTime))/(0.5f * fGapTime * fGapTime);
				m_fDestSpeed = QcMath::max_(10.f, fSpeed + (m_fGravitySpeed * fGapTime));

				pSceneNode->ToShowState(eDownToLiving);
				break;
			}
		case 3:
			{
				m_fGravitySpeed = 50.f;
				m_fDestSpeed = 70.f;

				fGap = 0.f;
				break;
			}
		case 4:
			{
				f32 fGapTime = 1.0f;
				fGap = 0.001f;

				m_fDestScale *= 1.0f/0.7f;
				m_fScaleSpeed = 0.1f/fGapTime;

				m_fGravitySpeed = ((m_boundInfo[m_iBoundIdx + 1] - m_fCurrentLen) - (fSpeed * fGapTime))/(0.5f * fGapTime * fGapTime);
				m_fDestSpeed = QcMath::max_(10.f, fSpeed + (m_fGravitySpeed * fGapTime));

				pSceneNode->ToShowState(eLivingToUp);
				break;
			}
		case 5:
			{
				f32 fGapTime = 2.0f;
				fGap = 0.001f;

				m_fDestScale *= 1.0f/0.65f;
				m_fScaleSpeed = 0.3f/fGapTime;

				fSpeed = (m_boundInfo[m_iBoundIdx + 1] - m_fCurrentLen)/fGapTime;
				m_fGravitySpeed = 0.f;

				//m_fGravitySpeed = ((m_boundInfo[m_iBoundIdx + 1] - m_fCurrentLen) - (fSpeed * fGapTime))/(0.5f * fGapTime * fGapTime);
				//m_fDestSpeed = QcMath::max_(10.f, fSpeed + (m_fGravitySpeed * fGapTime));
				break;
			}
		case 6:
			{
				//1취
				f32 fGapTime = 1.0f;
				fGap = 0.001f;

				m_fDestScale *= 1.0f/0.9f;
				m_fScaleSpeed = 0.1f/fGapTime;

				m_fGravitySpeed = ((m_boundInfo[m_iBoundIdx + 1] - m_fCurrentLen) - (fSpeed * fGapTime))/(0.5f * fGapTime * fGapTime);
				m_fDestSpeed = QcMath::max_(10.f, fSpeed + (m_fGravitySpeed * fGapTime));

				pSceneNode->ToShowState(eUpToLiving);
				pSceneNode->As<QcFish*>()->LeaveShelter();
				break;
			}
		case 7:
			{
				fSpeed = m_fSpeedSave;
				pFish->SetDepth(0.f);
				m_bEnterShelter = false;
				break;
			}
		}
		++m_iBoundIdx;
	}

	if (fGap != 0.f)
		pFish->SetDepth(pFish->GetDepth() + fGap);

	if (m_bEnterShelter)
	{
		if ( (m_fGravitySpeed > 0 && m_fDestSpeed > fSpeed) || (m_fGravitySpeed < 0 && m_fDestSpeed < fSpeed) )
			fSpeed += m_fGravitySpeed * fElapsedTime;

		if ( (m_fScaleSpeed > 0 && m_fDestScale > pSceneNode->GetScaleX()) || (m_fScaleSpeed < 0 && m_fDestScale < pSceneNode->GetScaleX()) )
		{
			pSceneNode->SetScale(pSceneNode->GetScaleX() + m_fScaleSpeed * fElapsedTime);
		}
	}
	return false;
}