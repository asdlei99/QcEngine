#include "pch.hpp"
#include "QcSceneNodeStateMonitor.hpp"
#include "Animator\QiAnimator.hpp"
#include "Scene\QcSceneNode.hpp"
#include "Res\QcResImageDesc.hpp"

QcSceneNodeStateMonitor::QcSceneNodeStateMonitor()
{
}

void QcSceneNodeStateMonitor::StateChange(QcSceneNode* pSceneNode, QeState eRawState, QeState eNowState)
{
	const QcResImageDescPtr& pDesc = pSceneNode->GetDesc();
	const QsStateParameter* pStateParameter = pDesc->GetStateParameter(eNowState);
	if (m_pTextureAnimate.Get())
	{
		pSceneNode->RemoveAnimator(m_pTextureAnimate);
	}

	if (pStateParameter)
	{
		QiAnimator* pMonitor = SGlobalData.goAnimateCreator.MakeTextureAnimator(pStateParameter);
		m_pTextureAnimate.Set(const_cast<QiAnimator*>(pMonitor));
		pSceneNode->AddAnimator(pMonitor);
	}

	if (eNowState == eDying)
	{
		f32 fdyingTime = pDesc->GetDyingTime();
		if (fdyingTime > ROUNDING_ERROR_f32)
			pSceneNode->SetTimer(DyingTimer, fdyingTime);
	}
	if (eNowState == eDead)
	{
		pSceneNode->SetState(eUsable);
		pSceneNode->RemoveMonitor(QiSceneNodeMonitorPtr(this));
	}
}