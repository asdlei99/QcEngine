#include "pch.hpp"
#include "QcQuickAccountShowNode.hpp"
#include "QcPlayerMgr.hpp"
#include "Res\QcResDescParam.hpp"
#include "FileStorageSystem\QcPlayerInforList.hpp"
#include "FileStorageSystem\QcPlayerInfo.hpp"

ImplRuntimeClass(QcQuickAccountShowNode);

QcQuickAccountShowNode::QcQuickAccountShowNode()
{

}

void QcQuickAccountShowNode::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcSceneNode::Init(desc, bLoadSubNode);
	m_pDeepSlipNodeSlide = QmResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(QuickAccountWin));
	m_vStartPos.X = 0.5f * (m_pDeepSlipNodeSlide->GetWidth() - GetWidth());
	m_vStartPos.Y = (GetHeight() + m_pDeepSlipNodeSlide->GetHeight()) * 0.5f;
	m_pDeepSlipNodeSlide->SetPosition(m_vStartPos);
	m_pMoveAnimator  = NULL;
	m_iDeepSlipTimes = 1;
	m_bShowLast = false;
	m_pDeepSlipNode = QmResMgr->MakeSceneNodeInstance<QcSceneNode>(QmObjType(QuickDeepSlip));

	struct QsItemInfo{
		QcObjType m_objType;
		QcDigitalNode* m_pDigital;
		QcObjType	m_objUnitType;
	};

	QsItemInfo ItemInfo[] = {
		{QmObjName(TotalProfit) , &m_oTotalProfilt , QmObjName(QuickAccountCoinUnit)},
		{QmObjName(CurrentProfit) , &m_oCurrentProfilt , QmObjName(QuickAccountCoinUnit)},
		{QmObjName(InScore) , &m_oInScore , QmObjName(QuickAccountCoinUnit)},
		{QmObjName(ExitScore) , &m_oOutScore , QmObjName(QuickAccountCoinUnit)},
		{QmObjName(InCoin) , &m_oInCoin , QmObjName(QuickAccountCoinUnit)},
		{QmObjName(OutCoin) , &m_oOutCoin , QmObjName(QuickAccountCoinUnit)},
		{QmObjName(Lottery) , &m_oOutLottery , QmObjName(QuickAccountCoinUnit)},
		{QmObjName(LeaveTime) , &m_oLeaveTime , QmObjName(QuickAccountTimeUnit)},
#ifdef PRINTINFO
		{QcObjType("") , &m_oInScoreEn , QcObjType("")},
		{QcObjType("") , &m_oOutScoreEn , QcObjType("")},
		{QcObjType("") , &m_oCcFlagEn , QcObjType("")},
		{QcObjType("") , &m_oIdx1En , QcObjType("")},
		{QcObjType(""), &m_oIdx2En , QcObjType("")},
		{QcObjType("") , &m_oZcEn , QcObjType("")},
		{QcObjType("") , &m_oCurCEn , QcObjType("")},
#endif
	};

	f32 fHalfWidth = GetWidth() * 0.5f;
	f32 fHalfHeight = GetHeight() * 0.5f;
	QcPoint pt(fHalfWidth - 30.f, fHalfHeight - 22.f);
	f32 fGap = 37.f;

	for (int i=0; i<QmCountOf(ItemInfo); ++i)
	{
		QsItemNode nodeItem;
		if (ItemInfo[i].m_objType.size())
		{
			nodeItem.m_pPreNode = QmResMgr->MakeSceneNodeInstance<QcSceneNode>(ItemInfo[i].m_objType);
			nodeItem.m_pPreNode->SetPosition(QcPoint(((nodeItem.m_pPreNode->GetWidth() - GetWidth()) * 0.5f) , pt.Y));
		}
		nodeItem.m_pDigitalNode = ItemInfo[i].m_pDigital;
		QmSetDigitalType(*(nodeItem.m_pDigitalNode), DialogDigital);
		nodeItem.m_pDigitalNode->SetAlign(eAlignRightCenter);
		nodeItem.m_pDigitalNode->SetPosition(QcPoint(pt.X - 30.f,pt.Y));
		if (ItemInfo[i].m_objUnitType.size())
		{
			nodeItem.m_pExtNode = QmResMgr->MakeSceneNodeInstance<QcSceneNode>(ItemInfo[i].m_objUnitType);
			nodeItem.m_pExtNode->SetPosition(pt);
		}
		m_arItemList.push_back(nodeItem);
		pt.Y -= fGap;
	}
	SetCurrentTexture(NULL);
}

