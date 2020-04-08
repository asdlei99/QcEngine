#include "pch.hpp"
#include "QcBullet.hpp"
#include "QcSceneAnimation.hpp"
#include "QcSceneAnimationMgr.hpp"
#include "QcBulletMgr.hpp"
#include "Fish\QcFish.hpp"
#include "QcPlayer.hpp"
#include "Res\QcResBulletDesc.hpp"
#include "Res\QcResGunDesc.hpp"
#include "Animator\QcAnimatorMoveBoundBox.hpp"
#include "Animator\QiAnimatorDeque.hpp"
#include "Animator\QcAnimatorTexture.hpp"

ImplRuntimeClass2(QcBullet, 300);

QcBullet::QcBullet()
: m_fExpodeRadiu(0.f)
, m_iLevel(0)
, m_pPlayer(NULL)
, m_pTargetFish(NULL)
, m_bSuperBullet(false)
,m_bIsBugBullet(false)
{

}
QcBullet::~QcBullet()
{

}

void QcBullet::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	BaseClass::Init(desc, bLoadSubNode);
	//QcSceneNode* pBulletFireAnimation = QmResMgr->MakeSceneNodeInstance<QcSceneNode>(pImageDesc);
	//pBulletFireAnimation->SetPosition(QcPoint((GetWidth() + pBulletFireAnimation->GetWidth()) * -0.3f , 0));
	//AddSubSceneNode(pBulletFireAnimation);
}

void QcBullet::Animate(double dCurrentTime,f32 fElapsedTime)
{
	if (m_pTargetFish)
	{
		m_pBoundBox->SetRotation(QcVector(GetPosition(), m_pTargetFish->GetPosition()));
	}
	BaseClass::Animate(dCurrentTime, fElapsedTime);
}

void QcBullet::LoadBulletInfo(const QcResGunDesc* pGunDesc, u32 iScore, QcPlayer* pPlayer)
{
	m_pTargetFish = pPlayer->GetLockFish();
	if (m_pTargetFish)
		m_pTargetFish->AddMonitor(this);
	m_pPlayer = pPlayer;

	u32 arrang = pGunDesc->GetLevelMax() - pGunDesc->GetLevelMin();
	f32 fMin;
	f32 fMax;
	pGunDesc->GetExpodeRadiu(fMin, fMax);
	m_fExpodeRadiu = ((fMax - fMin) * (iScore - pGunDesc->GetLevelMin())/arrang) + fMin;
	m_fExpodeRadiu = QcMath::clamp(m_fExpodeRadiu, fMin, fMax);
	QmAssert(m_fExpodeRadiu < 1000.f);
	m_bSuperBullet = pGunDesc->GetSuperGun();
	m_iLevel = iScore;
	
	m_oDigitalNode.SetRotation(PI + HALF_PI);
	SetDigitalValue(m_iLevel);
	QmSetDigitalType(GetDigitalNode(), BulletDigital);
	//SetDigitalType(QmObjType(BulletDigital));
	SetDigitalVisible(true);
	m_pBoundBox = (QcAnimatorMoveBoundBox*)QmAnimatorCreator->MakeBoundBoxAnimator(QmGlobalData.gScreenRect, pGunDesc->GetSpeed() * (m_pTargetFish ? 1.35f : 1.0f));
	AddAnimator(m_pBoundBox);


    static QcResImageDesc* pImageDesc = QmResMgr->GetImageDesc(QmObjType(BulletFire));
    f32 fOffsetStep = GetHeight()/m_arSubList.size();
    if (m_bSuperBullet) 
		fOffsetStep *= 0.65f;
    f32 fOffsetY = m_arSubList.size()%2 == 0 ? (m_arSubList.size()/2-0.5f)*fOffsetStep : (m_arSubList.size()/2)*fOffsetStep;

	const QcResBulletDesc* pBulletDesc = static_cast<const QcResBulletDesc*>(GetDesc());

	QcPointList SubFirePosList = pBulletDesc->GetSubFirePosList();
	assert(SubFirePosList.size() == m_arSubList.size());
    for (u32 i=0; i<m_arSubList.size(); ++i)
    {
		
        m_arSubList[i]->SetPosition(SubFirePosList.at(i));
        fOffsetY -= fOffsetStep;
    }
}

