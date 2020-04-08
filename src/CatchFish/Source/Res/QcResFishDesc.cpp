#include "pch.hpp"
#include "QcResFishDesc.hpp"
#include "QcResImageDesc.hpp"
#include "QcTexture.hpp"
#include "Collision\QcRegionMulti.hpp"
#include "Collision\QcRegionCircle.hpp"
#include "Collision\QcRegionPolygon.hpp"
#include "Collision\QcRegionMultiCircle.hpp"
#include "Scene\Fish\QcFish.hpp"
#include "Animator\FishAnimator\QcDefaultFishAnimator.hpp"

u32 FindPlayingFlag(const QcObjType& objType)
{
	u32 eFlag = 0;
	if (objType == QmObjName(fish01))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else if (objType == QmObjName(fish02))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else if (objType == QmObjName(fish03))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else if (objType == QmObjName(fish04))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else if (objType == QmObjName(fish05))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else if (objType == QmObjName(fish06))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else if (objType == QmObjName(fish07))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else if (objType == QmObjName(fish08))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else if (objType == QmObjName(fish10))
		eFlag = QcFish::eHitPlay | QcFish::eSpeedUp;
	else
		eFlag = QcFish::eRandomPlay | QcFish::eSpeedUp;

	return eFlag;
}

ImplRuntimeClass2(QcResFishDesc, 50);

QcResFishDesc::QcResFishDesc()
{
	m_pRuntimeClass = QmRuntimeClass(QcFish);
	m_bIsBottomPath = false;
}

void QcResFishDesc::LoadStateDesc(const QcXmlNodeIterator& node)
{
	QcResImageDesc::LoadStateDesc(node);

	if (GetStateParameter(ePlaying) == NULL)
		AddState(ePlaying, 1.2f, false);
	if (GetStateParameter(eLivingToDown) == NULL)
		AddState(eLivingToDown, 1.f, false);
	if (GetStateParameter(eDowning) == NULL)
		AddState(eDowning, 1.f, true);
	if (GetStateParameter(eDownToLiving) == NULL)
		AddState(eDownToLiving, 1.f, false);
	if (GetStateParameter(eLivingToUp) == NULL)
		AddState(eLivingToUp, 1.f, false);
	if (GetStateParameter(eUping) == NULL)
		AddState(eUping, 1.f, true);
	if (GetStateParameter(eUpToLiving) == NULL)
		AddState(eUpToLiving, 1.f, false);

	if (QcMath::equals(m_fDyingTime, 0.f))
	{
		const QsStateParameter* pDyingState = GetStateParameter(eDying);
		if (pDyingState)
			m_fDyingTime = pDyingState->m_fPlayTime;
	}
}

bool QcResFishDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	QcResImageDesc::LoadDesc(node);
	return true;
}

bool QcResFishDesc::LoadAttibute(const QcXmlNodeIterator& node)
{
	QcResImageDesc::LoadAttibute(node);

	m_fDyingTime = node.Attribute("dyingtime", 0.0f);
	m_nScore = node.Attribute("score", 10);
	m_fScale = node.Attribute("scale",1.0f);
	m_fSpeed = node.Attribute("speed", 80.f + QcMath::max_(0.f, 30.f - m_nScore * 2.0f));		//(80.f, 110.f)
	m_sBombEffect = node.Attribute("BombEffect");
	m_oSound = node.Attribute("Sound");
	m_ID = QmArrayFind(QaFishList, m_objType);
	m_iPlayingFlag = FindPlayingFlag(m_objType);

	m_pAnimator = QmClassFactoryInstance->Find(node.Attribute("Animator"));
	if (m_pAnimator == NULL)
		m_pAnimator =  QmRuntimeClass(QcDefaultFishAnimator);

	return true;
}

void QcResFishDesc::PostLoad(QcResManager* pResMgr)
{
	QcResImageDesc::PostLoad(pResMgr);
	if (!m_pSourceRegion)
	{
		const QsStateParameter* pState = GetStateParameter(eLiving);
		if (pState)
		{
			f32 fHalfWidth = pState->m_pTextureList[0]->GetWidth() * 0.5f;
			f32 fHalffHeight = pState->m_pTextureList[0]->GetHeight() * 0.5f;
#if 0
			if (fHalfWidth/fHalffHeight > 1.5f || fHalffHeight/fHalfWidth > 1.5f)
			{	
				QcPoint pointList[] = 
				{
					QcPoint(-fHalfWidth, -fHalffHeight), 
					QcPoint(fHalfWidth, -fHalffHeight),
					QcPoint(fHalfWidth, fHalffHeight),
					QcPoint(-fHalfWidth, fHalffHeight),
					QcPoint(-fHalfWidth, -fHalffHeight), 
				};
				m_pSourceRegion.Set(new QcRegionPolygon(pointList, sizeof(pointList)/sizeof(QcPoint)));
			}
			else
#endif
			{
				m_pSourceRegion = new QcRegionCircle(QcPoint(0.f, 0.f), (fHalfWidth + fHalffHeight) * 0.5f);
			}
		}
	}
}