void QcQuickAccountShowNode::RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	m_pDeepSlipNodeSlide->Render(render , pWorldMatrix);
	for (int i = 1; i < m_iDeepSlipTimes; ++i)
	{
		m_pDeepSlipNode->SetPosition(QcPoint(m_vStartPos.X , m_vStartPos.Y - m_pDeepSlipNodeSlide->GetHeight() * (i)));
		m_pDeepSlipNode->Render(render , pWorldMatrix);
	}
	int iQuanBuZongYing = 0;
	int iLastProfit = 0;
	int iBenCiLiRun = 0;
	int iShangFen = 0;
	int iXiaFen = 0;
	int iToubi = 0;
	int iTuiBi = 0;
	int iCaiPiao = 0;
	int iLeaveTime = QcMath::ceil32(QmSysSetting->GetLeaveTime()/60.f);

	QcPlayerInforList* pInforList = QmSceneMgr->GetPlayerMgr()->GetPlayerInfoList();
	for (int i=0; i<QmMaxPlayer; ++i)
	{
		iLastProfit += pInforList->GetPlayerInfo(i)->GetLiRunLast();
		iShangFen += pInforList->GetPlayerInfo(i)->GetInScore();
		iXiaFen += pInforList->GetPlayerInfo(i)->GetOutScore();
		iToubi += pInforList->GetPlayerInfo(i)->GetInCoin();
		iTuiBi += pInforList->GetPlayerInfo(i)->GetOutCoin();
		iCaiPiao += pInforList->GetPlayerInfo(i)->GetOutLottery();
	}
	iQuanBuZongYing = iShangFen + iToubi - (iXiaFen + iTuiBi + iCaiPiao);
	iBenCiLiRun = iQuanBuZongYing - iLastProfit;

	m_oTotalProfilt.SetValue(iQuanBuZongYing);
	m_oCurrentProfilt.SetValue(iBenCiLiRun);
	m_oInScore.SetValue(iShangFen);
	m_oOutScore.SetValue(iXiaFen);
	m_oInCoin.SetValue(iToubi);
	m_oOutCoin.SetValue(iTuiBi);
	m_oOutLottery.SetValue(iCaiPiao);
	m_oLeaveTime.SetValue(iLeaveTime);
#ifdef PRINTINFO
	QsPrintInfo pRintInfo={0};
	QmFishAlg->GetPlayerTestInfo(&pRintInfo);
	m_oInScoreEn.SetValue(pRintInfo.iInScoreEn);
	m_oOutScoreEn.SetValue(pRintInfo.iOutScoreEn);
	m_oCcFlagEn.SetValue(pRintInfo.iCcFlagEn);
	m_oIdx1En.SetValue(pRintInfo.iIdx1En);
	m_oIdx2En.SetValue(pRintInfo.iIdx2En);
	m_oZcEn.SetValue(pRintInfo.iZcEn);
	m_oCurCEn.SetValue(pRintInfo.iCurCEn);
#endif

	int nCount = m_bShowLast ? m_arItemList.size() : (m_iDeepSlipTimes - 1);
	for (int i = 0; i < nCount; ++i)
	{
		QsItemNode& node = m_arItemList[i];
		node.m_pDigitalNode->Render(render , pWorldMatrix);
		if (m_bShowWord)
		{
			if (node.m_pPreNode)
				node.m_pPreNode->Render(render , pWorldMatrix);
			if (node.m_pExtNode)
				node.m_pExtNode->Render(render , pWorldMatrix);
		}
	}
}

void QcQuickAccountShowNode::Animate( double dCurrentTime,f32 fElapsedTime )
{
	m_pDeepSlipNodeSlide->Animate(dCurrentTime,fElapsedTime);
	if (m_bVisible)
	{
		if (m_iDeepSlipTimes < (s32)m_arItemList.size())
		{
			if (NULL == m_pMoveAnimator)
			{
				m_pMoveAnimator =QmAnimatorCreator->MakeLineAnimator2(m_pDeepSlipNodeSlide->GetPosition(),
					QcPoint(m_vStartPos.X , m_vStartPos.Y - m_pDeepSlipNodeSlide->GetHeight() * m_iDeepSlipTimes),400);
				m_pDeepSlipNodeSlide->AddAnimator(m_pMoveAnimator);
			}
			else if (m_pMoveAnimator != NULL)
			{
				if (m_pMoveAnimator->IsFinished())
				{
					++m_iDeepSlipTimes;
					m_pMoveAnimator = QmAnimatorCreator->MakeLineAnimator2(m_pDeepSlipNodeSlide->GetPosition(),
						QcPoint(m_vStartPos.X , m_vStartPos.Y - m_pDeepSlipNodeSlide->GetHeight() * m_iDeepSlipTimes),400);	
					m_pDeepSlipNodeSlide->AddAnimator(m_pMoveAnimator);
				}
			}
		}
		else
		{
			if (m_pMoveAnimator->IsFinished())
				m_bShowLast = true;
		}
		
	}
	else
	{
		m_pDeepSlipNodeSlide->RemoveAnimator(NULL);
		m_pMoveAnimator  = NULL;
		m_iDeepSlipTimes = 1;
		m_pDeepSlipNodeSlide->SetPosition(m_vStartPos);
		m_bShowLast = false;
	}
}
