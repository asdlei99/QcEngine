#include "pch.hpp"
#include "QcDeviceRender.hpp"
#include "EffectRender\QcEffect.hpp"
#include "Res\QcTexture.hpp"

const DWORD QsVertex::FVF = D3DFVF_XYZ|D3DFVF_TEX1;

#define QmIndexdNum 6

QcDeviceRender::QcDeviceRender()
: m_pDevice(NULL)
, m_pIndexBuffer(NULL)
, m_pVertexBuffer(NULL)
, m_pFont(NULL)
, m_bDeviceLost(false)
, m_iCaptureShadow(0)
, m_bSuspendShadow(true)
, m_iShadowLayer(1)
, m_bMutilScreen(false)
{
}

QcDeviceRender::~QcDeviceRender()
{
	OnDestroy();
}

bool QcDeviceRender::BeginScene(bool backBuffer, bool zBuffer, const QcColor& color)
{
	if (!m_pDevice)
		return false;

	HRESULT hr;
	if (m_bDeviceLost)
	{
		if (FAILED(hr = m_pDevice->TestCooperativeLevel()))
		{
			if (hr == D3DERR_DEVICELOST)
			{
				Sleep(100);
				hr = m_pDevice->TestCooperativeLevel();
				if (hr == D3DERR_DEVICELOST)
					return false;
			}

			if ((hr == D3DERR_DEVICENOTRESET) && !Reset())
				return false;
		}
	}

	DWORD flags = 0;

	if (backBuffer)
		flags |= D3DCLEAR_TARGET;

	if (zBuffer)
		flags |= D3DCLEAR_ZBUFFER;

	if (flags)
	{
		hr = m_pDevice->Clear( 0, NULL, flags, color.color, 1.0, 0);
		if (FAILED(hr))
		{
			QmLog("DIRECT3D9 clear failed..");
			return false;
		}
	}

	hr = m_pDevice->BeginScene();
	if (FAILED(hr))
	{
		QmLog("DIRECT3D9 begin scene failed.");
		return false;
	}
	return true;
}
bool QcDeviceRender::EndScene()
{
	HRESULT hr = m_pDevice->EndScene();
	if (FAILED(hr))
	{
		QmLog("DIRECT3D9 end scene failed.");
		return false;
	}

	hr = m_pDevice->Present(NULL, NULL, NULL, NULL);
	if (SUCCEEDED(hr))
		return true;

	switch (hr)
	{
	case D3DERR_DEVICELOST:
		{
			m_bDeviceLost = true;
			QmLog("DIRECT3D9 device lost EndScene");
			break;
		}
	case D3DERR_DEVICEREMOVED:
		{
			QmLog("Device removed at EndScene");
			break;
		}
	case D3DERR_INVALIDCALL:
		{
			QmLog("Invalid Call at EndScene");
			break;
		}
	default:
		{
			QmLog("DIRECT3D9 present failed at EndScene");
			break;
		}
	}
	return true;
}


bool QcDeviceRender::Create(IDirect3DDevice9* pDevice)
{
	if (m_pDevice)
		return false;

#ifdef QmDXFontOn
	D3DXCreateFont(pDevice, 16, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"ËÎÌå", &m_pFont);
#endif

	m_pDevice = pDevice;

	pDevice->AddRef();
	pDevice->GetDeviceCaps(&m_caps);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

	SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);

	SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	m_pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_STENCILREF, 0x2);
	m_pDevice->SetRenderState(D3DRS_STENCILMASK, 0xfffffff);
	m_pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_GREATEREQUAL);
	m_pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	m_pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, eDepthFunGreater);

	m_ShadowColor = QcColorf(0.0f, 0.0f, 0.0f, 0.5f);
	m_arDelayerRenderObj.reserve(50);

	QsMaterial().ToD3DMaterial(m_material);
	pDevice->SetMaterial(&m_material);
	return true;
}

