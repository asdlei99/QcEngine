#pragma once

#include "Scene\QcDigitalNode.hpp"

class QcSceneDigital : public QcSceneNode
{
	DeclareRuntimeClass(QcSceneDigital);
public:
	QcSceneDigital();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
    virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);

	void SetDigitalVisible(bool bVisible) { m_oDigitalNode.SetVisible(bVisible); }
	void SetDigitalValue(s64 value, u32 nFillChars = 0){ m_oDigitalNode.SetValue(value, nFillChars);}
	QcDigitalNode& GetDigitalNode() {return m_oDigitalNode;}
protected:
	QcDigitalNode m_oDigitalNode;
};