#include "Pch.hpp"
#include "DcWaterEffect.hpp"
#include "Res\QcTexture.hpp"

ImplRuntimeClass(DcWaterEffect);

DcWaterEffect    * DcWaterEffect::m_pInstance = NULL;

#define HEIGHT_MAP_SIZE 512
#define BACK_SIZE 2048
#define RoundHU 10
#define Radius 7.f
#define FaceNum (360/RoundHU)
#define VertexNum (FaceNum+2)

struct  stWaterPoint
{
    D3DXVECTOR4     pos;
    const static DWORD   FVF = D3DFVF_XYZRHW;
};

struct stHeightMapPoint
{
    D3DXVECTOR4     pos;
    D3DXVECTOR2     texCoord;
    const static DWORD  FVF = D3DFVF_XYZRHW|D3DFVF_TEX1;
};


DcWaterEffect::DcWaterEffect()
{
    ZeroMemory(m_pTarget, sizeof(m_pTarget));
    ZeroMemory(m_pBackBuffer, sizeof(m_pBackBuffer));

    m_fRenderTime = 0;
    m_fUpdateTime = 0;
    m_iCurTime = 0;
    m_iRef = 0;

    m_fRenderPlan = 3.f;
    m_fUpdateTick = 0.04f;
    m_fDamp = 0.95f;
    m_fForce = 1.f;
}

DcWaterEffect::~DcWaterEffect()
{
    if (m_pInstance == this)
    {
        for (int i=0; i<TEX_MAx; i++)
            SAFE_RELEASE(m_pTarget[i]);
        for (int i=0; i<2; i++)
            SAFE_RELEASE(m_pBackBuffer[i]);

        m_fRenderTime = 0;

        m_vecPos.clear();
    }
    else if (m_pInstance && --m_pInstance->m_iRef <= 0)
        SAFE_DELETE(m_pInstance);
}

void DcWaterEffect::Init( ID3DXEffect* pEffect )
{
    if (!m_pInstance)
        m_pInstance = new DcWaterEffect();

    m_bFinished = false;

    if (m_pInstance == this)
    {
        QcEffect::Init(pEffect);

        if (m_iRef++ == 0)
        {
			QmResLocker();
            LPDIRECT3DDEVICE9 pDevice = QmSence->GetRender().GetDevice();
            LPDIRECT3DSURFACE9 pOld, pNew;
            pDevice->GetRenderTarget(0, &pOld);
            for (int i=0; i<TEX_MAx; i++)
            {
                if (FAILED(D3DXCreateTexture(pDevice, HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE, 0, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pTarget[i])))
                    QmLogError("Render Target Create Failed!");
                m_pTarget[i]->GetSurfaceLevel(0, &pNew);
                pDevice->SetRenderTarget(0, pNew);
                pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
                pNew->Release();
            }

            for (int i=0; i<2; i++)
            {
                if (FAILED(D3DXCreateTexture(pDevice, BACK_SIZE, BACK_SIZE, 0, D3DUSAGE_RENDERTARGET,
                    D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pBackBuffer[i])))
                    QmLogError("Render Target Create Failed!");
                m_pBackBuffer[i]->GetSurfaceLevel(0, &pNew);
                pDevice->SetRenderTarget(0, pNew);
                pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
                pNew->Release();
            }

            pDevice->SetRenderTarget(0, pOld);
            pOld->Release();

        }
    }
    else if (m_pInstance)
        m_pInstance->Init(pEffect);
}

void DcWaterEffect::OnAnimate( QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime )
{
    if (m_pInstance == this)
    {
        if (!QcMath::equals(m_iCurTime, giFrameID))
        {
            if (m_fRenderTime > 0)
            {
                m_fRenderTime -= fElapsedTime;
                m_fUpdateTime += fElapsedTime;
            }
            m_iCurTime = giFrameID;
        }
    }
    else if (m_pInstance)
    {
        m_pInstance->OnAnimate(pSceneNode, dCurrentTime, fElapsedTime);
        m_bFinished = m_pInstance->m_bFinished;
    }
}