bool QcDeviceRender::CreateHardwareBuffer(IDirect3DDevice9* pDevice)
{
	HRESULT hr = 0;

#ifndef QmDynamicVertex
#if (QmVertexNum == 4)
	pDevice->GetIndices(&m_pIndexBuffer);
	if (m_pIndexBuffer != NULL)
		m_pIndexBuffer->Release();

	//Create Index Buffer;
	hr = pDevice->CreateIndexBuffer(QmIndexdNum * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndexBuffer, NULL);
	if (FAILED(hr))
		QmLogError("´´½¨Ë÷ÒýÊ§°Ü");

	WORD *pWord = NULL, szWord[QmIndexdNum] = {0, 1, 3, 1, 2, 3};
	hr = m_pIndexBuffer->Lock(0, QmIndexdNum*sizeof(WORD), (void**)&pWord, 0);
	if (FAILED(hr))
	{
		QmLogError("ÐÞ¸ÄË÷ÒýÊ§°Ü");
		m_pIndexBuffer->Release();
	}
	memcpy(pWord, szWord, 6*sizeof(WORD));
	m_pIndexBuffer->Unlock();
	pDevice->SetIndices(m_pIndexBuffer);
#endif
	
	//Create Vertex Buffer
	hr = pDevice->CreateVertexBuffer(QmVertexNum * sizeof(QsVertex), D3DUSAGE_WRITEONLY, QsVertex::FVF, D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL);
	if (FAILED(hr))
		QmLogError("´´½¨¶¥µã»º³åÊ§°Ü");

	QsVertex* pVertex = NULL;
	hr = m_pVertexBuffer->Lock(0, QmVertexNum * sizeof(QsVertex), (void**)&pVertex, NULL);
	if (FAILED(hr))
		QmLogError("ÐÞ¸Ä¶¥µã»º³åÊ§°Ü");

	f32 iScale = 1.0;
	pVertex[0].vPos = D3DXVECTOR3(-0.5f * iScale, -0.5f * iScale, 0);
	pVertex[1].vPos = D3DXVECTOR3(-0.5f * iScale, 0.5f * iScale, 0);
	pVertex[2].vPos = D3DXVECTOR3(0.5f * iScale, 0.5f * iScale, 0);
	pVertex[3].vPos = D3DXVECTOR3(0.5f * iScale, -0.5f * iScale, 0);

	pVertex[0].tu = pVertex[1].tu = pVertex[1].tv = pVertex[2].tv = 0.f;
	pVertex[0].tv = pVertex[2].tu = pVertex[3].tu = pVertex[3].tv = 1.f;

	pVertex[4] = pVertex[0];
	pVertex[5] = pVertex[2];

	m_pVertexBuffer->Unlock();
	pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(QsVertex));
#endif
	pDevice->SetFVF(QsVertex::FVF);

	//D3DXMATRIX matProjection;
	//D3DXMatrixOrthoLH(&matProjection, SGlobalData.GetSceneWidth(), SGlobalData.GetSceneHeight(), -2.0, 50.f);
	//pDevice->SetTransform(D3DTS_PROJECTION, &matProjection);

	f32 fWinWidth = QmGlobalData.gWindowSize.Width;
	f32 fWidth = QmGlobalData.gScreenRect.getWidth();
	f32 fHeight = QmGlobalData.gScreenRect.getHeight();
	if (QmSysSetting->IsTwoScreenMode())
	{
		fWidth *= 2;
		fWinWidth *= 2;
	}

	QcMatrix4f projectionMat;
	projectionMat.buildProjectionMatrixOrthoLH(fWidth, fHeight, -1.0f, 1.f);
	SetTransform(D3DTS_PROJECTION, projectionMat);


	m_bMutilScreen = (QmSysSetting->IsUionScreenMode() || QmSysSetting->IsTwoScreenMode());
	if (m_bMutilScreen)
	{
		m_arScreenView[eScreen1].setTranslation(fWidth * 0.25f, 0.f);
		m_arScreenView[eScreen2].setTranslation(-fWidth * 0.25f, 0.f);

		m_arScissorRect[eScreen1].left = int(fWinWidth * 0.5f);
		m_arScissorRect[eScreen1].top = 0;
		m_arScissorRect[eScreen1].right = (int)fWinWidth;
		m_arScissorRect[eScreen1].bottom = (int)fHeight;

		m_arScissorRect[eScreen2].left = 0;
		m_arScissorRect[eScreen2].top = 0;
		m_arScissorRect[eScreen2].right = int(fWinWidth * 0.5f);
		m_arScissorRect[eScreen2].bottom = int(fHeight);

		m_arScissorRect[QmMaxScreen].left = 0;
		m_arScissorRect[QmMaxScreen].top = 0;
		m_arScissorRect[QmMaxScreen].right = (int)fWinWidth;
		m_arScissorRect[QmMaxScreen].bottom = (int)fHeight;
	}
	return hr == S_OK; 
}

