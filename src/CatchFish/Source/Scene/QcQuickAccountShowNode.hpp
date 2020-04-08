#pragma once

#include "Scene\QcSceneNode.hpp"
#include "Scene\QcDigitalNode.hpp"
class QcDigitalNode;
class QcAnimatorMove;
class QcQuickAccountShowNode : public QcSceneNode
{
	struct QsItemNode{
		QcSceneNodePtr m_pPreNode;
		QcDigitalNode* m_pDigitalNode;
		QcSceneNodePtr	m_pExtNode;
	};


	DeclareRuntimeClass(QcQuickAccountShowNode);
public:
	QcQuickAccountShowNode();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	void	  SetShowWords(bool val) 
	{ 
		m_bShowWord = val; 
	}
protected:
	QcDigitalNode m_oTotalProfilt;
	QcDigitalNode m_oCurrentProfilt;
	QcDigitalNode m_oInScore;
	QcDigitalNode m_oOutScore;
	QcDigitalNode m_oInCoin;
	QcDigitalNode m_oOutCoin;
	QcDigitalNode m_oOutLottery;
	QcDigitalNode m_oLeaveTime;
	QcDigitalNode m_oInScoreEn;
	QcDigitalNode m_oOutScoreEn;
	QcDigitalNode m_oCcFlagEn;
	QcDigitalNode m_oIdx1En;
	QcDigitalNode m_oIdx2En;
	QcDigitalNode m_oZcEn;
	QcDigitalNode m_oCurCEn;
	QcSceneNodePtr  m_pDeepSlipNodeSlide;
	QcSceneNodePtr  m_pDeepSlipNode;
	QcPoint			  m_vStartPos;
	QcSharedPtr<QcAnimatorMove> m_pMoveAnimator;
	s32				  m_iDeepSlipTimes;
	std::vector<QsItemNode> m_arItemList;
	bool					  m_bShowLast;
	bool					  m_bShowWord;
};	