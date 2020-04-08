#pragma once

#include "Scene\QcSceneAnimation.hpp"
class QcBackGround;
class DcWaterEffect;
class QcSceneLogo : public QcSceneAnimation
{
	typedef QcSceneAnimation QcBaseClass;
	DeclareRuntimeClass(QcSceneLogo);
public:
	QcSceneLogo(){}

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);

	void Start(const QcBackGround* pSceneNode, const QcBackGround* pMirrorBg);

    void OnTimer(u32 nTimer, void* pContext);
protected:
	void FinishAnimator(u32 iParm);
private:
	QcSharedPtr<QcBackGround> m_pNextBackGround;
	QcSharedPtr<QcBackGround> m_pNextMirrorBackGround;
    QcSharedPtr<DcWaterEffect> m_pWaterEffect;
};