bool QcDeviceRender::Reset()
{
#ifdef QmDXFontOn
	m_pFont->OnResetDevice();
#endif

	if (FAILED(CreateHardwareBuffer(m_pDevice)))
		QmLogError("´´½¨Ó²¼þBufÊ§°Ü");

	return true;
}

//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
bool QcDeviceRender::OnLost()
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
#ifdef QmDXFontOn
	m_pFont->OnLostDevice();
#endif
	return true;
}

//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
bool QcDeviceRender::OnDestroy()
{
	SAFE_RELEASE(m_pFont);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pDevice);
	return true;
}

void QcDeviceRender::SetMaterialColor(const QcColorf& sMaterialColor)
{
	m_lastMaterialColor.r = sMaterialColor.r;
	m_lastMaterialColor.g = sMaterialColor.g;
	m_lastMaterialColor.b = sMaterialColor.b;
	m_lastMaterialColor.a *= sMaterialColor.a;

	if (!(m_iCaptureShadow && !m_bSuspendShadow))
	{
		SetMaterialColor2(m_lastMaterialColor);
	}
}

void QcDeviceRender::SetMaterialColor2(const QcColorf& sMaterialColor)
{
	m_material.Emissive.r = sMaterialColor.r;
	m_material.Emissive.g = sMaterialColor.g;
	m_material.Emissive.b = sMaterialColor.b;
	m_material.Diffuse.a = sMaterialColor.a;
	m_pDevice->SetMaterial(&m_material);
}
void QcDeviceRender::GetMaterialColor(QcColorf& color)
{
	color.r = m_material.Emissive.r;
	color.g = m_material.Emissive.g;
	color.b = m_material.Emissive.b;
	color.a = m_material.Diffuse.a;
}

void QcDeviceRender::RenderPolyLine(const QcPointList& pointList, const QcColorf& color)
{
#if 1
	struct QsPolygonInfo
	{
		D3DXVECTOR3 vPos;
		QcColorf color;
	};
	static WORD indexvertextList[1000];
	static bool bInit = false;
	if (!bInit)
	{
		for (WORD i=0; i<500; ++i)
		{
			indexvertextList[i * 2] = i;
			indexvertextList[i * 2 + 1] = i + 1;
		}
		bInit = true;
	}
	QcArray<QsPolygonInfo> vertextList(pointList.size(), true);
	for (int i=0; i<(int)vertextList.size(); ++i)
	{
		vertextList[i].vPos = D3DXVECTOR3(pointList[i].X, pointList[i].Y, 0.f);
		vertextList[i].color = color;
	}

	HRESULT hRet = NULL;
	m_pLastTexture = NULL;
	m_pDevice->SetTexture(0, NULL);

	SetTransform(D3DTS_WORLD, gsIdentityMatrix);
	//SetTransform(D3DTS_VIEW, gsIdentityMatrix);

	m_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	int nTime = vertextList.size()/500 + 1;
	for (int i=0; i<nTime; ++i)
	{
		int index = i * 500;
		int nCount = (i == nTime - 1) ? (vertextList.size() - index) : 500;
		hRet = m_pDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, nCount, nCount - 1, indexvertextList, D3DFMT_INDEX16, &(vertextList[index]), sizeof(QsPolygonInfo));
	}
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pDevice->SetFVF(QsVertex::FVF);
#else
	struct QsPolygonInfo
	{
		FLOAT x, y, z;
		DWORD color;
	};
	static WORD indexvertextList[1000];
	static bool bInit = false;
	if (!bInit)
	{
		for (WORD i=0; i<500; ++i)
		{
			indexvertextList[i * 2] = i;
			indexvertextList[i * 2 + 1] = i + 1;
		}
		bInit = true;
	}
	QsPolygonInfo vertextList[500];
	for (int i=0; i<(int)500; ++i)
	{
		vertextList[i].x = i - 250.f;
		vertextList[i].y = i - 250.f;
		vertextList[i].z = 0.f;
		//vertextList[i].h = 0.f;
		vertextList[i].color = 0xffff0000;
	}
	m_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	
	SetTransform(D3DTS_WORLD, gsIdentityMatrix);
	SetTransform(D3DTS_VIEW, gsIdentityMatrix);
	m_pDevice->SetTexture(0, NULL);
	HRESULT hRet = NULL;
	//int nTime = vertextList.size()/500 + 1;
	//for (int i=0; i<nTime; ++i)
	{
		int index = 0;
		int nCount = 500;

		hRet = m_pDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, nCount, nCount, indexvertextList, D3DFMT_INDEX16, &(vertextList[index]), sizeof(QsPolygonInfo));
	}