bool DcWaterEffect::OnRender( QcDeviceRender& render, const QcTexture* pTexture )
{
    if (m_pInstance == this)
    {
        LPDIRECT3DDEVICE9 pDevice = render.GetDevice();
        D3DXMATRIX matOldW, matOldP, matOldTex;
        pDevice->GetTransform(D3DTS_WORLD, &matOldW);
        pDevice->GetTransform(D3DTS_PROJECTION, &matOldP);
        pDevice->GetTransform(D3DTS_TEXTURE0, &matOldTex);

        LPDIRECT3DINDEXBUFFER9 pOldIB;
        LPDIRECT3DVERTEXBUFFER9 pOldVB;
        UINT uiVBOffset, uiVBStride;
        pDevice->GetIndices(&pOldIB);
        pDevice->GetStreamSource(0, &pOldVB, &uiVBOffset, &uiVBStride);

        LPDIRECT3DSURFACE9 pOldTarget;
        pDevice->GetRenderTarget(0, &pOldTarget);

        if (m_fRenderTime > 0)
        {
            while (m_fUpdateTime >= m_fUpdateTick)
            {
                AddImpulse(pDevice);

                WaterSimulation(pDevice);

                SwapHeightmaps();
                m_fUpdateTime -= m_fUpdateTick;
            }

            ConvertNormal(pDevice);
            Water(pDevice, pOldTarget);
        }
        else
        {
            if (m_fRenderTime < 0)
            {    
                LPDIRECT3DSURFACE9 pTmpSurf;
                for (int i=0; i<TEX_MAx; i++)
                {
                    m_pTarget[i]->GetSurfaceLevel(0, &pTmpSurf);
                    pDevice->SetRenderTarget(0, pTmpSurf);
                    pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
                    pTmpSurf->Release();
                }
                m_fRenderTime = 0;
                m_fUpdateTime = 0;
				m_bFinished = true;
            }
        }

        pDevice->SetTransform(D3DTS_TEXTURE0, &matOldTex);
        pDevice->SetTransform(D3DTS_PROJECTION, &matOldP);
        pDevice->SetTransform(D3DTS_WORLD, &matOldW);
        pDevice->SetFVF(QsVertex::FVF);
        pDevice->SetStreamSource(0, pOldVB, uiVBOffset, uiVBStride);
        pDevice->SetIndices(pOldIB);
        pDevice->SetRenderTarget(0, pOldTarget);

        SAFE_RELEASE(pOldTarget);
        SAFE_RELEASE(pOldVB);
        SAFE_RELEASE(pOldIB);

        return true;
    }

    else if (m_pInstance)
        return m_pInstance->OnRender(render, pTexture);

    return false;
}

bool DcWaterEffect::SetWaterPos(const D3DXVECTOR3 &pos )
{
    if (!m_pInstance)
        return false;

    m_pInstance->m_vecPos.push_back(pos);
    D3DXVECTOR3 &newpos = m_pInstance->m_vecPos.back();
    newpos.x = BACK_SIZE/2 + newpos.x;
    newpos.y = BACK_SIZE/2 - newpos.y;
    newpos.x /= BACK_SIZE/HEIGHT_MAP_SIZE;
    newpos.y /= BACK_SIZE/HEIGHT_MAP_SIZE;
    m_pInstance->m_fRenderTime = m_pInstance->m_fRenderPlan;
    return true;
}

