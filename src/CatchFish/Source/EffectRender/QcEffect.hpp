#pragma once

class QcEffect : public QcObject
{
	DeclareRuntimeClass(QcEffect);
public:
	QcEffect();
	virtual ~QcEffect();

	virtual void Init(ID3DXEffect* pEffect);
	virtual void OnAnimate(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime){}
	virtual bool OnRender(QcDeviceRender& render, const QcTexture* pTexture);
	virtual bool OnPreRenderPass(QcDeviceRender& render, u32 iPass){return true; }
	virtual void OnHit(){}

	void SetEffectFloat(const char* psName, f32 fValue){ GetEffect()->SetFloat(psName, fValue);}
	void SetEffectFloatArray(const char* psName, const f32* pValue, u32 nCount){ GetEffect()->SetFloatArray(psName, pValue, nCount);}
	bool IsFinish(){return m_bFinished;}
	void SetActive(bool bActive) {m_bActive = bActive;}
	bool IsAcitve() {return m_bActive;}
	ID3DXEffect* GetEffect() {return m_pEffect;}
protected:
	ID3DXEffect* m_pEffect;
	D3DXHANDLE m_texHandle;
	D3DXHANDLE m_colorHandle;
	D3DXHANDLE m_colorModeHandle;
	D3DXHANDLE m_alphaModeHandle;
	bool m_bActive;
	bool m_bFinished;
};