#endif
}

void QcDeviceRender::RenderTexture(const QcTexture* pTexture, const QcMatrix& worldMat, QcEffect* pEffect)
{
	SetTexture(pTexture);
	if (m_iCaptureShadow > 0)
	{
		QsDelayRenderInfo delayRender = {pEffect, pTexture, m_lastMaterialColor, worldMat};
		m_arDelayerRenderObj.push_back(delayRender);

		if (!m_bSuspendShadow)
		{
			delayRender.m_worldMat.Offset(QcMath::min_(40.f, 20.f + m_iShadowLayer * 5.f), QcMath::min_(20.f, 10.f + m_iShadowLayer * 5.f));

			SetTransform(D3DTS_WORLD, delayRender.m_worldMat);

			if (m_pDefaultEffect)
				m_pDefaultEffect->OnRender(*this, pTexture);
			else
				RenderTexture();
		}
	}
	else
	{
		SetTransform(D3DTS_WORLD, worldMat);
		if (pEffect == NULL)
			pEffect = m_pDefaultEffect;

		if (pEffect == NULL || !pEffect->OnRender(*this, pTexture))
		{
			RenderTexture();
		}
	}
}

void QcDeviceRender::SetTexture(const QcTexture* pTexture)
{
	if (pTexture && pTexture != m_pLastTexture)
	{
		m_pLastTexture = const_cast<QcTexture*>(pTexture);
#if 0
		QcMatrix mat(gsIdentityMatrix);
		mat._31 = pTexture->GetUPos();
		mat._32 = pTexture->GetVPos();
		mat._11 = pTexture->GetULen();
		mat._22 = pTexture->GetVLen();
		if (m_pDevice->SetTransform(D3DTS_TEXTURE0, ToD3DMatrix(mat)) == D3DERR_INVALIDCALL)
			QmLogError("SetTexTransform");
#endif
		m_pDevice->SetTexture(0, pTexture->GetTexture());
	}
}

void QcDeviceRender::RenderTexture()
{
	QmProfileFragment(RenderTexture);
#ifdef QmDynamicVertex

	static QsVertex gsVertex[6] = 
	{
		{D3DXVECTOR3(-0.5f, 0.5f, 0.f), 0.f, 1.f},
		{D3DXVECTOR3(0.5f, 0.5f, 0.f), 0.f, 1.f},
		{D3DXVECTOR3(0.5f, -0.5f, 0.f), 0.f, 1.f},
		{D3DXVECTOR3(-0.5f, -0.5f, 0.f), 0.f, 1.f},
		{D3DXVECTOR3(-0.5f, 0.5f, 0.f), 0.f, 1.f},
		{D3DXVECTOR3(0.5f, -0.5f, 0.f), 0.f, 1.f},
	};

	f32 fLeft = 0.f;
	f32 fTop = 0.f;
	f32 fRight = 1.0f;
	f32 fBottom = 1.0f;
	if (m_pLastTexture)
	{
		fLeft = m_pLastTexture->GetUPos();
		fTop = m_pLastTexture->GetVPos();
		fRight = fLeft + m_pLastTexture->GetULen();
		fBottom = fTop + m_pLastTexture->GetVLen();
	}

	gsVertex[0].tu = gsVertex[3].tu = fLeft;
	gsVertex[0].tv = gsVertex[1].tv = fTop;

	gsVertex[1].tu = gsVertex[2].tu = fRight;
	gsVertex[2].tv = gsVertex[3].tv = fBottom;

	gsVertex[4] = gsVertex[0];
	gsVertex[5] = gsVertex[2];

	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, gsVertex, sizeof(QsVertex));

