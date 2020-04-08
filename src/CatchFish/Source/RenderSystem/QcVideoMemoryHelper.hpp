#pragma once

#ifdef QmTestVersion

class QcVideoMemoryHelper : public QcObject
{
public:
	QcVideoMemoryHelper(HMONITOR hMonitor = NULL) : m_hMonitor(hMonitor){}

	bool GetVideoMemoryViaDirectDraw(DWORD* pdwAvailableVidMem);
	bool GetVideoMemoryViaDxDiag(DWORD* pdwDisplayMemory);
	bool GetVideoMemoryViaWMI(DWORD* pdwAdapterRam);
	bool GetVideoMemoryViaDXGI(DWORD* pDedicatedVideoMemory, DWORD* pDedicatedSystemMemory, DWORD* pSharedSystemMemory);
protected:
	HRESULT GetDeviceIDFromHMonitor(HMONITOR hm, WCHAR* strDeviceID, int cchDeviceID);
	HMONITOR GetMonitor()
	{
		if (m_hMonitor == NULL)
		{
			m_hMonitor = DXUTMonitorFromWindow(DXUTGetHWND(), MONITOR_DEFAULTTOPRIMARY);
		}
		return m_hMonitor;
	}
protected:
	HMONITOR m_hMonitor;
};

#endif