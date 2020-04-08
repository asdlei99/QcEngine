#pragma once

#include "Scene\QiSceneNodeMonitor.hpp"

class QcSceneNodeStateMonitor : public QiSceneNodeMonitor
{
public:
	QcSceneNodeStateMonitor();

protected:
	virtual void StateChange(QcSceneNode* pSceneNode, QeState eRawState, QeState eNowState);
	virtual void LayerChange(QcSceneNode* pSceneNode, Int32 iRawLayer, Int32 iNoewLayer){};
private:
	QiAnimatorPtr m_pTextureAnimate;
};