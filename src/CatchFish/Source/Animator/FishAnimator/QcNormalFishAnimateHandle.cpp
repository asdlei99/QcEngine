#include "pch.hpp"
#include "QcNormalFishAnimateHandle.hpp"
#include "Scene\Fish\QcFish.hpp"

ImplRuntimeClass2(QcNormalFishAnimateHandle, 100);

QmRegisterConstVar2(f32, gfPlayingProbability, 1.0f, 0.f, 1.0f, 0.02f);


QcNormalFishAnimateHandle::QcNormalFishAnimateHandle(f32 fMinSpeed, f32 fMaxSpeed, f32 fMinGapTime, f32 fMaxGapTime, u32 playingFlag)
: m_fMinSpeed(fMinSpeed)
, m_fMaxSpeed(fMaxSpeed)
, m_fMinGapTime(fMinGapTime)
, m_fMaxGapTime(fMaxGapTime)
, m_fTimeCount(fMaxGapTime)
, m_fGravity(0.f)
, m_fPlayingTime(0.f)
, m_fPlayingTimeCount(0.f)
, m_iPlayingFlag(playingFlag)
{
}

void QcNormalFishAnimateHandle::OnHit(f32 fHitTime)
{
	if ( (m_iPlayingFlag & QcFish::eHitPlay) && m_fPlayingTimeCount >= m_fPlayingTime)
	{
		m_eToState = ePlaying;
		m_fPlayingTimeCount = 0.f;
		m_fPlayingTime = fHitTime + 0.8f;
	}
}

bool QcNormalFishAnimateHandle::Animate(QcSceneNode* pSceneNode, f32& fSpeed, double dCurrentTime,f32 fElapsedTime)
{
	if (!QcMath::equals(giFrameID, m_iCurrentFrame))
	{
		m_iCurrentFrame = giFrameID;
		if (m_fTimeCount > 0.f)
			m_fTimeCount -= fElapsedTime;

		//打中非惯性处理
		if ( (m_iPlayingFlag & QcFish::eHitPlay) && m_fPlayingTimeCount < m_fPlayingTime && Randomf() < gfPlayingProbability)
		{
			m_fPlayingTimeCount += fElapsedTime;
			if (m_eToState != eStateNone)
			{
				if (pSceneNode->GetDesc()->GetStateParameter(m_eToState))
					pSceneNode->SetState(m_eToState);
				m_eToState = eStateNone;
			}

			if (m_iPlayingFlag & QcFish::eSpeedUp)
			{
				//60%加速
				if (m_fPlayingTimeCount < m_fPlayingTime * 0.6f)
				{
					m_fDestSpeed = 300.f;
					m_fGravity = 500.f;
				}
				//再恢复正常
				else
				{
					m_fDestSpeed = 50.f;
					m_fGravity = -500.f;
				}
			}
			else if (m_iPlayingFlag & QcFish::eSpeedDownToUp)
			{
				//即降速
				if (m_fPlayingTimeCount < m_fPlayingTime * 0.3f)
				{
					m_fDestSpeed = 10.f;
					m_fGravity = -500.f;
				}
				//即加速
				else if (m_fPlayingTimeCount < m_fPlayingTime * 0.7f)
				{
					m_fDestSpeed = 350.f;
					m_fGravity = 500.f;
				}
				else
				{
					m_fDestSpeed = 50.f;
					m_fGravity = -500.f;
				}
			}
		}
		//随机
		else
		{
			if (m_fTimeCount < 0.f)
			{
				m_fDestSpeed = Randomf(m_fMinSpeed, m_fMaxSpeed);
				m_fTimeCount = Randomf(m_fMinGapTime, m_fMaxGapTime);
				if (pSceneNode->GetState() == eLiving && Randomf() < 0.2f && (m_iPlayingFlag & QcFish::eHitPlay) == 0)
				{
					m_eToState = pSceneNode->As<QcFish*>()->TryToPlayingState();
					m_fTimeCount = QcMath::max_(m_fMinGapTime, 1.5f);
					m_fDestSpeed = m_fMaxSpeed;
				}
				else if (pSceneNode->GetState() == ePlaying)
				{
					m_fDestSpeed = m_fMinSpeed;
					m_eToState = pSceneNode->ToShowState(eLiving);
				}
				m_fGravity = (m_fDestSpeed - fSpeed)/(m_fTimeCount * 0.2f);
			}
		}
		if (m_fGravity < 0.f)
		{
			if (fSpeed > m_fDestSpeed)
				fSpeed += m_fGravity * fElapsedTime;
			else
				m_fGravity = 0.f;
		}
		else if (m_fGravity > 0.f)
		{
			if (fSpeed < m_fDestSpeed)
				fSpeed += m_fGravity * fElapsedTime;
			else
				m_fGravity = 0.f;
		}
		m_fLastSpeed = fSpeed;
	}
	else
	{
		if (m_eToState != eStateNone)
		{
			pSceneNode->ToShowState(m_eToState);
		}
		fSpeed = m_fLastSpeed;
	}
	return false;

}