#else

#if (QmVertexNum == 4)
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
#else
	m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
#endif

#endif

}

void QcDeviceRender::BeginCaptureShadow(int iLayer)
{
	m_iShadowLayer = iLayer;
	++m_iCaptureShadow;
	m_pDevice->SetRenderState(D3DRS_STENCILREF, (0xFF & m_iShadowLayer + 1));

	if (m_iCaptureShadow == 1)
	{
		SuspendShadow(false);
	}
}

bool QcDeviceRender::SuspendShadow(bool bSuspend)
{
	if (bSuspend)
	{
		SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
	else if (m_iCaptureShadow > 0)
	{
		SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		m_pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		SetMaterialColor2(m_ShadowColor);
	}
	bool bRet = m_bSuspendShadow;
	m_bSuspendShadow = bSuspend;
	return bRet;
}
void QcDeviceRender::EndCaptureShadow()
{
	if (m_iCaptureShadow > 0)
	{
		--m_iCaptureShadow;
		if (m_iCaptureShadow == 0)
		{
			SuspendShadow(true);

			std::vector<QsDelayRenderInfo>::iterator iter(m_arDelayerRenderObj.begin());
			std::vector<QsDelayRenderInfo>::iterator endIter(m_arDelayerRenderObj.end());
			for (; iter != endIter; ++iter)
			{
				SetMaterialColor2(iter->m_materialColor);
				RenderTexture(iter->m_pTexture, iter->m_worldMat, iter->m_pEffect);
			}
			m_arDelayerRenderObj.clear();
		}
	}
}

void QcDeviceRender::SwitchToSubScreenInner(int iScreen)
{
	switch(iScreen)
	{
	case eScreen1:
		{
			SetTransform(D3DTS_VIEW, m_arScreenView[eScreen1]);
			m_pDevice->SetScissorRect(m_arScissorRect + eScreen1);
			break;
		}
	case eScreen2:
		{
			SetTransform(D3DTS_VIEW, m_arScreenView[eScreen2]);
			m_pDevice->SetScissorRect(m_arScissorRect + eScreen2);
			break;
		}
	default:
		{
			SetTransform(D3DTS_VIEW, gsIdentityMatrix);
			m_pDevice->SetScissorRect(m_arScissorRect + QmMaxScreen);
			break;
		}
	}
}

void QcDeviceRender::PostRenderScene()
{
	
}

void QcDeviceRender::RenderText(const char* psStr, const QcRect& rect, DWORD format, const QcColorf& color)
{
	RECT rct = {(LONG)rect.left(), (LONG)rect.top(), (LONG)rect.right(), (LONG)rect.bottom()};
    if (!m_pFont)
    {
        D3DXCreateFont(m_pDevice, 16, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            L"ËÎÌå", &m_pFont);
    }
	m_pFont->DrawTextA(NULL, psStr, -1, &rct, format, D3DXCOLOR(color.r, color.g, color.b, color.a));
}

UINT QcDeviceRender::GetAvailableTextureMem()
{
	return GetDevice()->GetAvailableTextureMem();
}


#ifdef QmTestVersion
void QcDeviceRender::ScreenCapture(const char *pFileName)
{
    if (!pFileName) return;

    LPDIRECT3DSURFACE9 pSurf;
    m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurf);
    D3DXSaveSurfaceToFileA(pFileName, D3DXIFF_PNG, pSurf, NULL, NULL);
    pSurf->Release();
}
#endif