void DcWaterEffect::AddImpulse(LPDIRECT3DDEVICE9 pDevice)
{
    if (m_vecPos.empty())
        return;

    LPDIRECT3DSURFACE9 pSurf;
    m_pTarget[TEX_HEIGHT1]->GetSurfaceLevel(0, &pSurf);

    pDevice->SetRenderTarget(0, pSurf);

    m_pEffect->SetTechnique("AddImpulse");
    m_pEffect->SetFloat("fForce", m_fForce);

    WORD Index[FaceNum*3] = {0};
    WORD nCurIndex = 1;
    for (int i=0;i<FaceNum;++i)
    {
        int n = i * 3;
        Index[n+0] = 0;
        Index[n+1] = nCurIndex;
        nCurIndex += 1;
        Index[n+2] = nCurIndex;
    }

    stWaterPoint vertex[VertexNum];
    pDevice->SetFVF( stWaterPoint::FVF );

	f32 fCs = cos(D3DXToRadian(RoundHU));
	f32 fSn = sin(D3DXToRadian(RoundHU));
	QcVector vec(1.0f, 0.f);

    UINT nPassNum = 0;
    m_pEffect->Begin(&nPassNum,0);
    for (UINT i = 0; i < nPassNum; ++i)
    {
        m_pEffect->BeginPass(i);
        m_pEffect->CommitChanges();
        for (size_t j=0; j<m_vecPos.size(); ++j)
        {
            vertex[0].pos = D3DXVECTOR4(m_vecPos[j].x, m_vecPos[j].y, 0, 1);
			vec.X = Radius;
			vec.Y = 0.f;
            for (int k=1; k<VertexNum; ++k)
            {
				vertex[k].pos.x = vertex[0].pos.x + vec.X;
				vertex[k].pos.y = vertex[0].pos.y + vec.Y;
				vertex[k].pos.z = 0.f;
				vertex[k].pos.w = 1.f;

				vec.rotateBy(fCs, fSn);
                /*vertex[k].pos = D3DXVECTOR4(vertex[0].pos.x+Radius*cosf(D3DXToRadian(iCurDeg)), 
                    vertex[0].pos.y+Radius*sinf(D3DXToRadian(iCurDeg)), 0, 1);
                iCurDeg += RoundHU;*/
            }
            pDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, VertexNum, 
                FaceNum, Index, D3DFMT_INDEX16, vertex, sizeof(stWaterPoint));
        }
        m_pEffect->EndPass();
    }
    m_pEffect->End();

    m_vecPos.clear();
    pSurf->Release();
}

void DcWaterEffect::WaterSimulation( LPDIRECT3DDEVICE9 pDevice )
{
    LPDIRECT3DSURFACE9 pSurf;
    m_pTarget[TEX_HEIGHT2]->GetSurfaceLevel(0, &pSurf);

    pDevice->SetRenderTarget(0, pSurf);

    m_pEffect->SetTechnique("WaterSimulation");
    m_pEffect->SetTexture("heightmap_prev"   ,m_pTarget[TEX_HEIGHT0]);
    m_pEffect->SetTexture("heightmap_current",m_pTarget[TEX_HEIGHT1]);

    D3DXVECTOR4 texSize(1.f/HEIGHT_MAP_SIZE, 1.f/HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE);
    m_pEffect->SetVector("texture_size", &texSize);

    m_pEffect->SetFloat("fDamping", m_fDamp);

    stHeightMapPoint v[4] = 
    {
        {D3DXVECTOR4(0, HEIGHT_MAP_SIZE, 0, 1), D3DXVECTOR2(0, 1)}, 
        {D3DXVECTOR4(0, 0, 0, 1), D3DXVECTOR2(0, 0)},
        {D3DXVECTOR4(HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE, 0, 1), D3DXVECTOR2(1, 1)},
        {D3DXVECTOR4(HEIGHT_MAP_SIZE, 0, 0, 1), D3DXVECTOR2(1, 0)}
    };
    pDevice->SetFVF(stHeightMapPoint::FVF);

    UINT nPassNum = 0;
    m_pEffect->Begin(&nPassNum,0);
    for (UINT i = 0; i < nPassNum; i++)
    {
        m_pEffect->BeginPass(i);
        m_pEffect->CommitChanges();
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(stHeightMapPoint));
        m_pEffect->EndPass();
    }
    m_pEffect->End();

    pSurf->Release();
}

