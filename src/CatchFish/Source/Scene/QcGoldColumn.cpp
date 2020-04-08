#include "pch.hpp"
#include "QcGoldColumn.hpp"
#include "Animator\QcAnimatorAlpha.hpp"
#include "Res\QcTexture.hpp"
#include "Animator\QiAnimatorDeque.hpp"

ImplRuntimeClass2(QcGoldColumn, 10 * 20);

#define MAX_CHIPS               28

QcGoldColumn::QcGoldColumn()
: m_bFinish(false)
{

}

void QcGoldColumn::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcBaseClass::Init(desc, bLoadSubNode);
	m_pItemAnimation = NULL;
	SetCurrentTexture(NULL);
	QmSetDigitalType(m_oDigitalNode, DigitalCoin);
	//m_oDigitalNode.SetDigitalType(QmObjType(DigitalCoin));
}

void QcGoldColumn::Animate(double dCurrentTime,f32 fElapsedTime, bool ItemAnimationPlay)
{
	QcBaseClass::Animate(dCurrentTime, fElapsedTime);
	//if (m_pItemAnimation &&ItemAnimationPlay)
	//{
	//	m_pItemAnimation->Animate(dCurrentTime,fElapsedTime);
	//}
	m_oDigitalNode.Animate(dCurrentTime,fElapsedTime);
	//m_pBagNode->Animate(dCurrentTime,fElapsedTime);

    if (m_pChipAnimation && ItemAnimationPlay)
        m_pChipAnimation->Animate(dCurrentTime, fElapsedTime);

    m_arrChips.Animate(dCurrentTime, fElapsedTime);
}

void QcGoldColumn::ShowColumn(u32 iScore, const QcPoint& startPos, bool IsWinMode)
{
    m_arrChips.clear();

	if (iScore < 100)
    {
        m_iNumOfChips = 5;
    }
    else if (iScore < 1000)
    {
        m_iNumOfChips = 5 + iScore/100;
    }
    else if (iScore < 10000)
    {
        m_iNumOfChips = 14 + iScore / 1000;
    }
    else
    {
        m_iNumOfChips = 23 + iScore / 10000;
    }

    if (m_iNumOfChips > MAX_CHIPS)
        m_iNumOfChips = MAX_CHIPS;

    QcSceneNodePtr pFirstChip(QmGlobalData.gpResMgr->MakeSceneNodeInstance<QcSceneNode>(m_pDesc));
    pFirstChip->SetPosition(QcPoint(0, m_pDesc->GetHeight()*.5f));
    m_arrChips.push_back(pFirstChip);

    if (!m_pChipAnimation)
        m_pChipAnimation = QmGlobalData.gpResMgr->MakeGameInstance(QmObjName(ChipAnimation));
    m_pChipAnimation->SetVisible(true);
    m_pChipAnimation->SetPosition(pFirstChip->GetPosition() + QcPoint(0,(pFirstChip->GetHeight()+m_pChipAnimation->GetHeight())*.5f));

	//QcResImageDesc* pItemDesc = NULL;
	//if (iScore < 100)
	//{	
	//	static QcResImageDesc* gsDesc = QmResMgr->GetImageDesc(QmObjType(SilveryCoin));
	//	pItemDesc = gsDesc;	
	//}	
	//else if (iScore < 1000)
	//{
	//	static QcResImageDesc* gsDesc = QmResMgr->GetImageDesc(QmObjType(GoldCoin));
	//	pItemDesc = gsDesc;	
	//}
	//else if (iScore < 10000)
	//{
	//	static QcResImageDesc* gsDesc = QmResMgr->GetImageDesc(QmObjType(SilveryGem));
	//	pItemDesc = gsDesc;
	//}
	//else
	//{	
	//	static QcResImageDesc* gsDesc = QmResMgr->GetImageDesc(QmObjType(RedGem));
	//	pItemDesc = gsDesc;
	//}
	//QcResManager* pResMgr = QmResMgr;
	//if (!IsWinMode)
	//{
	//	m_pItemAnimation = pResMgr->MakeSceneNodeInstance<QcSceneAnimation>(pItemDesc);
	//	m_pItemAnimation->SetScale(0.6f);
	//	m_pItemAnimation->SetPosition(QcPoint(0.f, 100.f));
	//	QcAnimatorMove* ItemMoveUpAnimation = static_cast<QcAnimatorMove*>(QmAnimatorCreator->MakeLineAnimator2(m_pItemAnimation->GetPosition(), QcPoint(0,180), 500.f));
	//	QcAnimatorMove* ItemMoveDownAnimation = static_cast<QcAnimatorMove*>(QmAnimatorCreator->MakeLineAnimator2(m_pItemAnimation->GetPosition(), QcPoint(0,-30.f), 300.f));
	//	QiAnimatorDeque* pItemAnimatorList = QmAnimatorCreator->MakeAnimatorDeque();
	//	QcGravitySpeed* UpAspeed = new QcGravitySpeed(-500, 1.0f);
	//	QcGravitySpeed* DownAspeed = new QcGravitySpeed(300, 1.0f);
	//	ItemMoveUpAnimation->AddSpeedAnimator(UpAspeed);
	//	ItemMoveDownAnimation->AddSpeedAnimator(DownAspeed);
	//	pItemAnimatorList->AddAnimator(ItemMoveUpAnimation);
	//	pItemAnimatorList->AddAnimator(ItemMoveDownAnimation);
	//	
	//	pItemAnimatorList->AddFinishCallBack(QcFinishAnimatorCb(boost::bind(&QcGoldColumn::FinishAnimator, this, _1)));
	//	m_pItemAnimation->AddAnimator(pItemAnimatorList);
	//	m_pItemAnimation->SetFinishFlag(pItemAnimatorList);

		m_oDigitalNode.SetValue(iScore);
		m_oDigitalNode.SetVisible(false);
	//	m_oDigitalNode.SetPosition(QcPoint(0.f,70.f));
	//}

	//static QcResImageDesc* gspBagNodeDesc = QmResMgr->GetImageDesc(QmObjType(GoldColumnBag));
	//m_pBagNode = gspBagNodeDesc->MakeInstance();
	//QcPoint bag_pos(0.f, 0.5f * gspBagNodeDesc->GetHeight());
	//m_pBagNode->SetPosition(bag_pos);
}