void QcBullet::Explode()
{
	RemoveMonitor();
#if 0
	QcResManager* pResMgr = QmResMgr;
	const QcResBulletDesc* pBulletDesc = static_cast<const QcResBulletDesc*>(GetDesc());
	const QcResImageDesc* pNetDesc = static_cast<const QcResBulletDesc*>(pResMgr->GetImageDesc(pBulletDesc->GetNetKind()));
	UInt32 nNet = pBulletDesc->GetNetNum();

 	QcSceneAnimation* pAnimation = QfMakeEmptyInstance<QcSceneNet>();
	QiAnimatorDeque* pAnimatorDeque = QmAnimatorCreator->MakeAnimatorDeque();

	f32 fRoration = TWO_PI / nNet;

	f32 fScaleSub = 1.0f;
	QcArray<QcPoint> posList(nNet);
	switch (nNet)
	{
	case 1:
		{
			posList[0] = QcPoint(0.f, 0.f);
			break;
		}
	case 2:
		{
			posList[0].X = m_fExpodeRadiu * 0.5f;
			posList[0].Y = 0.f;
			posList[1].X = -m_fExpodeRadiu * 0.5f;
			posList[1].Y = 0.f;
			fScaleSub = 0.5f;
			break;
		}
	case 3:
	case 4:
		{
			f32 fTemp = (f32)1.0/sin(fRoration * 0.5f);
			//内切圆半径
			f32 fSubRadiu = (f32)m_fExpodeRadiu/(1.0f + fTemp);
			if (nNet & 2)
			{
				posList[0].X = 0.f;
				posList[0].Y = (m_fExpodeRadiu - fSubRadiu);
			}
			else
			{
				posList[0].X = fSubRadiu;
				posList[0].Y = (m_fExpodeRadiu - fSubRadiu) * cos(fRoration * 0.5f);
			}
			f32 fSinTemp = sin(fRoration);
			f32 fCosTemp = cos(fRoration);
			for (UInt32 i=1; i<nNet; ++i)
			{	
				posList[i] = posList[i-1];
				posList[i].rotateBy(fCosTemp, fSinTemp);
			}
			fScaleSub = fSubRadiu/m_fExpodeRadiu;
			break;
		}
	}
	//50.f 为网的透明宽度
	QmAssert(pNetDesc->GetWidth() > 100.f);
	fScaleSub *= m_fExpodeRadiu/( (pNetDesc->GetWidth() * 0.5f - /*50.f*/0.f) );

 	for (UInt32 i=0; i<nNet; ++i)
 	{
 		QcSceneNode* pSceneNode = pResMgr->MakeSceneInstance<QcSceneNode>(pNetDesc);
 		pAnimation->AddSubSceneNode(pSceneNode);
		pSceneNode->SetPosition(posList[i]);
		pSceneNode->SetScale(fScaleSub);
		pSceneNode->SetColor(GetOwner()->GetColorFlag());
 	}

	f32 fInitScale = 0.2f;
	f32 fEndScale = 1.0f;
	const f32 fLivingTime = 0.6f;
 	pAnimatorDeque->AddAnimator(QmAnimatorCreator->MakeScaleAnimator(fInitScale, fEndScale, (fEndScale-fInitScale)/(fLivingTime * 0.5f) ));
	pAnimatorDeque->AddAnimator(QmAnimatorCreator->MakeDitherAnimator(fLivingTime * 0.5f, 6.f, GetPosition()));
	pAnimation->AddAnimator(pAnimatorDeque);
	pAnimation->SetLivingTime(fLivingTime);
	pAnimation->SetPosition(GetPosition());
	pAnimation->SetRotation(GetRotation() - HALF_PI);
	pAnimation->SetScale(fInitScale);
	//pAnimation->SetColor(QcColor(255, 255, 0, 255));

	QmSceneMgr->GetAnimationMgr()->AddAnimation(pAnimation, eAnimationLayerNet);
	QmSceneMgr->GetAnimationMgr()->AddParticle(eAnimationLayerNet, pBulletDesc->GetNetWave().c_str(), GetPosition(),fLivingTime, m_fExpodeRadiu/pBulletDesc->GetNetWaveRadiu());
#else 
	f32 fInitScale = 0.2f;
	f32 fEndScale = 1.0f;
	const f32 fLivingTime = 0.7f;
	QcResManager* pResMgr = QmResMgr;
	QiAnimatorDeque* pAnimatorDeque = QmAnimatorCreator->MakeAnimatorDeque();
	const QcResBulletDesc* pBulletDesc = static_cast<const QcResBulletDesc*>(GetDesc());

	//QcResImageDesc* pNetDesc;
	QcResImageDesc* pNetEffDesc = NULL;
	
	if (m_bSuperBullet)
	{
		QcResImageDesc* psDesc = pResMgr->GetImageDesc(pBulletDesc->GetNetEffB());
		pNetEffDesc = psDesc;
	}else
	{
		QcResImageDesc* psDesc = pResMgr->GetImageDesc(pBulletDesc->GetNetEffA());
		pNetEffDesc = psDesc;
	}
	
	QcSceneAnimation* pAnimation = QfMakeEmptyInstance<QcSceneAnimation>();
	pAnimation->SetRenderState(QmMakeRenderState(gNetRenderState));
    QcSceneNode* pNetPicAnimation = pResMgr->MakeSceneNodeInstance<QcSceneNode>(pNetEffDesc);
    pNetPicAnimation->SetColor(GetOwner()->GetColorFlag());	
    pNetPicAnimation->SetRotation(-HALF_PI);
    pAnimation->AddSubSceneNode(pNetPicAnimation);

    if (m_arSubList.size() > 1)
    {
        QcPoint BasePos(m_fExpodeRadiu, 0);
        f32 fRadStep = TWO_PI / m_arSubList.size();

        if (m_arSubList.size()%2 == 0)
            BasePos.rotateByRad(fRadStep*.5f);

        pNetPicAnimation->SetPosition(BasePos);

        for (u32 i=1; i<m_arSubList.size(); ++i)
        {
            pNetPicAnimation = pResMgr->MakeSceneNodeInstance<QcSceneNode>(pNetEffDesc);
            pNetPicAnimation->SetColor(GetOwner()->GetColorFlag());
            BasePos.rotateByRad(fRadStep);
            pNetPicAnimation->SetPosition(BasePos);
            pNetPicAnimation->SetRotation(-HALF_PI);
            pAnimation->AddSubSceneNode(pNetPicAnimation);
        }
    }
	pAnimation->SetLivingTime(fLivingTime);
	pAnimation->SetPosition(GetPosition());
	pAnimation->SetRotation(GetRotation());
	pAnimation->SetColor(GetOwner()->GetColorFlag());
	pAnimation->SetScale(fInitScale);
	
	pAnimatorDeque->AddAnimator(QmAnimatorCreator->MakeScaleAnimator(fInitScale, fEndScale, (fEndScale-fInitScale)/(fLivingTime * 0.5f) ));
	if (pBulletDesc->GetIsShock())
	{
		pAnimatorDeque->AddAnimator(QmAnimatorCreator->MakeDitherAnimator(fLivingTime * 0.5f, 3.f, GetPosition()));
	}
	
	pAnimation->AddAnimator(pAnimatorDeque);
	pAnimation->SetFinishFlag(pAnimatorDeque);
	
	QmSceneMgr->GetAnimationMgr()->AddAnimation(pAnimation, eAnimationLayerNet);
#endif
	ToDie();
}

void QcBullet::OnRemoveNodeMonitor(QcSceneNode* pNode)
{
	if (m_pTargetFish == pNode)
	{
		m_pTargetFish = NULL;
	}
}

void QcBullet::RemoveMonitor()
{
	if (m_pTargetFish)
	{
		QcSharedPtr<QcSceneNode> save(m_pTargetFish);
		m_pTargetFish = NULL;
		save->RemoveMonitor(this);
	}
}

void QcBullet::ToDead()
{
	BaseClass::ToDead();
	QmSceneMgr->GetBulletMgr()->Remove(this);
}