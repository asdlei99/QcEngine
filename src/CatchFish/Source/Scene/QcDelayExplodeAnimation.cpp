#include "pch.hpp"
#include "QcDelayExplodeAnimation.hpp"

ImplRuntimeClass(QcDelayExplodeAnimation);

QcDelayExplodeAnimation::QcDelayExplodeAnimation()
{

}

void QcDelayExplodeAnimation::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcSceneAnimation::Init(desc, bLoadSubNode);

	m_color.r = m_color.g = m_color.b = 0.f;

	AddAnimator(QmAnimatorCreator->MakeAlphaAnimator(0.1f, 0.5f, 1.f, true));
}

void QcDelayExplodeAnimation::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	QmAutoSuspendShadow(render);
	render.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	render.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	QcMatrix mat;
	mat.setScale(QmGlobalData.gScreenRect.getWidth(), QmGlobalData.gScreenRect.getHeight());
	render.SetMaterialColor(m_color);
	render.RenderTexture(NULL, mat);

	render.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	render.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}