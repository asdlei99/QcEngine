#include "Pch.hpp"

#ifdef QmTestVersion

#include <dxdiag.h>
#include <wbemidl.h>
#include <ddraw.h>
#include "QcVideoMemoryHelper.hpp"

#pragma comment(lib,"odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "wbemuuid.lib")


typedef HRESULT ( WINAPI* LPCREATEDXGIFACTORY )( REFIID, void** );
typedef HRESULT (WINAPI* LPDIRECTDRAWCREATE )( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData );
typedef BOOL ( WINAPI* PfnCoSetProxyBlanket )( IUnknown* pProxy, DWORD dwAuthnSvc, DWORD dwAuthzSvc,
											  OLECHAR* pServerPrincName, DWORD dwAuthnLevel, DWORD dwImpLevel,
											  RPC_AUTH_IDENTITY_HANDLE pAuthInfo, DWORD dwCapabilities );

struct DDRAW_MATCH
{
	GUID guid;
	HMONITOR hMonitor;
	CHAR strDriverName[512];
	bool bFound;
};

//-----------------------------------------------------------------------------
BOOL WINAPI DDEnumCallbackEx( GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm )
{
	UNREFERENCED_PARAMETER( lpDriverDescription );

	DDRAW_MATCH* pDDMatch = ( DDRAW_MATCH* ) lpContext;
	if( pDDMatch->hMonitor == hm )
	{
		pDDMatch->bFound = true;
		strcpy_s( pDDMatch->strDriverName, 512, lpDriverName );
		memcpy( &pDDMatch->guid, lpGUID, sizeof( GUID ) );
	}
	return TRUE;
}

