#include "pch.hpp"
#include "QcGameApp.hpp"
#include "QcGlobalData.hpp"
#include "Scene\QcScene.hpp"
#include "Res\QcResManager.hpp"
#include "Res\QcResPackLoader.hpp"
#include "Thread\QcLock.hpp"
#include "Common\QcMediaFileFinder.hpp"
#include "Common\QcEncryptXmlLoader.hpp"

class QcUsbError : public QcObject
{
public:
	QcUsbError() : m_pFont(NULL) {}
	~QcUsbError() { SAFE_RELEASE(m_pFont);}

	void Run(QcGameApp* pApp)
	{
		DXUTCreateDevice(IsDebuggerPresent() ? true : false, 640, 480);
		
		D3DXCreateFont(DXUTGetD3D9Device(), 20, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			L"宋体", &m_pFont);


		DXUTSetCallbackD3D9FrameRender(OnRenderError, this);

		pApp->MainLoop();
		
	}
	static void CALLBACK OnRenderError( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
	{
		HRESULT hr;

		V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0) );

		// Render the scene
		if( SUCCEEDED( pd3dDevice->BeginScene() ) )
		{
			RECT rect = {0, 0, 640, 480};
			static_cast<QcUsbError*>(pUserContext)->m_pFont->DrawTextA(NULL, "校验失败", -1, &rect, DT_CENTER | DT_NOCLIP | DT_VCENTER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			V( pd3dDevice->EndScene() );
		}
	}
protected:
	
protected:
	ID3DXFont* m_pFont;
};

QcGameApp::QcGameApp()
{
	m_bFinishLoading = false;
}
QcGameApp::~QcGameApp()
{
	
}

INT QcGameApp::Initialize(bool bNeedWaitProgram)
{	
#ifndef QmTestVersion
	if (IsDebuggerPresent())
		ExitApp();
#else
	if (IsDebuggerPresent())
		bNeedWaitProgram = false;
#endif
	//RedirectIOToConsole();

	// Show the cursor and clip it when in full screen
	DXUTSetCursorSettings(false, false);

	DXUTInit(false, true); // Parse the command line, handle the default hotkeys, and show msgboxes
	DXUTSetHotkeyHandling(true, false, true);
	DXUTCreateWindow(L"CatchFish");

	DWORD key[4] = {0};
	if (InitCom() && GetEnKey(1, key) == 0)
	{
		QmGlobalData.SetEncryptKey(key, sizeof(key));
#ifdef QmIOUsbOn
		if (!QmGlobalData.m_pFilePacker->Open(PackName))
#endif
		{	
			QmGlobalData.m_pFilePacker = NULL;
		}
		// Set DXUT callbacks
		DXUTSetCallbackD3D9DeviceAcceptable(IsD3D9DeviceAcceptable, this);
		DXUTSetCallbackD3D9DeviceCreated(OnD3D9CreateDevice, this);
		DXUTSetCallbackD3D9DeviceReset(OnD3D9ResetDevice, this);
		DXUTSetCallbackD3D9FrameRender(OnD3D9FrameRender, this);
		DXUTSetCallbackD3D9DeviceLost(OnD3D9LostDevice, this);
		DXUTSetCallbackD3D9DeviceDestroyed(OnD3D9DestroyDevice, this);
		DXUTSetCallbackMsgProc(MsgProc, this);
		DXUTSetCallbackFrameMove(OnFrameMove, this);
		DXUTSetCallbackDeviceChanging(ModifyDeviceSettings, this);

		m_pScene = MakeSceneInstance();

		if (bNeedWaitProgram)
		{
			QcString waitProgram;
			if (QcMediaFileFinderInstance->FindFile("WaitProgram.exe", waitProgram))
			{
				char paraBuf[512] = {0};
				char exePath[MAX_PATH] = {0};
				GetModuleFileNameA(NULL, exePath, MAX_PATH);
				sprintf_s(paraBuf, "LogPath=%s;exePath=%s;ScreenMode=%d", QmLogPath, exePath, (QmSysSetting->IsUionScreenMode() || QmSysSetting->IsTwoScreenMode()) ? 1 : 0);
				QmLog("paraBuf=%s", paraBuf);
				QfExcuteShell(waitProgram.c_str(), paraBuf, 0);
				ShutDownIOThread();
				m_pScene = NULL;
				return 0;
			}
			else
			{
				QmLog("Could not find WaitProgram.exe");
			}
		}

		//load config.
		m_appConfig.LoadFromFile("ResConfig.xml");
		m_pScene->InitApp(m_appConfig);

		HRESULT hr;
		int width = (int)QmGlobalData.gWindowSize.Width;
		int height = (int)QmGlobalData.gWindowSize.Height;
		if (QmSysSetting->IsTwoScreenMode())
			width *= 2;

		if ((hr = DXUTCreateDevice(!m_appConfig.IsFullScreen(), width, height)) != S_OK)
		{
			QmLog("Fail To DXUTCreateDevice. error code:0x=%0x 0d=%d", hr, hr);
		}
#ifdef QmTestVersion
		if (m_appConfig.IsFullScreen())
			LockSetForegroundWindow(LSFW_LOCK);
#endif
		m_pScene->LoadResouce();
		MainLoop();
		ShutDownIOThread();
		QcThread::ShutDownAllThread();

		m_pScene->OnLostDevice();
		m_pScene->OnDestroyDevice();
		m_pInputMgr.Set(NULL);
		m_pScene = NULL;
		QmSoundMgr->Reset();
	}
	else
	{
		QmLog("校验失败");
#ifdef QmIOUsbOn
		ShutDownIOThread();
#endif
		QcUsbError error;
		error.Run(this);
		QcThread::ShutDownAllThread();
	}
	DXUTShutdown(0);

	ExitApp(false);
	return 0;
}

