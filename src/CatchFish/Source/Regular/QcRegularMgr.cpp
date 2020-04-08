#include "pch.hpp"
#include "QcRegularMgr.hpp"
#include "QcResFishRegularDesc.hpp"
#include "QcResSceneRegularDesc.hpp"
#include "QiSceneRegularMaker.hpp"
#include "Res\QcResImageDesc.hpp"
#include "Res\QcResFishDesc.hpp"
#include "Res\QcResPathDesc.hpp"
#include "Res\QcPathMgr.hpp"
#include "Scene\Fish\QcFish.hpp"
#include "Scene\Fish\QcSameKindBombFish.hpp"
#include "Animator\QcAnimatorMovePath.hpp"
#include "Animator\FishAnimator\QcGravityFishAnimator.hpp"

QcRegularMgr::QcRegularMgr()
{
}

void QcRegularMgr::Parse(const QcXmlNodeIterator& pRootNode, void* pParam)
{
	QcXmlNodeIterator node = pRootNode.FirstNode("RegularList");
	if (node)
	{
		node = node.FirstNode();
		while (node)
		{
			QcResFishRegularDesc* pFishRegular(new QcResFishRegularDesc());
			pFishRegular->LoadDesc(node);
			m_arFishRegularList.PushBack(pFishRegular);

			node = node.NextSibling();
		}

		std::sort(m_arFishRegularList.begin(), m_arFishRegularList.end(),  QsResDescSorter());

		QcSharedPointerList<QcResFishRegularDesc>::iterator iter(m_arFishRegularList.begin());
		QcSharedPointerList<QcResFishRegularDesc>::iterator endIter(m_arFishRegularList.end());
		f32 fAver = 1.0f/m_arFishRegularList.size();
		f32 fTotal = 0.f;
		for (; iter != endIter; ++iter)
		{
			if (!(*iter)->m_bOnlyInSceneRegular)
			{
				if (QcMath::equals((*iter)->m_fRandom, 0.f))
					(*iter)->m_fRandom = fAver;
				fTotal += (*iter)->m_fRandom ;
			}
		}
		f32 fLastRandom = 0.0f;
		iter = m_arFishRegularList.begin();
		for (; iter != endIter; ++iter)
		{
			if (!(*iter)->m_bOnlyInSceneRegular)
			{
				(*iter)->m_fRandom = (*iter)->m_fRandom/fTotal + fLastRandom;
				fLastRandom = (*iter)->m_fRandom;
			}
			//随机出场景队列时，把只能在过场中使用的队列排除。
			else
				(*iter)->m_fRandom = -0.01f;
		}
	}
	node = pRootNode.FirstNode("SceneRegularList");
	if (node)
	{
		node = node.FirstNode();
		while (node)
		{
			QcSharedPtr<QcResSceneRegularDesc> pSceneRegular(new QcResSceneRegularDesc());
			if (pSceneRegular->LoadDesc(node))
				m_arSceneRegularList.PushBack(pSceneRegular);

			node = node.NextSibling();
		}
		std::sort(m_arFishRegularList.begin(), m_arFishRegularList.end(),  QsResDescSorter());
	}
}

QcResFishRegularDesc* QcRegularMgr::FindFishRegularDesc(const QcObjType& objType)
{
	QcSharedPointerList<QcResFishRegularDesc>::iterator iter=binary_searchEx(m_arFishRegularList.begin(), m_arFishRegularList.end(), objType, QsResDescSorter());
	if (iter != m_arFishRegularList.end())
		return (*iter);
	return NULL;
}
QcResSceneRegularDesc* QcRegularMgr::FindSceneRegularDesc(const QcObjType& objType)
{
	QcSharedPointerList<QcResSceneRegularDesc>::iterator iter=binary_searchEx(m_arSceneRegularList.begin(), m_arSceneRegularList.end(), objType, QsResDescSorter());
	return (*iter);
}

QiSceneRegularMaker* QcRegularMgr::BeginSceneRegular(QcList<QcFish*>& fishList, const QcObjType& objType)
{
	u32 index = 0;
	if (objType == ID_NULL)
	{
#ifdef QmTestVersion
		static u32 giLast = 0;
		giLast = QcMath::surround(giLast + 1, 0, (u16)m_arSceneRegularList.size());
#else
		index = RandomInt(0, (u16)m_arSceneRegularList.size());
#endif
		
	}
	else
	{
		QcSharedPointerList<QcResSceneRegularDesc>::iterator iter = std::binary_searchEx(m_arSceneRegularList.begin(), m_arSceneRegularList.end(), objType, QsResDescSorter());
		if (iter == m_arSceneRegularList.end())
			index = RandomInt(0,  (u16)m_arSceneRegularList.size());
		else
			index = iter - m_arSceneRegularList.begin();
	}

	QiSceneRegularMaker* pCurrentSceneRegularMaker = m_arSceneRegularList[index]->GetMakerClass()->CreateObject<QiSceneRegularMaker>();
	pCurrentSceneRegularMaker->SetObjType(m_arSceneRegularList[index]->GetObjType());
	pCurrentSceneRegularMaker->BeginSceneRegular(this, fishList, m_arSceneRegularList[index]);
	return pCurrentSceneRegularMaker;
}

