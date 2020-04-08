#include "Pch.hpp"
#include "QcBloomEffect.hpp"
#include "Res\QcTexture.hpp"
#include "QcEffectMgr.hpp"

ImplRuntimeClass(QcBloomEffect);

#define WORLD_WIDTH     1920
#define WORLD_HEIGHT    1080

void QcBloomEffect::Init(ID3DXEffect* pEffect)
{
	m_fTime = 0;
    m_fTick = Randomf(0.3f, 1.5f);
	QcEffect::Init(pEffect);

    SetBloomColor(QcColorf(1, 1, 0, 1), Randomf(1,2), Randomf(0.5f, 1.5f));

    stVertex v[4] = 
    {
        {-TARGET_WIDTH/2, -TARGET_HEIGHT/2, 0, 0, 1, 0, 1},
        {-TARGET_WIDTH/2, TARGET_HEIGHT/2, 0, 0, 0, 0, 0},
        {TARGET_WIDTH/2, -TARGET_HEIGHT/2, 0, 1, 1, 1, 1},
        {TARGET_WIDTH/2, TARGET_HEIGHT/2, 0, 1, 0, 1, 0}
    }; 
    memcpy_s(tmpVertex, sizeof(tmpVertex), v, sizeof(v));
}

void QcBloomEffect::OnAnimate(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	if (IsAcitve())
	{
		m_fTime += fElapsedTime*m_fTick;
	}
}

