#pragma once

class QiSceneNodeMonitor
{
	friend class QcSceneNode;
public:
	//virtual void OnDieNode(QcSceneNode* pNode) { OnRemoveNodeMonitor(pNode);}
	//virtual void OnDeadNode(QcSceneNode* pNode) { OnRemoveNodeMonitor(pNode);}
	virtual void OnRemoveNodeMonitor(QcSceneNode*){}
	virtual void OnAddNodeMonitor(QcSceneNode*){}
};