#include "pch.hpp"
#include "QcAnimatorMovePath.hpp"
#include "Scene\QcSceneNode.hpp"
#include "Res\QcResPathDesc.hpp"

ImplRuntimeClass2(QcAnimatorMovePath, 200);


void QcAnimatorMovePath::Reset(const QcResPathDesc* pDesc, f32 fSpeed)
{
	QcAnimatorMove::Reset();

	if (m_pResPathDesc)
		m_pResPathDesc->SetUsable(true);

	m_pResPathDesc = const_cast<QcResPathDesc*>(pDesc);
	m_pResPathDesc->SetUsable(false);
	m_fSpeed = fSpeed;
	m_iIndex = 0;
	m_fCurrentLeft = m_pResPathDesc->GetSeqList()[m_iIndex].m_fLen;
}

QcAnimatorMovePath::~QcAnimatorMovePath()
{
	if (m_pResPathDesc)
		m_pResPathDesc->SetUsable(true);
}

void QcAnimatorMovePath::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	const QcList<QsPathNodeInfo>& seqList = m_pResPathDesc->GetSeqList();
    f32 fOldSpd = m_fSpeed;
	if (HandleSpeed(pSceneNode, dCurrentTime, fElapsedTime))
	{
		m_vOffset.X = 0.f;
		m_vOffset.Y = 0.f;

		Walk((m_fSpeed+fOldSpd)*0.5f * fElapsedTime);	
	}
	pSceneNode->SetRotation(seqList[m_iIndex].m_fOriental);
	pSceneNode->SetPosition(pSceneNode->GetPosition() + m_vOffset);
}

void QcAnimatorMovePath::Walk(f32 walkDist)
{
	const QcList<QsPathNodeInfo>& seqList = m_pResPathDesc->GetSeqList();
	while (walkDist > 0.f)
	{
		if (walkDist > m_fCurrentLeft)
		{
			m_vOffset += seqList[m_iIndex].m_vec * m_fCurrentLeft;
			walkDist -= m_fCurrentLeft;
			++m_iIndex;
			if (m_iIndex < seqList.size())
			{
				m_fCurrentLeft = seqList[m_iIndex].m_fLen;
			}
			else
			{
				m_bFinished = true;
				m_iIndex -= 1;
				m_fCurrentLeft = 0.f;

				break;
			}
		}
		else
		{
			m_vOffset += seqList[m_iIndex].m_vec * walkDist;
			m_fCurrentLeft -= walkDist;
			walkDist = 0.f;
		}
	}
}