bool QcGameApp::IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
									   bool bWindowed, void* pUserContext )
{
	// No fallback defined by this app, so reject any device that 
	// doesn't support at least ps2.0
	if( pCaps->PixelShaderVersion < D3DPS_VERSION( 2, 0 ) )
		return false;

	// Skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3D9Object();
	if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
		D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
		return false;

	return true;
}

HRESULT QcGameApp::OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	static_cast<QcGameApp*>(pUserContext)->GetScene()->OnCreateDevice(pd3dDevice);
	return S_OK;
}
HRESULT QcGameApp::OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	static_cast<QcGameApp*>(pUserContext)->GetScene()->OnResetDevice(pd3dDevice);
	return S_OK;
}

void QcGameApp::OnD3D9LostDevice(void* pUserContext)
{
}
void QcGameApp::OnD3D9DestroyDevice(void* pUserContext)
{
}

void QcGameApp::OnFrameMove( double fTime, float fElapsedTime, void* pUserContext)
{
#ifdef QmTestVersion
	if (QmDebugHelper.OnFrameMove())
#endif
    {
        ++giFrameID;
		static_cast<QcGameApp*>(pUserContext)->GetScene()->StartFrame(fTime, fElapsedTime);
	}
}
void QcGameApp::OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
	HRESULT hr;
	V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, 0x00000000, 0.0f, 0));

	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		static_cast<QcGameApp*>(pUserContext)->GetScene()->Draw(fElapsedTime);
		V( pd3dDevice->EndScene() );
	}
}

LRESULT QcGameApp::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
#ifdef QmTestVersion
	//if (static_cast<QcGameApp*>(pUserContext)->m_bFinishLoading)
	{
		if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
		{
			QmDebugHelper.OnMouseEvent(uMsg, wParam, lParam);
		}
		else
		{
			switch(uMsg)
			{
			case WM_SYSCOMMAND:
				if (wParam == SC_CLOSE || wParam == SC_MAXIMIZE)
					*pbNoFurtherProcessing = true;
				break;
			case WM_KEYUP:
			case WM_SYSKEYUP:
				{
					*pbNoFurtherProcessing = true;
					QmDebugHelper.OnWinKeyUp(wParam);
					break;
				}
			}
		}		
	}
