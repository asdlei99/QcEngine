#pragma once

#ifndef QmIOUsbOn
#ifdef QmHardAlg
#define QmAlgorithmFile "Temp1"
#define QmSysSettingFile "Temp0"
#define QmPlayerInfoFile "Temp2"
#else
#define QmAlgorithmFile "__Temp1"
#define QmSysSettingFile "__Temp0"
#define QmPlayerInfoFile "__Temp2"
#endif
#else
#define QmAlgorithmFile "_Temp1"
#define QmSysSettingFile "_Temp0"
#define QmPlayerInfoFile "_Temp2"
#endif

#ifndef QmIOUsbOn
inline bool InitCom(){return true;}
inline BYTE GetEnKey(BYTE Iflag,DWORD* Opdata)
{
	Opdata[0] = 0x9a94bea5;
	Opdata[1] = 0x45ed0278;
	Opdata[2] = 0x12345678;
	Opdata[3] = 0x23456789;
	return 0;
}
inline void GetKeyMap(__int32 keymap[eKeyTotal])
{
	memset(keymap, 0, sizeof(__int32) * eKeyTotal);
}
inline void ShutDownIOThread(void)
{}
inline void SetCardErrorCode(int iErrorCode){}
inline BYTE ModifyClassGroupNum(WORD Ixianhao,DWORD Ijitaihao){return 0;}
inline BYTE GetClassGroupNum(WORD* pOxianhao,DWORD* pOjitaihao)
{
	*pOxianhao = 100; *pOjitaihao = 1000; return 0;
}
#endif

#ifdef QmReadLockerOn
#define QmReadLock(cs) QmLocker(cs)
#else
#define QmReadLock(cs)
#endif

#define QmGlobalData QsGlobalData::gGlobalData
#define QmSence QmGlobalData.goApp->GetScene()
#define QmSceneMgr QmGlobalData.gpSceneMgr
#define QmResMgr QmGlobalData.gpResMgr
#define QmSettingMgr QmResMgr->GetSettingMgr()
#define QmSoundMgr QmGlobalData.gpSoundMgr
#define QmAnimatorCreator QmGlobalData.gpAnimateCreator
#define QmFishAlg QmGlobalData.gpAlgorithm
#define QmSysSetting QmGlobalData.gpSysSetting
#define QmResLocker() QmLocker(QmResMgr->GetLocker())
#define QcMediaFileFinderInstance QmGlobalData.gpMediaFinder
#define QmTimerMgr GetInstance<QcTimerMgr>()
#define QmDebugHelper QmGlobalData.gsDebugHelper

#define QmMaxPlayer (QmPlayerNum)

#ifdef QmTestVersion
#define QmGlobalConstVar __declspec(selectany)
#define QmRegisterConstVar(name, min, max, gap) QmDebugHelper.AddModifyItem(name, min, max, gap, #name)
#define QmRegisterConstVar2(type, name, intiValue, min, max, gap)\
	QmGlobalConstVar type name = intiValue;\
	const s32 QmUniqueVarName = QmRegisterConstVar(name, min, max, gap)
#else
#define QmGlobalConstVar const
#define QmRegisterConstVar2(type, name, intiValue, min, max, gap) const type name = intiValue
#define QmRegisterConstVar(type, name, intiValue, min, max, gap) ((void)0)
#endif

//ProfilerLoading
#ifdef QmTestVersion
#define QmProfilerLoading(Type) QcDebugProfilerHelper QmUniqueVarName(Type, #Type)
#else
#define QmProfilerLoading(Type)
#endif


