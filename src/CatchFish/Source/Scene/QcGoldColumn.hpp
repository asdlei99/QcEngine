#pragma once

#include "Scene\QcSceneDigital.hpp"
class QcDigitalNode;
class QcGoldColumn : public QcSceneNode
{
	typedef QcSceneNode QcBaseClass;
	DeclareRuntimeClass(QcGoldColumn);
public:
	QcGoldColumn();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime, bool ItemAnimationPlay);
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
    virtual void OnTimer(u32 nTimer, void* pContext);
	bool IsFinish() const {return m_bFinish;}
	void ShowColumn(u32 iScore, const QcPoint& startPos, bool IsWinMode);
	void FinishAnimator(u32 iParam);    //回调函数 用于判断动画是否完成
protected:
	bool m_bFinish;
	//QcSceneNodePtr m_pRollingCoin;
	QcDigitalNode m_oDigitalNode;
	QcSceneAnimationPtr m_pItemAnimation;
	QcSceneNodePtr m_pBagNode;

    QcSceneNodePtrList      m_arrChips;
    QcSceneNodePtr          m_pChipAnimation;
    u32                  m_iNumOfChips;
};


#define ADD_CHIP_TIMER          10001