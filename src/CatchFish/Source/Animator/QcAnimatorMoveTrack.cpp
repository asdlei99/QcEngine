#include "pch.hpp"
#include "QcAnimatorMoveTrack.hpp"
#include "Scene\QcSceneNode.hpp"

ImplRuntimeClass2(QcAnimatorMoveTrack, 100);

QcAnimatorMoveTrack::QcAnimatorMoveTrack()
{

}

QcAnimatorMoveTrack::~QcAnimatorMoveTrack()
{
	RemoveMonitor();
}

void QcAnimatorMoveTrack::Reset(QcSceneNode* pTargetNode, f32 speed, bool bSelfRotate)
{
	//移除旧监听器
	RemoveMonitor();

	QcAnimatorMove::Reset();
	m_pTargetNode.Set(pTargetNode);
	//安装新的监听器
	if (pTargetNode)
		m_pTargetNode->AddMonitor(this);

	m_vOffset.X = 0.f;
	m_vOffset.Y = 0.f;
	m_fSpeed = speed;
	m_bSelfRotate = bSelfRotate;
}

void QcAnimatorMoveTrack::OnRemoveNodeMonitor(QcSceneNode* pTargetNode)
{
	if (m_pTargetNode)
	{
		QmAssert(pTargetNode == m_pTargetNode);
		if (pTargetNode == m_pTargetNode)
		{
			m_pTargetNode = NULL;
		}
	}
}

void QcAnimatorMoveTrack::RemoveMonitor()
{
	if (m_pTargetNode)
	{
		QcSharedPtr<QcSceneNode> save(m_pTargetNode);
		m_pTargetNode = NULL;
		save->RemoveMonitor(this);
	}
}

void QcAnimatorMoveTrack::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	if (m_pTargetNode && m_pTargetNode->IsLiving())
	{
        f32 fOldSpd = m_fSpeed;
		if (HandleSpeed(pSceneNode, dCurrentTime, fElapsedTime))
		{
			const QcPoint& targetPos = m_pTargetNode->GetPosition();
			const QcPoint& sourcePos = pSceneNode->GetPosition();
			QcVector vec(sourcePos, targetPos);
			f32 fWalk = abs((m_fSpeed+fOldSpd)*0.5f) * fElapsedTime;

			f32 fLen = vec.getLength();
			if (fLen - fWalk > ROUNDING_ERROR_f32)
			{
				vec.normalize(fLen);
				m_vOffset.normalize();
				//方向相反
				if (vec.dotProduct(m_vOffset) < -ROUNDING_ERROR_f32)
				{
					pSceneNode->SetPosition(targetPos);
					m_bFinished = true;
					RemoveMonitor();
				}
				else
				{
					m_vOffset = vec * fWalk;
					pSceneNode->SetPosition(sourcePos + m_vOffset);
				}
			}
			else
			{
				pSceneNode->SetPosition(targetPos);
				m_bFinished = true;
				RemoveMonitor();
			}
			if (m_bSelfRotate)
			{
				pSceneNode->SetRotation((f32)vec.getRadAngleTrig());
			}
		}
		else
		{
			QmAssert(false);
		}
	}
	else
	{
		m_bFinished = true;
		RemoveMonitor();
	}
}