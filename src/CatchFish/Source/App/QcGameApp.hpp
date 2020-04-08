#pragma once

#include "App\QcAppConfig.hpp"
#include "App\QcInputMgr.hpp"

typedef u32 QcHookerHandle;
typedef boost::function<bool (const QsRecvUsbData& data)> QcHookIOMsgCb;

class QcScene;
class QcGameApp : public QcApp
{
public:
	QcGameApp();
	virtual ~QcGameApp(void);

	int Initialize(bool bNeedWaitProgram);

	static bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
		bool bWindowed, void* pUserContext );
	static HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext );
	static HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext );
	static void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
	static void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
	static LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext );
	static void CALLBACK OnD3D9LostDevice( void* pUserContext );
	static void CALLBACK OnD3D9DestroyDevice( void* pUserContext );
	//static BOOL InitApp();
	//static void RenderText( double fTime );
	static bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
	//static void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
	//static void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );

	virtual QcScene* MakeSceneInstance();
	virtual void MainLoop();
	virtual void ExitApp(bool bTerminateProcess = true);

	void PollJoysticks();
	void FinishLoading();

	void AddIOHooker(const QcHookIOMsgCb& cb);

	QcScene* GetScene() { return m_pScene.Get(); }
	QcAppConfig& GetConfig() {return m_appConfig;}
protected:
	void PeekIOMessage();
private:
	QcAppConfig m_appConfig;
	QcSharedPtr<QcInputMgr> m_pInputMgr;
	QcSharedPtr<QcScene> m_pScene;
	std::vector<QcHookIOMsgCb> m_arIOHookerList;
	bool m_bFinishLoading;
};