void DcWaterEffect::ConvertNormal( LPDIRECT3DDEVICE9 pDevice )
{
    LPDIRECT3DSURFACE9 pSurf;
    m_pTarget[TEX_NORMAL]->GetSurfaceLevel(0, &pSurf);

    pDevice->SetRenderTarget(0, pSurf);

    m_pEffect->SetTechnique("ConvertNormal");
    m_pEffect->SetTexture("heightmap_current", m_pTarget[TEX_HEIGHT1]);
    m_pEffect->SetFloat("fNormalScale", 1.0f);

    stHeightMapPoint v[4] = 
    {
        {D3DXVECTOR4(0, HEIGHT_MAP_SIZE, 0, 1), D3DXVECTOR2(0, 1)}, 
        {D3DXVECTOR4(0, 0, 0, 1), D3DXVECTOR2(0, 0)},
        {D3DXVECTOR4(HEIGHT_MAP_SIZE, HEIGHT_MAP_SIZE, 0, 1), D3DXVECTOR2(1, 1)},
        {D3DXVECTOR4(HEIGHT_MAP_SIZE, 0, 0, 1), D3DXVECTOR2(1, 0)}
    };
    pDevice->SetFVF(stHeightMapPoint::FVF);

    UINT nPassNum = 0;
    m_pEffect->Begin(&nPassNum,0);
    for (UINT i = 0; i < nPassNum; i++)
    {
        m_pEffect->BeginPass(i);
        m_pEffect->CommitChanges();
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(stHeightMapPoint));
        m_pEffect->EndPass();
    }
    m_pEffect->End();
    
    pSurf->Release();
}

void DcWaterEffect::Water( LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DSURFACE9 pBackBuf )
{
    RECT rc = {BACK_SIZE/2-960, BACK_SIZE/2-540, BACK_SIZE/2+960, BACK_SIZE/2+540};

    LPDIRECT3DSURFACE9 pSurf;

    m_pBackBuffer[0]->GetSurfaceLevel(0, &pSurf);
    pDevice->SetRenderTarget(0, pSurf);
    pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
    pDevice->StretchRect(pBackBuf, NULL, pSurf, &rc, D3DTEXF_NONE);
    pSurf->Release();

    m_pBackBuffer[1]->GetSurfaceLevel(0, &pSurf);
    pDevice->SetRenderTarget(0, pSurf);

    m_pEffect->SetTechnique("Water");
    m_pEffect->SetTexture("NormalmapTex", m_pTarget[TEX_NORMAL]);
    m_pEffect->SetTexture("WaterTex"    , m_pBackBuffer[0]);	
    m_pEffect->SetFloat("fTexcoordScale", 0.1f);    

    stHeightMapPoint v[4] = 
    {
        {D3DXVECTOR4(0, BACK_SIZE, 0, 1), D3DXVECTOR2(0, 1)}, 
        {D3DXVECTOR4(0, 0, 0, 1), D3DXVECTOR2(0, 0)},
        {D3DXVECTOR4(BACK_SIZE, BACK_SIZE, 0, 1), D3DXVECTOR2(1, 1)},
        {D3DXVECTOR4(BACK_SIZE, 0, 0, 1), D3DXVECTOR2(1, 0)}
    };
    pDevice->SetFVF(stHeightMapPoint::FVF);

    UINT nPassNum = 0;
    m_pEffect->Begin(&nPassNum,0);
    for (UINT i = 0; i < nPassNum; i++)
    {
        m_pEffect->BeginPass(i);
        m_pEffect->CommitChanges();
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(stHeightMapPoint));
        m_pEffect->EndPass();
    }
    m_pEffect->End();

    pDevice->StretchRect(pSurf, &rc, pBackBuf, NULL, D3DTEXF_NONE);
    pSurf->Release();
}

void DcWaterEffect::SwapHeightmaps()
{
    LPDIRECT3DTEXTURE9 pTex = m_pTarget[TEX_HEIGHT0];
    m_pTarget[TEX_HEIGHT0] = m_pTarget[TEX_HEIGHT1];
    m_pTarget[TEX_HEIGHT1] = m_pTarget[TEX_HEIGHT2];
    m_pTarget[TEX_HEIGHT2] = pTex;
}

bool DcWaterEffect::SetUpdateTick( float fTick )
{
    if (!m_pInstance)
        return false;

    m_pInstance->m_fUpdateTick = fTick;
    return true;
}

bool DcWaterEffect::SetRenderTime( float fTime )
{
    if (!m_pInstance)
        return false;

    m_pInstance->m_fRenderPlan = fTime;
    return true;
}

bool DcWaterEffect::SetWaterForce( float fForce )
{
    if (!m_pInstance)
        return false;

    m_pInstance->m_fForce = fForce;
    return true;
}

bool DcWaterEffect::SetWaterDamp( float fDamp )
{
    if (!m_pInstance)
        return false;

    m_pInstance->m_fDamp = fDamp;
    return true;
}
