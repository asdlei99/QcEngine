#include "pch.hpp"
#include "QcAppConfig.hpp"

struct QsDevMode
{
	QsDevMode(u32 nWidth = 0, u32 nHeight = 0) : m_nWidth(nWidth), m_nHeigth(nHeight) {}

	bool operator == (const QsDevMode& left) const
	{
		return m_nWidth == left.m_nWidth && m_nHeigth == left.m_nHeigth;
	}

	bool operator < (const QsDevMode& left) const
	{
		if (m_nWidth < left.m_nWidth)
			return true;
		else if (m_nWidth == left.m_nWidth)
			return m_nHeigth < left.m_nHeigth;
		return false;
	}

	u32 m_nWidth;
	u32 m_nHeigth;
};

QcAppConfig::QcAppConfig() :
m_bFullScreen(false)
{
}

void QcAppConfig::Parse(const QcXmlNodeIterator& rootNode, void* pParam)
{
	QcXmlNodeIterator topNode = rootNode.FirstNode();
	if (!topNode)
		return;

	QmGlobalData.gWindowSize.Width = topNode.SubValue("SceneWidth", QmGlobalData.gScreenRect.getWidth());
	QmGlobalData.gWindowSize.Height = topNode.SubValue("SceneHeight", QmGlobalData.gScreenRect.getHeight());
	m_bFullScreen = !IsDebuggerPresent() && topNode.SubValue("FullScreen", 1.0f) > 0.f;
#ifdef QmTestVersion
	IsHaveEDID();
#endif
	if (!m_bFullScreen)
	{
		QmGlobalData.gWindowSize.Width = QcMath::min_(QmGlobalData.gWindowSize.Width, 1280.f);
		QmGlobalData.gWindowSize.Height = QcMath::min_(QmGlobalData.gWindowSize.Height, 600.f);
	}
	else
	{
		SearchBestConfig(QmGlobalData.gWindowSize.Width, QmGlobalData.gWindowSize.Height);
	}

	if (QmSysSetting->IsUionScreenMode())
	{
		QmGlobalData.gScreenRect._BottomLeftPt.X *= 2;
		QmGlobalData.gScreenRect._TopRightPt.X *= 2;
	}
	//·ÖÆÁ´¦Àí
	else if (QmSysSetting->IsTwoScreenMode())
	{
		QmGlobalData.gWindowSize.Width /= 2;
	}
	m_bImmediatelPresent = topNode.SubValue("ImmediatelPresent", 1.0f) > 0.f;
	QmSysSetting->AdjustTrackedBox(QmGlobalData.gScreenRect);
}

