#pragma once

#include "EffectRender\QcEffect.hpp"
class QcHighlightEffect : public QcEffect
{
	DeclareRuntimeClass(QcHighlightEffect);
public:
	QcHighlightEffect() 
		:m_iFlag(0)
		,m_fLuminance(0.f)
		,m_fMinLuminance(0.f)
		,m_fMaxLuminance(0.f)
		,m_fUnit(0.f)
		,m_nPeriod(0)
	{}

	virtual void OnAnimate(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
	virtual bool OnRender(QcDeviceRender& render, const QcTexture* pTexture);
	virtual void OnHit();
	void FlashLuminance(f32 fMinLuminance, f32 fMaxLuminance, f32 fPeriodTime, u32 nPeriod = 0);
protected:
	int m_iFlag;
	f32 m_fLuminance;
	f32 m_fMinLuminance;
	f32 m_fMaxLuminance;
	f32 m_fUnit;
	u32 m_nPeriod;
};