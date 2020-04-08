#pragma once

#include "Scene\QcJumpScoreFishAnimation.hpp"

class QcJumpScoreFishAnimation : public QcSceneAnimation
{
	DeclareRuntimeClass(QcJumpScoreFishAnimation);
public:
	QcJumpScoreFishAnimation();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);

    void Start();
    void Finish();

private:
    int     m_iRef;
};