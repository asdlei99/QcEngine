#pragma once
#include "QmMacro.hpp"
#include "QcBaseType.hpp"
extern s32 giExitFlag;
extern u64 giFrameID;

enum QeScreenID
{
	eScreen1 = 0,
	eScreen2 = 1,
	QmMaxScreen,
};
extern s32 giScreenHandleing;

enum QeState
{
	eStateNone = 0,
	eLiving = 0x1,
	eDying = 0x2,
	ePlaying = 0x4,
	eLivingToDown = 0x8,
	eDowning = 0x10,
	eDownToLiving = 0x20,
	eLivingToUp = 0x40,
	eUping = 0x80,
	eUpToLiving = 0x100,
	eDead = 0x200,
	eReady = 0x400,
};
struct StatePair
{
	const char* psStr;
	QeState eState;
};
const StatePair gStatePairs[] = 
{
	{"000", eLiving},
	{"Living", eLiving},
	{"001", eDying},
	{"Dying", eDying},
	{"002", ePlaying},
	{"003", eLivingToUp},
	{"004", eUping},
	{"005", eUpToLiving},
	{"006", eLivingToDown},
	{"007", eDowning},
	{"008", eDownToLiving},
    {"009", eDead},
};
inline const char* StateToString(QeState eState)
{
	for (int i=0; i<QmCountOf(gStatePairs); ++i)
	{
		if (gStatePairs[i].eState == eState)
			return gStatePairs[i].psStr;
	}
	return NULL;
}
inline QeState StringToState(const char* psStr)
{
	for (int i=0; i<QmCountOf(gStatePairs); ++i)
	{
		if (QfStrNoCaseCmp(psStr, gStatePairs[i].psStr) == 0)
			return gStatePairs[i].eState;
	}
	return eStateNone;
}

enum QeTimerType
{
	eDyingTimer = 1,
	eDelayTimer,
	eCountDownTimer,
	eSceneTimer,
	eSceneStateTimer,
	eSceneCheckErrorTimer,
	eGameRunningTime,
	eFlashTimer,
	eSuperGunTimer,
	ePlayerActiveTimer,
	eCoinErrorTimer,
	eFishGenerateTimer,
	eFishGenerate2Timer,
	eFishLockFillTimer,
	eFishFrozenTimer,
	eFishFleeTimer,
	eWidgetUpdateTimer,
	eSameKindFishWaitToExploseTimer,
	eXuanFengYuDieTimer,
	eThrowCoinTimer,
    eSceneLogoTimer,
    eSceneWaterInterTimer,
    eSceneWaterTotalTime,
	eAlgToIOTimer,
	eTimerCount,
	eTimerParticleGap,
	ePlayerStopAutoFire,
};

enum QeMatrixMask
{
	eMaskX = 0x1,
	eMaskY = 0x2,
	eMaskZ = 0x4,
	eMaskXYZ = eMaskX | eMaskY | eMaskZ,
};

enum QeMathNodeInfoMask
{
	eMaskPos = 0x1,
	eMaskRotation = 0x2,
	eMaskScale = 0x4,
	eMaskAll = 0xFFFFFFFF,
};

enum QeMachineType
{
	eMachine_10_1 = 0,
	eMachine_8_1,
	eMachine_6_1,
	eMachine_4_1,
	eMachine_4_2,
	eMachine_3_2,
	//联屏 IO
	eMachine_UionScreen_6_Stand_1IO,
	eMachine_UionScreen_8_Stand_1IO,
	eMachine_UionScreen_8_Lie_1IO,
	eMachine_UionScreen_10_Lie_1IO,
	//联屏 2IO
	eMachine_UionScreen_6_Stand_2IO,
	eMachine_UionScreen_8_Stand_2IO,
	eMachine_UionScreen_8_Lie_2IO,
	eMachine_UionScreen_10_Lie_2IO,
	eMachine_UionScreen_12_Lie_2IO,
	eMachine_UionScreen_14_Lie_2IO,
	eMachine_UionScreen_16_Lie_2IO,
	//分屏 IO
	eMachine_TwoScreen_4_Stand_1IO,
	eMachine_TwoScreen_6_Stand_1IO,
	eMachine_TwoScreen_8_Stand_1IO,
	eMachine_TwoScreen_8_Lie_1IO,
	//分屏 2IO
	eMachine_TwoScreen_6_Stand_2IO,
	eMachine_TwoScreen_8_Stand_2IO,
	eMachine_TwoScreen_8_lie_2IO,
	eMachine_TwoScreen_12_lie_2IO,
	eMachine_TwoScreen_16_lie_2IO,
	eMachine_TwoScreen_20_lie_2IO,
	eMachineEnd = 0xffffffff,
};