bool QcVideoMemoryHelper::GetVideoMemoryViaDirectDraw(DWORD* pdwAvailableVidMem)
{
	LPDIRECTDRAW pDDraw = NULL;
	LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx = NULL;
	HRESULT hr;
	bool bGotMemory = false;
	*pdwAvailableVidMem = 0;

	HMONITOR hMonitor = GetMonitor();
	HINSTANCE hInstDDraw;
	LPDIRECTDRAWCREATE pDDCreate = NULL;

	hInstDDraw = LoadLibrary( L"ddraw.dll" );
	if( hInstDDraw )
	{
		DDRAW_MATCH match;
		ZeroMemory( &match, sizeof( DDRAW_MATCH ) );
		match.hMonitor = hMonitor;

		pDirectDrawEnumerateEx = ( LPDIRECTDRAWENUMERATEEXA )GetProcAddress( hInstDDraw, "DirectDrawEnumerateExA" );

		if( pDirectDrawEnumerateEx )
		{
			hr = pDirectDrawEnumerateEx( DDEnumCallbackEx, ( VOID* )&match, DDENUM_ATTACHEDSECONDARYDEVICES );
		}

		pDDCreate = ( LPDIRECTDRAWCREATE )GetProcAddress( hInstDDraw, "DirectDrawCreate" );
		if( pDDCreate )
		{
			pDDCreate( &match.guid, &pDDraw, NULL );

			LPDIRECTDRAW7 pDDraw7;
			if( SUCCEEDED( pDDraw->QueryInterface( IID_IDirectDraw7, ( VOID** )&pDDraw7 ) ) )
			{
				DDSCAPS2 ddscaps;
				ZeroMemory( &ddscaps, sizeof( DDSCAPS2 ) );
				ddscaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
				hr = pDDraw7->GetAvailableVidMem( &ddscaps, pdwAvailableVidMem, NULL );
				if( SUCCEEDED( hr ) )
					bGotMemory = true;
				pDDraw7->Release();
			}
		}
		FreeLibrary( hInstDDraw );
	}
	return bGotMemory;
}
bool QcVideoMemoryHelper::GetVideoMemoryViaDxDiag(DWORD* pdwDisplayMemory )
{
	HMONITOR hMonitor = GetMonitor();
	WCHAR strInputDeviceID[512];
	GetDeviceIDFromHMonitor( hMonitor, strInputDeviceID, 512 );

	HRESULT hr;
	HRESULT hrCoInitialize = S_OK;
	bool bGotMemory = false;
	IDxDiagProvider* pDxDiagProvider = NULL;
	IDxDiagContainer* pDxDiagRoot = NULL;
	IDxDiagContainer* pDevices = NULL;
	IDxDiagContainer* pDevice = NULL;
	WCHAR wszChildName[256];
	WCHAR wszPropValue[256];
	DWORD dwDeviceCount;
	VARIANT var;

	*pdwDisplayMemory = 0;
	hrCoInitialize = CoInitialize( 0 );
	VariantInit( &var );

	// CoCreate a IDxDiagProvider*
	hr = CoCreateInstance( CLSID_DxDiagProvider,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IDxDiagProvider,
		( LPVOID* )&pDxDiagProvider );
	if( SUCCEEDED( hr ) ) // if FAILED(hr) then it is likely DirectX 9 is not installed
	{
		DXDIAG_INIT_PARAMS dxDiagInitParam;
		ZeroMemory( &dxDiagInitParam, sizeof( DXDIAG_INIT_PARAMS ) );
		dxDiagInitParam.dwSize = sizeof( DXDIAG_INIT_PARAMS );
		dxDiagInitParam.dwDxDiagHeaderVersion = DXDIAG_DX9_SDK_VERSION;
		dxDiagInitParam.bAllowWHQLChecks = FALSE;
		dxDiagInitParam.pReserved = NULL;
		pDxDiagProvider->Initialize( &dxDiagInitParam );

		hr = pDxDiagProvider->GetRootContainer( &pDxDiagRoot );
		if( SUCCEEDED( hr ) )
		{
			hr = pDxDiagRoot->GetChildContainer( L"DxDiag_DisplayDevices", &pDevices );
			if( SUCCEEDED( hr ) )
			{
				hr = pDevices->GetNumberOfChildContainers( &dwDeviceCount );
				if( SUCCEEDED( hr ) )
				{
					for( DWORD iDevice = 0; iDevice < dwDeviceCount; iDevice++ )
					{
						bool bFound = false;
						hr = pDevices->EnumChildContainerNames( iDevice, wszChildName, 256 );
						if( SUCCEEDED( hr ) )
						{
							hr = pDevices->GetChildContainer( wszChildName, &pDevice );
							if( SUCCEEDED( hr ) )
							{
								hr = pDevice->GetProp( L"szKeyDeviceID", &var );
								if( SUCCEEDED( hr ) )
								{
									if( var.vt == VT_BSTR )
									{
										if( wcsstr( var.bstrVal, strInputDeviceID ) != 0 )
											bFound = true;
									}
									VariantClear( &var );
								}

								if( bFound )
								{
									hr = pDevice->GetProp( L"szDisplayMemoryEnglish", &var );
									if( SUCCEEDED( hr ) )
									{
										if( var.vt == VT_BSTR )
										{
											bGotMemory = true;
											wcscpy_s( wszPropValue, 256, var.bstrVal );

											// wszPropValue should be something like "256.0 MB" so _wtoi will convert it correctly
											*pdwDisplayMemory = _wtoi( wszPropValue );

											// Convert from MB to bytes
											*pdwDisplayMemory *= 1024 * 1024;
										}
										VariantClear( &var );
									}
								}
								SAFE_RELEASE( pDevice );
							}
						}
					}
				}
				SAFE_RELEASE( pDevices );
			}
			SAFE_RELEASE( pDxDiagRoot );
		}
		SAFE_RELEASE( pDxDiagProvider );
	}

	if( SUCCEEDED( hrCoInitialize ) )
		CoUninitialize();

	return bGotMemory;
}
bool QcVideoMemoryHelper::GetVideoMemoryViaWMI(DWORD* pdwAdapterRam)
{
	HMONITOR hMonitor = GetMonitor();
	WCHAR strInputDeviceID[512];
	GetDeviceIDFromHMonitor( hMonitor, strInputDeviceID, 512 );

	HRESULT hr;
	bool bGotMemory = false;
	HRESULT hrCoInitialize = S_OK;
	IWbemLocator* pIWbemLocator = NULL;
	IWbemServices* pIWbemServices = NULL;
	BSTR pNamespace = NULL;

	*pdwAdapterRam = 0;
	hrCoInitialize = CoInitialize( 0 );

	hr = CoCreateInstance( CLSID_WbemLocator,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		( LPVOID* )&pIWbemLocator );
#ifdef PRINTF_DEBUGGING
	if( FAILED( hr ) ) wprintf( L"WMI: CoCreateInstance failed: 0x%0.8x\n", hr );
#endif

	if( SUCCEEDED( hr ) && pIWbemLocator )
	{
		// Using the locator, connect to WMI in the given namespace.
		pNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );

		hr = pIWbemLocator->ConnectServer( pNamespace, NULL, NULL, 0L,
			0L, NULL, NULL, &pIWbemServices );
#ifdef PRINTF_DEBUGGING
		if( FAILED( hr ) ) wprintf( L"WMI: pIWbemLocator->ConnectServer failed: 0x%0.8x\n", hr );
#endif
		if( SUCCEEDED( hr ) && pIWbemServices != NULL )
		{
			HINSTANCE hinstOle32 = NULL;

			hinstOle32 = LoadLibraryW( L"ole32.dll" );
			if( hinstOle32 )
			{
				PfnCoSetProxyBlanket pfnCoSetProxyBlanket = NULL;

				pfnCoSetProxyBlanket = ( PfnCoSetProxyBlanket )GetProcAddress( hinstOle32, "CoSetProxyBlanket" );
				if( pfnCoSetProxyBlanket != NULL )
				{
					// Switch security level to IMPERSONATE. 
					pfnCoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
						RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );
				}

				FreeLibrary( hinstOle32 );
			}

			IEnumWbemClassObject* pEnumVideoControllers = NULL;
			BSTR pClassName = NULL;

			pClassName = SysAllocString( L"Win32_VideoController" );

			hr = pIWbemServices->CreateInstanceEnum( pClassName, 0,
				NULL, &pEnumVideoControllers );
#ifdef PRINTF_DEBUGGING
			if( FAILED( hr ) ) wprintf( L"WMI: pIWbemServices->CreateInstanceEnum failed: 0x%0.8x\n", hr );
#endif

			if( SUCCEEDED( hr ) && pEnumVideoControllers )
			{
				IWbemClassObject* pVideoControllers[10] = {0};
				DWORD uReturned = 0;
				BSTR pPropName = NULL;

				// Get the first one in the list
				pEnumVideoControllers->Reset();
				hr = pEnumVideoControllers->Next( 5000,             // timeout in 5 seconds
					10,                  // return the first 10
					pVideoControllers,
					&uReturned );
#ifdef PRINTF_DEBUGGING
				if( FAILED( hr ) ) wprintf( L"WMI: pEnumVideoControllers->Next failed: 0x%0.8x\n", hr );
				if( uReturned == 0 ) wprintf( L"WMI: pEnumVideoControllers uReturned == 0\n" );
#endif

				VARIANT var;
				if( SUCCEEDED( hr ) )
				{
					bool bFound = false;
					for( UINT iController = 0; iController < uReturned; iController++ )
					{
						pPropName = SysAllocString( L"PNPDeviceID" );
						hr = pVideoControllers[iController]->Get( pPropName, 0L, &var, NULL, NULL );
#ifdef PRINTF_DEBUGGING
						if( FAILED( hr ) )
							wprintf( L"WMI: pVideoControllers[iController]->Get PNPDeviceID failed: 0x%0.8x\n", hr );
#endif
						if( SUCCEEDED( hr ) )
						{
							if( wcsstr( var.bstrVal, strInputDeviceID ) != 0 )
								bFound = true;
						}
						VariantClear( &var );
						if( pPropName ) SysFreeString( pPropName );

						if( bFound )
						{
							pPropName = SysAllocString( L"AdapterRAM" );
							hr = pVideoControllers[iController]->Get( pPropName, 0L, &var, NULL, NULL );
#ifdef PRINTF_DEBUGGING
							if( FAILED( hr ) )
								wprintf( L"WMI: pVideoControllers[iController]->Get AdapterRAM failed: 0x%0.8x\n",
								hr );
#endif
							if( SUCCEEDED( hr ) )
							{
								bGotMemory = true;
								*pdwAdapterRam = var.ulVal;
							}
							VariantClear( &var );
							if( pPropName ) SysFreeString( pPropName );
							break;
						}
						SAFE_RELEASE( pVideoControllers[iController] );
					}
				}
			}

			if( pClassName )
				SysFreeString( pClassName );
			SAFE_RELEASE( pEnumVideoControllers );
		}

		if( pNamespace )
			SysFreeString( pNamespace );
		SAFE_RELEASE( pIWbemServices );
	}

	SAFE_RELEASE( pIWbemLocator );

	if( SUCCEEDED( hrCoInitialize ) )
		CoUninitialize();

	return bGotMemory;
}
bool QcVideoMemoryHelper::GetVideoMemoryViaDXGI(DWORD* pDedicatedVideoMemory, DWORD* pDedicatedSystemMemory, DWORD* pSharedSystemMemory)
{
	HMONITOR hMonitor = GetMonitor();
	HRESULT hr;
	bool bGotMemory = false;
	*pDedicatedVideoMemory = 0;
	*pDedicatedSystemMemory = 0;
	*pSharedSystemMemory = 0;

	HINSTANCE hDXGI = LoadLibrary( L"dxgi.dll" );
	if( hDXGI )
	{
		LPCREATEDXGIFACTORY pCreateDXGIFactory = NULL;
		IDXGIFactory* pDXGIFactory = NULL;

		pCreateDXGIFactory = ( LPCREATEDXGIFACTORY )GetProcAddress( hDXGI, "CreateDXGIFactory" );
		pCreateDXGIFactory( __uuidof( IDXGIFactory ), ( LPVOID* )&pDXGIFactory );

		for( int index = 0; ; ++index )
		{
			bool bFoundMatchingAdapter = false;
			IDXGIAdapter* pAdapter = NULL;
			hr = pDXGIFactory->EnumAdapters( index, &pAdapter );
			if( FAILED( hr ) ) // DXGIERR_NOT_FOUND is expected when the end of the list is hit
				break;

			for( int iOutput = 0; ; ++iOutput )
			{
				IDXGIOutput* pOutput = NULL;
				hr = pAdapter->EnumOutputs( iOutput, &pOutput );
				if( FAILED( hr ) ) // DXGIERR_NOT_FOUND is expected when the end of the list is hit
					break;

				DXGI_OUTPUT_DESC outputDesc;
				ZeroMemory( &outputDesc, sizeof( DXGI_OUTPUT_DESC ) );
				if( SUCCEEDED( pOutput->GetDesc( &outputDesc ) ) )
				{
					if( hMonitor == outputDesc.Monitor )
						bFoundMatchingAdapter = true;

				}
				SAFE_RELEASE( pOutput );
			}

			if( bFoundMatchingAdapter )
			{
				DXGI_ADAPTER_DESC desc;
				ZeroMemory( &desc, sizeof( DXGI_ADAPTER_DESC ) );
				if( SUCCEEDED( pAdapter->GetDesc( &desc ) ) )
				{
					bGotMemory = true;
					*pDedicatedVideoMemory = desc.DedicatedVideoMemory;
					*pDedicatedSystemMemory = desc.DedicatedSystemMemory;
					*pSharedSystemMemory = desc.SharedSystemMemory;
				}
				break;
			}
		}

		FreeLibrary( hDXGI );
	}
	return bGotMemory;
}