bool QcAppConfig::IsHaveEDID () const
{
	LONG rRet = 0;
	HKEY hDisplayKey = 0;
	if ( (rRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Enum\\DISPLAY", 0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &hDisplayKey)) == ERROR_SUCCESS)
	{
		int index = 0;
		char sName[256] = {0};
		DWORD nLen = QmCountOf(sName);
		while ((rRet = RegEnumKeyExA(hDisplayKey, index, sName, &nLen, 0, NULL, NULL, NULL)) == ERROR_SUCCESS)
		{
			nLen = QmCountOf(sName);
			++index;

			HKEY monitor = 0;
			if ( (rRet = RegOpenKeyExA(hDisplayKey, sName, 0, KEY_READ | KEY_ENUMERATE_SUB_KEYS, &monitor)) != ERROR_SUCCESS)
			{
				QmLog("0: fail to open key %s,%d", sName, rRet);
				QmLogSysError();
				continue;
			}

			char sModeName[256] = {0};
			DWORD sModeLen = QmCountOf(sModeName);
			int iModeIndex = 0;
			while ( (rRet = RegEnumKeyExA(monitor, iModeIndex, sModeName, &sModeLen, 0, NULL, NULL, NULL)) == ERROR_SUCCESS)
			{
				sModeLen = QmCountOf(sModeName);
				++iModeIndex;

				HKEY hControlMode = 0;
				char sControlName[128];
				sprintf_s(sControlName, "%s\\Control", sModeName);
				if ( (rRet = RegOpenKeyExA(monitor, sControlName, 0, KEY_READ, &hControlMode)) != ERROR_SUCCESS)
				{
					continue;
				}
				
				RegCloseKey(hControlMode);
				sprintf_s(sControlName, "%s\\Device Parameters", sModeName);
				if ( (rRet = RegOpenKeyExA(monitor, sControlName, 0, KEY_READ, &hControlMode)) != ERROR_SUCCESS)
				{
					QmLog("1: fail to open key %s, %d", sControlName, rRet);
					QmLogSysError();
					continue;
				}

				if ( (rRet = RegQueryValueExA(hControlMode, "EDID", 0, 0, NULL, NULL)) == ERROR_SUCCESS)
				{
					RegCloseKey(hControlMode);
					return true;
				}
				RegCloseKey(hControlMode);
			}
			
			if (iModeIndex == 0)
			{
				QmLog("3 : fail to RegEnumKeyExA :%d", rRet);
				QmLogSysError();
			}
		}
		if (index == 0)
		{
			QmLog("4: fail to RegEnumKeyExA:%d", rRet);
			QmLogSysError();
		}
	}
	else
	{
		QmLog("5:fail to RegOpenKeyExA:%d", rRet);
		QmLogSysError();
	}
	return false;
}

bool QcAppConfig::SearchBestConfig(f32& width, f32& height)
{
	bool bRet = false;

	std::vector<QsDevMode> arDevModeList;
	DEVMODE DevMode;
	DEVMODE currentDevMode = {0};
	DWORD idx = ENUM_CURRENT_SETTINGS;
	while (EnumDisplaySettings(NULL, idx, &DevMode))
	{
		if (idx == ENUM_CURRENT_SETTINGS)
			currentDevMode = DevMode;
		arDevModeList.push_back(QsDevMode(DevMode.dmPelsWidth, DevMode.dmPelsHeight));
		++idx;
	}
	
	QsDevMode arDevMode[] = 
	{
		QsDevMode(1920, 1080),
		QsDevMode(1920, 1080),
		QsDevMode(1600, 900),
		QsDevMode(1440, 900),
		QsDevMode(1366, 768),
		QsDevMode(1280, 768),
	};
	QeResolution currentResolution = QmSysSetting->GetResolution();
	if (currentResolution == eDefaultNone)
	{
		if (IsHaveEDID())
			arDevMode[0] = QsDevMode(u32(width), u32(height));
		else
			arDevMode[0] = arDevMode[e1366_768];
	}
	else
	{
		arDevMode[0] = arDevMode[currentResolution];
	}
	std::sort(arDevModeList.begin(), arDevModeList.end(), std::less<QsDevMode>());

	if (QmSysSetting->IsUionScreenMode() || QmSysSetting->IsTwoScreenMode())
	{
		if (currentDevMode.dmPelsWidth < DWORD(2000))
		{
			QmSysSetting->SetPlayerNumber(eMachine_10_1);
			QmLog("Could not to open two screen device.");
		}
		else
		{
			for (idx=0; idx<QmCountOf(arDevMode); ++idx)
			{
				arDevMode[idx].m_nWidth *= 2;
				if (std::binary_search(arDevModeList.begin(), arDevModeList.end(), arDevMode[idx]))
				{
					width = (f32)arDevMode[idx].m_nWidth;
					height = (f32)arDevMode[idx].m_nHeigth;

					bRet = true;
					break;
				}
				arDevMode[idx].m_nWidth /= 2;
			}
			if (!bRet)
			{
				width = (f32)currentDevMode.dmPelsWidth;
				height = (f32)currentDevMode.dmPelsHeight;

				bRet = true;
				QmLog("Could not find two screen resolution ratio. Now use [%f,%f]", currentDevMode.dmPelsWidth, currentDevMode.dmPelsHeight);
			}
		}
	}
	if (!bRet)
	{
		for (idx=0; idx<QmCountOf(arDevMode); ++idx)
		{
			if (std::binary_search(arDevModeList.begin(), arDevModeList.end(), arDevMode[idx]))
			{
				width = (f32)arDevMode[idx].m_nWidth;
				height = (f32)arDevMode[idx].m_nHeigth;

				bRet = true;
				break;
			}
		}
		if (idx != 0)
		{
			if (currentResolution != eDefaultNone)
				QmSysSetting->SetResolution(QeResolution(idx));
			QmLog("The screen ratio [%d,%d] is not supported, Now use [%f,%f]", arDevMode[0].m_nWidth, arDevMode[0].m_nHeigth, width, height);
		}
		if (idx == QmCountOf(arDevMode))
		{
			QmSysSetting->SetResolution(eDefaultNone);

			QcList<QsDevMode>::iterator iter(arDevModeList.begin());
			QcList<QsDevMode>::iterator endIter(arDevModeList.end());
			for (;iter != endIter; ++iter)
			{
				if (iter->m_nWidth > 1440)
					continue;
				else
				{
					f32 fPMBi = iter->m_nWidth/(float)iter->m_nHeigth;
					if (fPMBi >= (16/10.0f - 0.001) && fPMBi <= (16/9.0f + 0.001))
					{
						width = (f32)iter->m_nWidth;
						height = (f32)iter->m_nHeigth;
						bRet = true;
						break;
					}
				}
			}
		}
	}
	return bRet;
}