bool QcBloomEffect::OnRender(QcDeviceRender& render, const QcTexture* pTexture)
{
	if (IsAcitve())
	{
        LPDIRECT3DDEVICE9 pDevice = render.GetDevice();
        QcEffectMgr &effMgr = *QmResMgr->GetEffectMgr();
        D3DXMATRIX matP0, matW0, matTex;
        LPDIRECT3DSURFACE9 pOld;
        LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
        LPDIRECT3DINDEXBUFFER9  pIB = NULL;

        // 设置系数
        f32 fHue = (m_bloomColorCenter.Hue + sin(m_fTime*m_fColorFadeCoef)*D3DX_PI) * RADTODEG;
        m_pEffect->SetFloat("Hue", fHue);  //这里其实还需要设置色相，注意取值范围为0~360

        f32 fWeight = sin(m_fTime * m_fBloomCoef)*0.4f + 0.5f;
        //m_pEffect->SetFloat("Density", Density);        //放射阶数 
        m_pEffect->SetFloat("Weight", fWeight);          //取样权重
        //m_pEffect->SetFloat("Decay", fWeight);            //衰减系数

        //分解世界矩阵
        pDevice->GetTransform(D3DTS_WORLD, &matW0);
        D3DXVECTOR3 vScal, vTran;
        D3DXQUATERNION qRot;
        D3DXMatrixDecompose(&vScal, &qRot, &vTran, &matW0);

        //设置位置区域
        RECT rc = {(long)vTran.x+WORLD_WIDTH/2-TARGET_WIDTH/2, WORLD_HEIGHT/2-(long)vTran.y-TARGET_HEIGHT/2, 
            (long)vTran.x+WORLD_WIDTH/2+TARGET_WIDTH/2, WORLD_HEIGHT/2-(long)vTran.y+TARGET_HEIGHT/2};
        rc.left = max(rc.left, 0);
        rc.top = max(rc.top, 0);
        rc.right = min(rc.right, WORLD_WIDTH);
        rc.bottom = min(rc.bottom, WORLD_HEIGHT);

        RECT rc2 = {0, 0, TARGET_WIDTH, TARGET_HEIGHT};
        if (rc.left == 0)
        {
            rc2.left = TARGET_WIDTH-rc.right+rc.left;
            rc2.right = TARGET_WIDTH;
        }
        if (rc.top == 0)
        {
            rc2.top = TARGET_HEIGHT-rc.bottom+rc.top;
            rc2.bottom = TARGET_HEIGHT;
        }
        if (rc.right == WORLD_WIDTH)
        {
            rc2.right = rc.right-rc.left;
            rc2.left = 0;
        }
        if (rc.bottom == WORLD_HEIGHT)
        {
            rc2.bottom = rc.bottom-rc.top;
            rc2.top = 0;
        }
        const D3DSURFACE_DESC &desc = *DXUTGetD3D9BackBufferSurfaceDesc();
        /*const float fScaleW = float(desc.Width)/WORLD_WIDTH;
        const float fScaleH = float(desc.Height)/WORLD_HEIGHT;
        rc.left *= fScaleW;
        rc.right *= fScaleW;
        rc.top *= fScaleH;
        rc.bottom *= fScaleH;*/
		rc.left = (rc.left * desc.Width)/WORLD_WIDTH;
		rc.right = (rc.right * desc.Width)/WORLD_WIDTH;
		rc.top = (rc.top * desc.Height)/WORLD_HEIGHT;
		rc.bottom = (rc.bottom * desc.Height)/WORLD_HEIGHT;

        if (rc.left>=WORLD_WIDTH || rc.top>=WORLD_HEIGHT || rc.right<=0 || rc.bottom<=0)
            return false;

        //保存设备状态
        UINT uiVertexOffset, uiVertexStride;
        pDevice->GetTransform(D3DTS_PROJECTION, &matP0);
        pDevice->GetTransform(D3DTS_TEXTURE0, &matTex);
        pDevice->GetRenderTarget(0, &pOld);
        pDevice->GetStreamSource(0, &pVB, &uiVertexOffset, &uiVertexStride);
        pDevice->GetIndices(&pIB);

        //重新设置投影矩阵
        D3DXMATRIX matP1;
        D3DXMatrixOrthoLH(&matP1, TARGET_WIDTH, TARGET_HEIGHT, -1, 1);
        pDevice->SetTransform(D3DTS_PROJECTION, &matP1);

        //开始使用效果框架
        UINT uPass;
        m_pEffect->Begin(&uPass, 0);
        LPDIRECT3DSURFACE9 pNew;

        //去除平移信息，只保留旋转和缩放，使目标先渲染到Rt的中?
        D3DXMATRIX matW1;
        D3DXMatrixTransformation(&matW1, NULL, NULL, &vScal, NULL, &qRot, NULL);
        m_pEffect->SetTexture("txCombineColor", NULL);

        //第一次Pass，通过色相解决变色问题       
        m_pEffect->BeginPass(0);
        m_pEffect->SetTexture("txSourceColor", pTexture->GetTexture()); 
        m_pEffect->CommitChanges();
        LPDIRECT3DTEXTURE9 pTarget = effMgr.GetNextTarget();
        pTarget->GetSurfaceLevel(0, &pNew);
        pDevice->SetRenderTarget(0, pNew);
        pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);

		render.SetTexture(pTexture);
		pDevice->SetTransform(D3DTS_WORLD, &matW1);
		render.RenderTexture();

        m_pEffect->EndPass();
        pNew->Release();
        effMgr.FlipRt();

        //将相应位置的背景与变色后的对象混合
        LPDIRECT3DTEXTURE9 pSave = effMgr.GetRenderBack();
        pSave->GetSurfaceLevel(0, &pNew);
        pDevice->StretchRect(pOld, &rc, pNew, &rc2, D3DTEXF_NONE);
        pDevice->SetRenderTarget(0, pNew);
        pDevice->SetFVF(stVertex::FVF);
        D3DXMatrixIdentity(&matW1);
        pDevice->SetTransform(D3DTS_WORLD, &matW1);
        pDevice->SetTransform(D3DTS_TEXTURE0, &matW1);
        pDevice->SetTexture(0, pTarget);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tmpVertex, sizeof(stVertex));
        pNew->Release();

        //第二次Pass，使用Bloom实现放射性效果
        m_pEffect->BeginPass(1);
        m_pEffect->SetTexture("txSourceColor", pTarget);
        m_pEffect->CommitChanges();
        pTarget = effMgr.GetNextTarget();
        pTarget->GetSurfaceLevel(0, &pNew);
        pDevice->SetRenderTarget(0, pNew);
        pDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1, 0);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tmpVertex, sizeof(stVertex));   
        m_pEffect->EndPass();
        pNew->Release();
        effMgr.FlipRt();

        //最终混合输出回屏幕相应位置
        m_pEffect->BeginPass(2);
        m_pEffect->SetTexture("txCombineColor", pTarget);
        m_pEffect->SetTexture("txSourceColor", pSave);
        m_pEffect->CommitChanges();
        pTarget = effMgr.GetNextTarget();
        pTarget->GetSurfaceLevel(0, &pNew);
        pDevice->SetRenderTarget(0, pNew);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tmpVertex, sizeof(stVertex));
        m_pEffect->EndPass();
        pDevice->StretchRect(pNew, &rc2, pOld, &rc, D3DTEXF_NONE);
        pNew->Release();

        m_pEffect->End();

        //恢复所有
        pDevice->SetTransform(D3DTS_PROJECTION, &matP0);
        pDevice->SetTransform(D3DTS_WORLD, &matW0);
        pDevice->SetTransform(D3DTS_TEXTURE0, &matTex);
        pDevice->SetRenderTarget(0, pOld);
        pOld->Release();
        pDevice->SetStreamSource(0, pVB, uiVertexOffset, uiVertexStride);
		if (pVB)
			pVB->Release();
        pDevice->SetIndices(pIB);
		if (pIB)
			pIB->Release();
        pDevice->SetFVF(QsVertex::FVF);
        pDevice->SetTexture(0, pTexture->GetTexture());
	}
	return true;
}

void QcBloomEffect::SetBloomColor(const QcColorf& color, f32 fBloomCoef, f32 fColorFadeCoef)
{
	m_bloomColorCenter.fromRGB(color.toSColor());
    m_fBloomCoef = fBloomCoef;
    m_fColorFadeCoef = fColorFadeCoef;
}