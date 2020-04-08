#pragma once

#include "RenderSystem\QsMaterial.hpp"

class QcTexture;
class QcEffect;
class QcEffectMgr;
#include "EffectRender\QcEffect.hpp"

//#define QmDynamicVertex

struct QsVertex
{
	D3DXVECTOR3 vPos;
	float tu,tv;
	static const DWORD FVF;
};
#define QmVertexNum 6

enum QeComparisonFun
{
	eDepthFunNever = D3DCMP_NEVER,
	eDepthFunLessEqual = D3DCMP_LESSEQUAL,
	eDepthFunEqual = D3DCMP_EQUAL,
	eDepthFunLess = D3DCMP_LESS,
	eDepthFunNotEqual = D3DCMP_NOTEQUAL,
	eDepthFunGreaterEqual = D3DCMP_GREATEREQUAL,
	eDepthFunGreater = D3DCMP_GREATER,
	eDepthFunAlways = D3DCMP_ALWAYS,
};

#define QmAutoRevertAlpha(Render) QmAutoRevert(f32, Render.GetAlpha())
#define QmAutoSuspendShadow(Render) QmAutoRevertAlpha(Render); QcSuspendShadowHelper QmUniqueVarName##2(Render)
 
class QcDeviceRender : public QcObject
{
	QmDisableCopyAssignment(QcDeviceRender);
	struct QsDelayRenderInfo
	{
		QcEffect* m_pEffect;
		const QcTexture* m_pTexture;
		QcColorf m_materialColor;
		QcMatrix m_worldMat;
	};
public:
	explicit QcDeviceRender();
	virtual ~QcDeviceRender();

	void RenderPolyLine(const QcPointList& pointList, const QcColorf& color);
	void RenderTexture(const QcTexture* pTexture, const QcMatrix& worldMat, QcEffect* pEffect = NULL);
	void RenderTexture();
	void RenderText(const char* psStr, const QcRect& rect, DWORD Format = DT_LEFT | DT_NOCLIP, const QcColorf& color = QcColorf(1.0f, 1.0f, 0.0f));

	void SetTransform(D3DTRANSFORMSTATETYPE State, const QcMatrix4f& worldMat){ m_pDevice->SetTransform(State, ToD3DMatrix(worldMat));}
	void SetTransform(D3DTRANSFORMSTATETYPE State, const QcMatrix3f& worldMat)
	{ 
		const f32* data = worldMat.pointer();

		D3DMATRIX mat = 
		{
			data[0],data[1], 0,		0,
			data[3],data[4], 0,		0,
			0,		0,		 1,		0,
			data[6],data[7], 0,		1
		};
		m_pDevice->SetTransform(State, &mat);
	}
	void SetMaterialColor(const QcColorf& sMaterialColor);
	void GetMaterialColor(QcColorf& color);
	void SetTexture(const QcTexture* pTexture);
	u32 GetRenderState(u32 iType){ DWORD dwValue; m_pDevice->GetRenderState(D3DRENDERSTATETYPE(iType), &dwValue); return dwValue;}
	void SetRenderState(u32 iType, u32 value) { m_pDevice->SetRenderState(D3DRENDERSTATETYPE(iType), value);}
	u32 GetTextureStageState(int iStage, u32 Type) {DWORD dwValue; m_pDevice->GetTextureStageState(iStage, D3DTEXTURESTAGESTATETYPE(Type), &dwValue); return dwValue;}
	void SetTextureStageState(int iStage, u32 Type,u32 Value){ m_pDevice->SetTextureStageState(iStage, D3DTEXTURESTAGESTATETYPE(Type), Value);}
	void SetDefaultEffect(QcEffect* pEffect){ m_pDefaultEffect = pEffect;}

	f32& GetAlpha(){ return m_lastMaterialColor.a;}

	void BeginCaptureShadow(int iLayer);
	void EndCaptureShadow();
	bool SuspendShadow(bool bSuspend);
	void PostRenderScene();

	bool Create(IDirect3DDevice9* pDevice);
	bool Reset();
	bool OnLost();
	bool OnDestroy();
	IDirect3DDevice9* GetDevice() {return m_pDevice; }
	UINT GetAvailableTextureMem();
#ifdef QmTestVersion
    void ScreenCapture(const char *pFileName);
#endif

	void SwitchToSubScreen(int iScreen) {if (!m_bMutilScreen) return; SwitchToSubScreenInner(iScreen);}
private:
	void SwitchToSubScreenInner(int iScreen);
	HRESULT GetDeviceIDFromHMonitor( HMONITOR hm, WCHAR* strDeviceID, int cchDeviceID );

	bool BeginScene(bool backBuffer, bool zBuffer, const QcColor& color); //暂时未使用
	bool EndScene();													  //暂时未使用
	bool CreateHardwareBuffer(IDirect3DDevice9* pDevice);
	const D3DMATRIX* ToD3DMatrix(const QcMatrix4f& mat) const{ return (const D3DMATRIX*)(void*)(&mat);}
	void SetMaterialColor2(const QcColorf& sMaterialColor);
private:
	IDirect3DIndexBuffer9* m_pIndexBuffer;
	IDirect3DVertexBuffer9* m_pVertexBuffer;
	IDirect3DDevice9* m_pDevice;
	ID3DXFont*	m_pFont;

	bool m_bDeviceLost;

	QcSharedPtr<QcEffect> m_pDefaultEffect;
	QcTexture* m_pLastTexture;
	QcColorf m_lastMaterialColor;
	QcColorf m_ShadowColor;
	D3DMATERIAL9 m_material;
	std::vector<QsDelayRenderInfo> m_arDelayerRenderObj;
	s32 m_iShadowLayer;
	s32 m_iCaptureShadow;
	bool m_bSuspendShadow;

	QcMatrix4f m_arScreenView[QmMaxScreen];
	RECT m_arScissorRect[QmMaxScreen + 1];
	bool m_bMutilScreen;

	D3DCAPS9 m_caps;
};

class QcSuspendShadowHelper
{
	QmDisableCopyAssignment(QcSuspendShadowHelper);
public:
	QcSuspendShadowHelper(QcDeviceRender& render)
		: m_render(render)
	{
		render.SuspendShadow(true);
	}
	~QcSuspendShadowHelper()
	{
		m_render.SuspendShadow(false);
	}
protected:
	QcDeviceRender& m_render;
};