enum QeGameExitMode
{
	eOutCoin	= 0,
	eOutLottery		= 1,
	eOutWinCoin	= 2,
	eOutWinLottery	= 3,	
};

enum QeDaMaFlag
{
	eDaMaNone = 0,
	eDaMaNormal = 1,
	eDaMaClear = 2,
	eDaMaEnd = 0xffffffff,
};

enum QeAlign
{
	eAlignLeftTop,
	eAlignLeftCenter,
	eAlignLeftButtom,
	eAlignCenterTop,
	eAlignCenter,
	eAlignCenterBottom,
	eAlignRightTop,
	eAlignRightCenter,
	eAlignRightBottom,
};

enum QeEventType
{
	eEvtForceFillFish,
	eEvtFillFish,
	eEvtFishStateChange,
	eEvtSceneStateChange,
	eEvtShowDialog,
	eCustom = 0xffffffff,
};

enum QeLayer
{
	eLayerNone = -1,
	eLayerBegin = 0,

	eAnimationLayerBegin,
	eAnimationLayerBubble = eAnimationLayerBegin,
    eAnimationLayerNet,
    eAnimationLayerSpecial,
	eAnimationLayerDyingFish,
	eAnimationLayerScore,
	eAnimationLayerNormal,
	eAnimationLayerEffect,
	eAnimationLayerGunEffect,
	eAnimationLayerEnd,

	eLayerEnd = 0xFFFFFFFF,
};

struct QsFrameRegion
{
	int m_iBeginFrame;
	QcRegionAutoPtr m_pRegion;
};
typedef QcList<QsFrameRegion> QsFrameRegionList;

struct QsStateParameter
{
	QsStateParameter()
		: m_eState(eLiving)
		, m_nQty(0)
		, m_fPlayTime(0.f)
		, m_bCycle(false)
		, m_fWidth(0.f)
		, m_fHeight(0.f)
	{
	}
	f32 GetPeriodTime() const
	{
		if (m_nQty == 0)
			return 0.f;
		return m_fPlayTime;
	}
	QsFrameRegionList m_arFrameRegionList;
	QcTexturePtrList m_pTextureList;
	QeState m_eState;
	QcString m_sName;
	u32 m_nQty;
	f32 m_fPlayTime;
	f32 m_fWidth;
	f32 m_fHeight;
	bool m_bCycle;
};

#include "QcUtilFun.hpp"
#include "Common\QcList.hpp"
#include "Common\QcTimerHandle.hpp"
#include "Common\QcTimerMgr.hpp"
#include "Common\QcEventHandle.hpp"
#include "Common\QcObjectEx.hpp"
#include "FileStorageSystem\QcPlayerInfo.hpp"
#include "FileStorageSystem\QcFileStorageBase.hpp"
#include "FileStorageSystem\QcSystemSetting.hpp"
#include "FileStorageSystem\QcFishAlgorithm.hpp"
#include "RenderSystem\QcDeviceRender.hpp"
#include "RenderSystem\QiRenderState.hpp"
#include "RenderSystem\QcRenderStateHelper.hpp"
#include "QcGlobalData.hpp"
#include "App\QcGameApp.hpp"
#include "Scene\QcScene.hpp"
#include "Scene\QcSceneMgr.hpp"
#include "Scene\QcSceneAnimation.hpp"
#include "Scene\QcSceneNode.hpp"
#include "Res\QcResManager.hpp"
#include "Res\QcResImageDesc.hpp"
#include "Sound\QcSoundMgr.hpp"
#include "Animator\QcAnimatorCreator.hpp"
#include "Common\QcUtil.hpp"
#include "Common\QcEncryptFileBuf.hpp"
#include "Common\QcEncryptXmlLoader.hpp"
#include "Setting\QcSetting.hpp"


