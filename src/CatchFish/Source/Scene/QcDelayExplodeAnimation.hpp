#pragma once

#include "Scene\QcDelayExplodeAnimation.hpp"

class QcDelayExplodeAnimation : public QcSceneAnimation
{
	DeclareRuntimeClass(QcDelayExplodeAnimation);
public:
	QcDelayExplodeAnimation();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
};