void QcRegularMgr::MakeFishRegular(QcList<QcFish*>& fishList, const QcObjType& objType, f32 fSpeed, u32 nCount, f32 fGapWidth)
{
	QcResFishRegularDesc* pFishRegularDesc = NULL;
	if (objType == ID_NULL)
	{
		pFishRegularDesc = FindNormalFishRegular(Randomf());
	}
	else
	{
		pFishRegularDesc = FindFishRegularDesc(objType);
	}
	if (pFishRegularDesc == NULL)
	{
		QmLog("No Fish Regular");
		return;
	}

	u32 nSceneId = QmSceneMgr->GetSceneId();
	bool bScenePath = pFishRegularDesc->GetSceneWalkablePath(nSceneId).size() && Randomf() < 0.2;
	QcResPathDesc* pPathDesc = NULL;
	if (bScenePath)
	{
		pPathDesc = QcPathMgr::FindUsablePathDesc(pFishRegularDesc->GetSceneWalkablePath(nSceneId));
	}
	else
	{
		pPathDesc = QcPathMgr::FindUsablePathDesc(pFishRegularDesc->GetNormalWalkablePath());
	}
	if (pPathDesc == NULL)
	{
		if (pFishRegularDesc->m_pAnimator == QmRuntimeClass(QcGravityFishAnimator))
		{
			MakeFishRegular(fishList, pFishRegularDesc, NULL, fSpeed, nCount, fGapWidth);
		}
		else
		{
			MakeFishRegular(fishList, pFishRegularDesc, QcResPathDesc::MakeRegularPath(pFishRegularDesc->m_fWidth), fSpeed, nCount, fGapWidth);
		}
	}
	else
		MakeFishRegular(fishList, pFishRegularDesc, pPathDesc, fSpeed, nCount, fGapWidth);
}

void QcRegularMgr::MakeFishRegular(QcList<QcFish*>& fishList, const QcResFishRegularDesc* pFishRegular, const QcResPathDesc* pPathDesc, f32 fSpeed, u32 nCount, f32 fGapWidth)
{
	QcResManager* pResMgr = QmResMgr;
	const QsRegularItemList& itemList = pFishRegular->GetItemList();

	QsPathKeyPointList keyPointList;
	if (pPathDesc == NULL && pFishRegular->m_pAnimator == QmRuntimeClass(QcGravityFishAnimator))
	{
		pPathDesc = QcResPathDesc::MakeGravityPath(keyPointList, pFishRegular->GetWidth());
	}

	QcPoint pos;
	QcPoint vBasePos = pPathDesc->GetBasePoint();
	f32 fRotation = pPathDesc->GetStartRotation();
	f32 fCs = cos(fRotation);
	f32 fSn = sin(fRotation);
	f32 fDelayTime = 0.f;
	for (u32 i=0; i<nCount; ++i)
	{
		QcSharedPtr<QcAnimatorMove> pMoveAnimator(pFishRegular->m_pAnimator->CreateObject<QcAnimatorMove>());
		pMoveAnimator->Reset(pPathDesc, fSpeed);
		pMoveAnimator->SetStartTime(fDelayTime);

		if (keyPointList.size())
		{
			static_cast<QcGravityFishAnimator*>(pMoveAnimator.Get())->SetKeyPontList(keyPointList);
		}

		QsRegularItemListConstIter iter(itemList.begin());
		QsRegularItemListConstIter endIter(itemList.end()); 
		for (; iter != endIter; ++iter)
		{
			QcResFishDesc* pFishDesc = static_cast<QcResFishDesc*>(pResMgr->GetImageDesc(iter->m_sFishName));
			const QcPointList& pointList = iter->m_pointList;

			int iSameKindFish = -1;
			if (pFishDesc->GetScore() < 10 && Randomf() < 0.03f)
				iSameKindFish = QcSameKindBombFish::CanGenSameKindFish(pFishDesc) ? RandomInt(0, pointList.size()) : -1;

			QcPointList::const_iterator pointIter(pointList.begin());
			QcPointList::const_iterator endPointIter(pointList.end());
			for (; pointIter != endPointIter; ++pointIter)
			{
				pos = *pointIter;
				pos.rotateBy(fCs, fSn);

				pos += vBasePos;
				QcFish* pFish = NULL;
				if (iSameKindFish == 0)
					pFish = QcSameKindBombFish::MakeSameKindBombFish(pFishDesc);
				else
					pFish = pFishDesc->MakeInstance()->As<QcFish*>();
				--iSameKindFish;

				pFish->SetLayer(pFishRegular->GetLayer());
				pFish->SetPosition(pos);
				pFish->SetScale(iter->m_fScale);
				pFish->SetRotation(fRotation);
				pFish->AddAnimator(pMoveAnimator);
				fishList.push_back(pFish);

				//QmAssert(!pFish->IsIntersectScreen());
			}
		}
		fDelayTime += (fGapWidth + pFishRegular->m_fWidth)/fSpeed;
	}
}

void QcRegularMgr::MakeSpreadOutRegular(const QcObjType& mainFish, const QcObjType* pSubFish, u32 nCount, const QcPoint& startPos, const QcPoint& endPos, f32 fSpeed)
{
	
}

QcResFishRegularDesc* QcRegularMgr::FindNormalFishRegular(f32 fRandom)
{
	QcSharedPointerList<QcResFishRegularDesc>::iterator iter(m_arFishRegularList.begin());
	QcSharedPointerList<QcResFishRegularDesc>::iterator endIter(m_arFishRegularList.end());
	for (; iter != endIter; ++iter)
	{
		if ((*iter)->m_fRandom >= fRandom)
			return (*iter);
	}
	return m_arFishRegularList.front();
}