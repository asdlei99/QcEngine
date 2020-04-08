#pragma once

#ifdef QmTestVersion
#include "QcDebugHelper.hpp"
#endif

class QcGameApp;
class QcScene;
class QcResManager;
class QcSceneMgr;
class QcSystemSetting;
class QcMediaFileFinder;
class QcResPackLoader;
class QcSoundMgr;
class QcFishAlgorithm;
class QcAnimatorCreator;
struct QsGlobalData
{
public:
#ifdef QmTestVersion
	QcDebugHelper gsDebugHelper;
#endif

	QcSharedPtr<QcResPackLoader> m_pFilePacker;
	QcString gsFileStoragePath;
	QcGameApp* goApp;
	QcResManager* gpResMgr;
	QcSceneMgr* gpSceneMgr;
	QcSystemSetting* gpSysSetting;
	QcMediaFileFinder* gpMediaFinder;
	QcSoundMgr* gpSoundMgr;
	QcFishAlgorithm* gpAlgorithm;
	QcAnimatorCreator* gpAnimateCreator;

	QcRect gScreenRect;	//投影大小
	QcSize gWindowSize;	//分辨率
	//-1：D3DPOOL_MANAGED，0 D3DPOOL_MANAGED
	s32 giForceTexInVideoMem;

	void SetEncryptKey(void *pKey, unsigned long keyLen)
	{
		gpCrypt->SetKey((const char*)pKey, keyLen);
	}
	void EncryptData(char *pData, unsigned long dataLen, unsigned long gapLen = 16)
	{
		gpCrypt->Encrypt(pData, dataLen);
	}
	void DecryptData(char *pData, unsigned long dataLen, unsigned long gapLen = 16)
	{
		gpCrypt->Decrypt(pData, dataLen);
	}
	static QsGlobalData gGlobalData;
private:
	QsGlobalData();
	~QsGlobalData();
	QmDisableCopyAssignment(QsGlobalData);
	
	QiCrypt* gpCrypt;
};

inline QcPoint& QfPointToScreen(QcPoint& pos)
{
	static f32 gfXScale = QmGlobalData.gWindowSize.Width/QmGlobalData.gScreenRect.getWidth();
	static f32 gfYScale = QmGlobalData.gWindowSize.Height/QmGlobalData.gScreenRect.getHeight();

	pos.X *= gfXScale;
	pos.Y *= gfYScale;

	pos.X += QmGlobalData.gWindowSize.Width * 0.5f;
	pos.Y = QmGlobalData.gWindowSize.Height * 0.5f - pos.Y;

	return pos;
}

#ifdef QmTestVersion
class QcDebugProfilerHelper
{
	QcDebugProfilerHelper(const QcDebugProfilerHelper&);
	void operator=(const QcDebugProfilerHelper&);
public:
	explicit QcDebugProfilerHelper(QeProfilerLoadingType eType, const char* psName) : m_eType(eType)
	{
		QmDebugHelper.CalLoadingTime(m_eType, psName ,true);
	}
	~QcDebugProfilerHelper()
	{
		QmDebugHelper.CalLoadingTime(m_eType, NULL, false);
	}
protected:
	QeProfilerLoadingType m_eType;
};
#endif

