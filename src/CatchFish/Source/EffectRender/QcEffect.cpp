#include "pch.hpp"
#include "QcEffect.hpp"
#include "Res\QcTexture.hpp"

ImplRuntimeClass(QcEffect);

QcEffect::QcEffect() 
: m_pEffect(NULL)
, m_texHandle(NULL)
, m_colorHandle(NULL)
, m_colorModeHandle(NULL)
, m_alphaModeHandle(NULL)
{
	
}

QcEffect::~QcEffect()
{ 
	
}

void QcEffect::Init(ID3DXEffect* pEffect)
{ 
	m_pEffect = pEffect; 
	m_bFinished = false; 
	SetActive(true); 

	if (m_pEffect)
	{
		m_texHandle = m_pEffect->GetParameterByName(NULL, "texSource");
		m_colorHandle = m_pEffect->GetParameterByName(NULL, "materialColor");
		m_colorModeHandle = m_pEffect->GetParameterByName(NULL, "colorOpMode");
		m_alphaModeHandle = m_pEffect->GetParameterByName(NULL, "alphaOpMode");
	}
}


bool QcEffect::OnRender(QcDeviceRender& render, const QcTexture* pTexture)
{
	bool bRet = false;
	if (IsAcitve())
	{
		UINT numPass = 0;
		if (pTexture && m_texHandle)
		{
			m_pEffect->SetTexture(m_texHandle, pTexture->GetTexture());
		}
		if (m_colorHandle)
		{
			QcColorf matColor;
			render.GetMaterialColor(matColor);
			m_pEffect->SetFloatArray(m_colorHandle, (f32*)&matColor, 4);
		}
		if (m_colorModeHandle)
		{
			DWORD value = 0;
			render.GetDevice()->GetTextureStageState(0, D3DTSS_COLOROP, &value);
			GetEffect()->SetInt(m_colorModeHandle, value);
		}
		if (m_alphaModeHandle)
		{
			DWORD value = 0;
			render.GetDevice()->GetTextureStageState(0, D3DTSS_ALPHAOP, &value);
			GetEffect()->SetInt(m_alphaModeHandle, value);
		}

		QmAssert2((m_pEffect->Begin( &numPass, 0)) == S_OK);
		for (u32 iPass = 0; iPass<numPass; ++iPass)
		{
			if (OnPreRenderPass(render, iPass))
			{
				bRet = true;
				m_pEffect->BeginPass(iPass);

				render.RenderTexture();

				m_pEffect->EndPass();
			}
		}
		m_pEffect->End();
	}
	return bRet;
}