HRESULT QcVideoMemoryHelper::GetDeviceIDFromHMonitor(HMONITOR hm, WCHAR* strDeviceID, int cchDeviceID )
{
	HINSTANCE hInstDDraw;
	hInstDDraw = LoadLibrary( L"ddraw.dll" );
	if( hInstDDraw )
	{
		DDRAW_MATCH match;
		ZeroMemory( &match, sizeof( DDRAW_MATCH ) );
		match.hMonitor = hm;

		LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx = NULL;
		pDirectDrawEnumerateEx = ( LPDIRECTDRAWENUMERATEEXA )GetProcAddress( hInstDDraw, "DirectDrawEnumerateExA" );

		if( pDirectDrawEnumerateEx )
			pDirectDrawEnumerateEx( DDEnumCallbackEx, ( VOID* )&match, DDENUM_ATTACHEDSECONDARYDEVICES );

		if( match.bFound )
		{
			LONG iDevice = 0;
			DISPLAY_DEVICEA dispdev;

			ZeroMemory( &dispdev, sizeof( dispdev ) );
			dispdev.cb = sizeof( dispdev );

			while( EnumDisplayDevicesA( NULL, iDevice, ( DISPLAY_DEVICEA* )&dispdev, 0 ) )
			{
				// Skip devices that are monitors that echo another display
				if( dispdev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER )
				{
					iDevice++;
					continue;
				}

				// Skip devices that aren't attached since they cause problems
				if( ( dispdev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
				{
					iDevice++;
					continue;
				}

				if(QfStrNoCaseCmp( match.strDriverName, dispdev.DeviceName ) == 0 )
				{
					MultiByteToWideChar( CP_ACP, 0, dispdev.DeviceID, -1, strDeviceID, cchDeviceID );
					return S_OK;
				}

				iDevice++;
			}
		}

		FreeLibrary( hInstDDraw );
	}
	return E_FAIL;
}

#endif