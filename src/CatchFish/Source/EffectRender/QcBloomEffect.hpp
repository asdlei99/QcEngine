#pragma once
#include "EffectRender\QcEffect.hpp"

#define TARGET_WIDTH    300
#define TARGET_HEIGHT   300

class QcBloomEffect : public QcEffect
{
	DeclareRuntimeClass(QcBloomEffect);
public:
	QcBloomEffect()
		: m_fBloomCoef(1.f)
		, m_fColorFadeCoef(1.f)
	{}

	virtual void Init(ID3DXEffect* pEffect);
	virtual void OnAnimate(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
	virtual bool OnRender(QcDeviceRender& render, const QcTexture* pTexture);
	void SetBloomColor(const QcColorf& color, f32 fBloomCoef, f32 fColorFadeCoef); //系数均以正弦波处理
protected:
	QcColorHSL      m_bloomColorCenter;
	f32             m_fBloomCoef;
	f32             m_fColorFadeCoef;
	f32             m_fTime;
    f32             m_fTick;
    struct stVertex
    {
        float x,y,z;
        float u0,v0;
        float u1,v1;
        const static DWORD FVF = D3DFVF_XYZ|D3DFVF_TEX2;
    }tmpVertex[4];
};