void QcGoldColumn::RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	QcBaseClass::RenderSub(render, pWorldMatrix);
	//if (m_pItemAnimation)
	//{
	//	m_pItemAnimation->Render(render, pWorldMatrix);
	//}
	//m_pBagNode->Render(render, pWorldMatrix);

    m_arrChips.Render(render, pWorldMatrix);

    if (m_pChipAnimation)
        m_pChipAnimation->Render(render, pWorldMatrix);

	m_oDigitalNode.Render(render, pWorldMatrix);
}

void QcGoldColumn::FinishAnimator(u32 iParam)
{
	m_bFinish = true;
	m_oDigitalNode.SetVisible(true);
	m_pBagNode->SetState(eDying);
	m_pItemAnimation->SetVisible(false);
}

void QcGoldColumn::OnTimer( u32 nTimer, void* pContext )
{
    if (nTimer == ADD_CHIP_TIMER)
    {
        if (m_arrChips.size() < m_iNumOfChips)
        {
            QcPoint pos = m_arrChips.back()->GetPosition() + QcPoint(0, m_pDesc->GetHeight());
            m_pChipAnimation->SetPosition(pos + QcPoint(0, (m_pChipAnimation->GetHeight() + m_arrChips.back()->GetHeight())*.5f));

            QcSceneNodePtr pNewChip(QmGlobalData.gpResMgr->MakeSceneNodeInstance<QcSceneNode>(m_pDesc));
            pNewChip->SetPosition(pos);

            m_arrChips.push_back(pNewChip);
        }
        else
        {
            KillTimer(nTimer);
            m_pChipAnimation->SetVisible(false);
            m_oDigitalNode.SetPosition(m_pChipAnimation->GetPosition());
            m_oDigitalNode.SetVisible(true);
            m_bFinish = true;
        }
    }
}
