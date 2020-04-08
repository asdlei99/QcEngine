#pragma once

#include "Scene\QcSceneNode.hpp"
#include "Res\QcResDigitalDesc.hpp"

#define QmMaxDigital 128
#define QmSetDigitalType(digitalNode, digtialType)\
	do\
	{\
		static const QcResDigitalDesc* pTempDigitalDesc = dynamic_cast<const QcResDigitalDesc*>(QmResMgr->GetImageDesc(QcObjType(#digtialType)));\
			if (pTempDigitalDesc)\
			{\
			(digitalNode).SetDigitalType(pTempDigitalDesc);\
			}\
			else\
			{\
				QmAssert(false);\
			}\
	}while(0,0)

class QcDigitalNode : public QcSceneNode
{
	DeclareRuntimeClass(QcDigitalNode);
public:
	enum QeRadix
	{
		eRadix2 = 2,
		eRadix8 = 8,
		eRadix10 = 10,
		eRadix16 = 16,
	};

	QcDigitalNode();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void OnTimer(u32 nTimer, void* pContext);
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix);
	
	void UpdateSize();
	void SetAlign(QeAlign eAlign){ m_eAlign = eAlign; m_bPosDirty = true;}
	void SetRadix(QeRadix eRadix){ m_eRadix = eRadix; m_bPosDirty = true;}
	void SetValue(s64 value, u32 nFillChars = 0){ m_iValue = value; m_nFillChars = nFillChars; m_bPosDirty = true; m_bBuf = false;}
	void SetValue(BYTE* pBuf, u32 nSize);
	void SetFlashBit(s32 iBit);
	void SetFlashTime(f32 fShowTime, f32 fHideTime);
	void SetDigitalType(const QcObjType& objType);
	void SetDigitalType(const QcResDigitalDesc* pDesc);
	void SetUnitType(const QcObjType& objType);
	void SetGapWidth(f32 fGapWidth) { m_ffGapWidth = fGapWidth; m_bPosDirty = true;}
protected:
	QcPointList& GetDispPosList();
protected:
	s32 m_iFlashBit;
	f32 m_fTimeShow;
	f32 m_fTimeHide;
	bool m_bShowNode;

	QeAlign m_eAlign;
	QeRadix m_eRadix;
	union
	{
		s64 m_iValue;
		BYTE m_sBuf[QmMaxDigital];
	};
	f32 m_ffGapWidth;
	u32 m_nFillChars;
	bool m_bBuf;
	bool m_bPosDirty;
	QcPointList m_arPosList;
	QcTexturePtrList m_arTextureList;
	QcSceneNodePtr m_pUnitNode;
};