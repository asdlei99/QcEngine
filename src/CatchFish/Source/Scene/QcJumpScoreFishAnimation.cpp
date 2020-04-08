#include "pch.hpp"
#include "QcJumpScoreFishAnimation.hpp"

ImplRuntimeClass(QcJumpScoreFishAnimation);

QcJumpScoreFishAnimation::QcJumpScoreFishAnimation() : m_iRef(0)
{

}

void QcJumpScoreFishAnimation::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcSceneAnimation::Init(desc, bLoadSubNode);

	m_color.r = m_color.g = m_color.b = 0.0f;
	m_color.a = 0.5f;
}

void QcJumpScoreFishAnimation::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
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

void QcJumpScoreFishAnimation::Start()
{
    Reset();
    ++m_iRef;
}

void QcJumpScoreFishAnimation::Finish()
{
    if (m_iRef > 0)
        if (--m_iRef == 0)
            m_bFinished = true;
}
