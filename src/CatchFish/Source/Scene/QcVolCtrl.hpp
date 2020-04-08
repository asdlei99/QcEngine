#ifndef QcVolCtrl_h__
#define QcVolCtrl_h__
#include "Scene/QcSceneNode.hpp"

class QcVolCtrl : public QcSceneNode
{
	typedef QcSceneNode QcBaseClass;
	DeclareRuntimeClass(QcVolCtrl);
public:
	QcVolCtrl();
	~QcVolCtrl();
	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void OnKeyDown(u32 key, const QsKeyInfo& info);
private:
	f32 m_MyDisplayTime;
	f32 m_VolumnLevel;
	f32 m_keyDaleyTime;
	QcSceneNodePtrList m_arPointList;
	QcSceneAnimationPtr m_pNumNode;
};

#endif // QcVolCtrl_h__