#endif
	static bool bFullScreen = static_cast<QcGameApp*>(pUserContext)->GetConfig().IsFullScreen();
	if (bFullScreen && giExitFlag == 0)
	{
		switch(uMsg)
		{
		case WA_INACTIVE:
			{
				if (LOWORD(wParam) == WA_INACTIVE)
				{
					QmLog("WA_INACTIVE");
					//*pbNoFurtherProcessing = true;
				}
				break;
			}
		case WM_ACTIVATEAPP:
			{
				if (wParam == FALSE)
				{
					QmLog("WM_ACTIVATEAPP");
					//*pbNoFurtherProcessing = true;
				}
				break;
			}
		}
	}
	return 0;
}

bool QcGameApp::ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	if (static_cast<QcGameApp*>(pUserContext)->GetConfig().IsImmediatelPresent())
		pDeviceSettings->d3d9.pp.PresentationInterval =  D3DPRESENT_INTERVAL_IMMEDIATE;
	return true;
}

QcScene* QcGameApp::MakeSceneInstance()
{
	return new QcScene();
}

void QcGameApp::MainLoop()
{
	// Now we're ready to receive and process Windows messages.
	MSG msg;
	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	HWND hWnd = DXUTGetHWND();
	for (;;)
	{
		// Use PeekMessage() so we can use idle time to render the scene. 
		while (giExitFlag == 0 && PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0)
		{
			DispatchMessage( &msg );
#ifndef QmTestVersion
			if (GetConfig().IsFullScreen() && msg.message == WM_KEYDOWN || msg.message==WM_SYSKEYDOWN)
			{
				giExitFlag = 1;
				break;
			}
#endif
		}
		if (msg.message == WM_QUIT || giExitFlag)
		{
			break;
		}

#ifdef QmTestVersion
		QmDebugHelper.Frame();
#endif
		{
			QcEventHandle::DispatchAllEvent();
		}
		bool bActive = GetActiveWindow() == hWnd; 
		if (m_bFinishLoading && bActive && m_pInputMgr)
		{
			m_pInputMgr->Poll();
		}
		PeekIOMessage();

		if (!m_bFinishLoading || bActive)
		{
			if (!m_bFinishLoading && GetScene())
			{
				{
					QmResLocker();
					DXUTRender3DEnvironment();
				}
				::Sleep(50);
			}
			else
			{
				QmCheckDeadLoop();
				DXUTRender3DEnvironment();
			}
		}
	}
	if (::IsWindow(hWnd))
		SendMessage(hWnd, WM_CLOSE, 0, 0);
}

void QcGameApp::FinishLoading()
{
	m_pInputMgr = new QcInputMgr(m_pScene.Get());
	m_bFinishLoading = true;
}


void QcGameApp::AddIOHooker(const QcHookIOMsgCb& cb)
{
	m_arIOHookerList.push_back(cb);
}

void QcGameApp::PeekIOMessage()
{
#if 0
	std::vector<QsRecvUsbData> dataList;
	if (GetUsbMsg(dataList) && m_arIOHookerList.size())
	{
		std::vector<QcHookIOMsgCb>::iterator hookerIter = m_arIOHookerList.begin();
		while(hookerIter != m_arIOHookerList.end())
		{
			std::vector<QsRecvUsbData>::iterator iter(dataList.begin());
			std::vector<QsRecvUsbData>::iterator endIter(dataList.end());
			for (; iter != endIter; ++iter)
			{
				if (!(*hookerIter)(*iter))
				{
					hookerIter = m_arIOHookerList.erase(hookerIter);
					break;
				}
			}
			if (iter == endIter)
			{
				++hookerIter;
			}
		}
	}
#endif
}

void QcGameApp::ExitApp(bool bTerminateProcess)
{
	QmFlushLog();
#ifdef QmRebootOn
	EnbaleShutdownPrivilege();
	::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
	TerminateProcess(GetCurrentProcess(), 1);
#else
    if (bTerminateProcess)
    {
        DebugBreak();
        TerminateProcess(GetCurrentProcess(), 1);
    }	
#endif
}