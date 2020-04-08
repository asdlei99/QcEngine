#include "pch.hpp"
#include "QcFishControlInfo.hpp"
#include "QcFish.hpp"
#include "QcSameKindBombFish.hpp"
#include "QcFishMgr.hpp"
#include "Res\QcResManager.hpp"
#include "Res\QcPathMgr.hpp"
#include "Res\QcResPathDesc.hpp"
#include "Res\QcResFishDesc.hpp"
#include "Animator\FishAnimator\QcShelterHandle.hpp"
#include "Animator\FishAnimator\QcNormalFishAnimateHandle.hpp"
#include "Animator\FishAnimator\QcGravityFishAnimator.hpp"
#include "Scene\QcBackGroundMgr.hpp"
#include "Regular\QcRegularMgr.hpp"
#include "DcChainFish.hpp"


bool QcFishControlInfo::QsFishInfor::FishGenerate(QcList<QcFish*>& fishList)
{
	if (m_iAdded > 0)
	{
		QcResManager* pRes = QmResMgr;
		QcPathMgr* pPathMgr = pRes->GetPathMgr();

		for (int i=0; i<(int)m_iAdded; ++i)
		{
			u32 fishFlag = QcFish::eCalController;
			QcFish* pFish = m_pFishDesc->MakeInstance()->As<QcFish*>();
			QcAnimatorMove* pMoveAnimator = m_pFishDesc->GetAnimator()->CreateObject<QcAnimatorMove>();

			QcAnimatorSpeed* pSpeedAnimator = NULL;
			const QcResPathDesc* pPathDesc = NULL;
			//钻洞控制
			if (m_eScenePathType != -1 && Randomf() < QmBottomRandom)
			{
				pPathDesc = pPathMgr->GetUsableScenePathDesc(QmSceneMgr->GetSceneId(), QePathType(m_eScenePathType));
				QmAssert(pPathDesc);
				fishFlag |= QcFish::eBottom;
				const std::vector<f32>& boundInfo = pPathDesc->GetProperty("BoundInfo");
				pSpeedAnimator = new QcShelterHandle(boundInfo);
			}
			else
			{
				//pPathDesc = pPathMgr->GetUsablePathDesc((QePathType)m_ePathType);
				pPathDesc = QcResPathDesc::MakeSplinePath(m_pFishDesc->GetMaxLen());
				QmAssert(pPathDesc);
				pSpeedAnimator = new QcNormalFishAnimateHandle(m_pFishDesc->GetSpeed() * 0.5f, m_pFishDesc->GetSpeed() * 3.0f, 2.f, 4.f, m_pFishDesc->GetPlayingFlag());
			}
			pMoveAnimator->Reset(pPathDesc, m_pFishDesc->GetSpeed());
			if (pSpeedAnimator != NULL)
				pMoveAnimator->AddSpeedAnimator(pSpeedAnimator);
#ifdef QmTestVersion
			pFish->setPathObject(pPathDesc->GetObjType());
#endif
			pFish->SetPosition(pPathDesc->GetBasePoint());
			pFish->SetRotation(pPathDesc->GetStartRotation());
			pFish->SetFishFlag(fishFlag);
			pFish->AddAnimator(pMoveAnimator);

			fishList.push_back(pFish);
		}

		//Make Same Kind Fish
		if (m_iCurrentNum >= 3
			&& m_pFishDesc->GetScore() < 10
			&& Randomf() < 0.03f
			&& QcSameKindBombFish::CanGenSameKindFish(m_pFishDesc))
		{
			QcFish* pFish = QcSameKindBombFish::MakeSameKindBombFish(m_pFishDesc);

			QcAnimatorMove* pMoveAnimator = m_pFishDesc->GetAnimator()->CreateObject<QcAnimatorMove>();
			const QcResPathDesc* pPathDesc = pPathMgr->GetUsablePathDesc((QePathType)m_ePathType);
			pMoveAnimator->Reset(pPathDesc, m_pFishDesc->GetSpeed());
			QcAnimatorSpeed* pSpeedAnimator = new QcNormalFishAnimateHandle(m_pFishDesc->GetSpeed() * 0.6f, m_pFishDesc->GetSpeed() * 2.4f, 2.f, 3.f, m_pFishDesc->GetPlayingFlag());
			pMoveAnimator->AddSpeedAnimator(pSpeedAnimator);

#ifdef QmTestVersion
			pFish->setPathObject(pPathDesc->GetObjType());
#endif
			pFish->SetPosition(pPathDesc->GetBasePoint());
			pFish->SetRotation(pPathDesc->GetStartRotation());
			pFish->AddAnimator(pMoveAnimator);

			fishList.push_back(pFish);
		}
		m_iAdded = 0;

        return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


QcFishControlInfo::QcFishControlInfo()
: m_bLockFishFill(false)
, m_bFillMinFish(false)
, m_pLargeKind(NULL)
{
}

QcFishControlInfo::~QcFishControlInfo()
{
	QcList<QsFishKind*>::iterator iter(m_arKindList.begin());
	for (; iter != m_arKindList.end(); ++iter)
	{
		delete *iter;
	}
	m_arKindList.clear();
}

void QcFishControlInfo::Init(QcFishMgr* pFishMgr, const char* psConfig)
{
	m_pFishMgr = pFishMgr;

	QcEncryptXmlLoader loader;
	loader.LoadFromFile(psConfig, 0, QmBindParseFn(&QcFishControlInfo::Parse, this));
	Reset(true);
}

void QcFishControlInfo::Reset(bool bFishControlEnable)
{
	QcList<QsFishKind*>::iterator iter(m_arKindList.begin());
	for (; iter != m_arKindList.end(); ++iter)
	{
		(*iter)->Reset();
	}
	m_bForceFishFilled = true;
	LockFishFill(false);
 	if (m_pSameKindFishInfo)
 	{
 		if (bFishControlEnable)
 			m_oSameKindFishHelper.Reset(m_pSameKindFishInfo->m_nMin, m_pSameKindFishInfo->m_nMax, m_pSameKindFishInfo->m_MinTime, m_pSameKindFishInfo->m_MaxTime, m_pSameKindFishInfo->m_iMinDuringTime, m_pSameKindFishInfo->m_iMaxDuringTime, m_pSameKindFishInfo->m_fRandom);
 		else
 			m_oSameKindFishHelper.Reset(1, 3, 0, 0x7fffffff, 0, 0, 0.1f);
 	}
 	else
 		m_oSameKindFishHelper.Reset(0, 0, 0, 0, 0, 0, 1.0f);
}

void QcFishControlInfo::Parse(const QcXmlNodeIterator& rootNode, void* pParam)
{
	if (!rootNode)
		return;

	QcXmlNodeIterator topNode = rootNode.FirstNode();
	LoadFishKind(topNode, NULL);
	m_fFishFillTime = topNode.Attribute("FishFillTime", 3.f);
	CalculateRandom(NULL);
}

void QcFishControlInfo::LoadFishKind(const QcXmlNodeIterator& kindFishNode, QsFishKind* pParent)
{
	QcXmlNodeIterator firstNode = kindFishNode.FirstNode();
	while (firstNode)
	{
		if (QfStrNoCaseCmp(firstNode.name(), "FishKind") == 0)
		{
			QsFishKind* pFishKind = new QsFishKind();
			pFishKind->m_iCurrentNum = 0;
			pFishKind->m_nMin = firstNode.Attribute("min", 1);
			pFishKind->m_nMax = firstNode.Attribute("max", 1);
			QcPoint duringTime(0,0);
			duringTime = firstNode.Attribute("DuringTime", duringTime);
			pFishKind->m_iMinDuringTime = u32(duringTime.X * 1000);
			pFishKind->m_iMaxDuringTime = u32(duringTime.Y * 1000);
			pFishKind->m_ePathType = firstNode.Attribute("PathType", 1);
			pFishKind->m_eScenePathType = firstNode.Attribute("ScenePathType", -1);
			pFishKind->m_fRandom = firstNode.Attribute("random", 1.f);
			pFishKind->m_MinTime = firstNode.Attribute("MinTime", 0);
			pFishKind->m_MaxTime = firstNode.Attribute("MaxTime", 0x7fffffff);
			pFishKind->m_iTimer = pFishKind->m_MaxTime;
			pFishKind->m_iCurrentNum = 0;
			pFishKind->m_pParentKind = pParent;
			pFishKind->m_pControlInfo = this;

			if (QcNoCaseCompare(firstNode.Attribute("name")) == "MostLarge")
				m_pLargeKind = pFishKind;
			LoadFishKind(firstNode, pFishKind);
			if (pFishKind->m_objList.size() == 0)
			{
				delete pFishKind;
			}
			else if (pParent != NULL)
			{
				pParent->m_objList.PushBack(pFishKind);
			}
			else
			{
				m_arKindList.PushBack(pFishKind);
			}
		}
		else
		{
			LoadFishInfo(firstNode, pParent);
		}
		firstNode = firstNode.NextSibling();
	}
}

void QcFishControlInfo::LoadFishInfo(const QcXmlNodeIterator& fishInfoNode, QsFishKind* pParent)
{
	QcObjType objType(fishInfoNode.Attribute("name"));
	QcResFishDesc* pDesc = static_cast<QcResFishDesc*>(QmResMgr->GetImageDesc(objType));
	if (pDesc)
	{
		QsFishInfor* pFishInfor = new QsFishInfor();
		pFishInfor->m_pFishDesc = pDesc;
		pFishInfor->m_nMin = fishInfoNode.Attribute("min", 0);
		pFishInfor->m_nMax = fishInfoNode.Attribute("max", 1);
		QcPoint duringTime(0,0);
		duringTime = fishInfoNode.Attribute("DuringTime", duringTime);
		pFishInfor->m_iMinDuringTime = u32(duringTime.X * 1000);
		pFishInfor->m_iMaxDuringTime = u32(duringTime.Y * 1000);
		pFishInfor->m_fRandom = fishInfoNode.Attribute("random", 1.f);
		pFishInfor->m_ePathType = fishInfoNode.Attribute("PathType", pParent->m_ePathType);
		pFishInfor->m_eScenePathType = fishInfoNode.Attribute("ScenePathType", pParent->m_eScenePathType);
		pFishInfor->m_MinTime = fishInfoNode.Attribute("MinTime", 0);
		pFishInfor->m_MaxTime = fishInfoNode.Attribute("MaxTime", 0x7fffffff);
		pFishInfor->m_iTimer = pFishInfor->m_MaxTime;
		pFishInfor->m_iCurrentNum = 0;
		pFishInfor->m_pParentKind = pParent;
		pFishInfor->m_pControlInfo = this;
		if (pDesc->GetObjType() != QmObjName(SameKindFish))
		{
			pParent->m_objList.push_back(pFishInfor);
			m_arFishInfoList[objType] = pFishInfor;
		}
		else
		{
			m_pSameKindFishInfo = pFishInfor;
		}
	}
}

void QcFishControlInfo::CalculateRandom(QsFishKind* pFishKind)
{
#if 0
	if (pFishKind == NULL)
	{
		//计算总的概率
		f32 fTotal = 0.f;
		f32 aver = 1.0f/m_arKindList.size();
		QcList<QsFishKind*>::iterator iter(m_arKindList.begin());
		for (; iter != m_arKindList.end(); ++iter)
		{
			CalculateRandom((*iter));
 			if (QcMath::equals((*iter)->m_fRandom, 0.f))
 				(*iter)->m_fRandom = aver;
 
 			fTotal += (*iter)->m_fRandom;
		}
		//计算各个概率段
 		iter = m_arKindList.begin();
 		for (; iter != m_arKindList.end(); ++iter)
 		{
 			(*iter)->m_fRandom = (*iter)->m_fRandom/fTotal;
 		}
	}
	else
	{
		int nSize = pFishKind->m_objList.size();
		if (nSize > 0)
		{
			//计算总的概率
			f32 aver = 1.0f/nSize;
			f32 fTotal = 0.0f;
			QcList<QsFishInfor*>& arFishInfo = pFishKind->m_objList;
			QcList<QsFishInfor*>::iterator iter(arFishInfo.begin());
			for (; iter != arFishInfo.end(); ++iter)
			{
				if ((*iter)->IsKind())
				{
					CalculateRandom(pFishKind);
				}
				if (QcMath::equals((*iter)->m_fRandom, 0.f))
					(*iter)->m_fRandom = aver;

				fTotal += (*iter)->m_fRandom;
			}

			//计算各个概率段
			f32 fLastRandom = 0.0f;
			iter = arFishInfo.begin();
			for (; iter != arFishInfo.end(); ++iter)
			{
				(*iter)->m_fRandom = (*iter)->m_fRandom/fTotal + fLastRandom;
				fLastRandom = (*iter)->m_fRandom;
			}
		}
	}
#endif
}


QcFishControlInfo::QsFishInfor* QcFishControlInfo::FindFishKind(const QcObjType& objType)
{
	QcFishInforMap::iterator iter(m_arFishInfoList.find(objType));
	if (iter != m_arFishInfoList.end())
		return iter->second;
	return NULL;
}

void QcFishControlInfo::OnDead(const QcFish* pFish)
{
	QsFishInfor* pFishInfo = FindFishKind(pFish->GetObjType());
	if (pFishInfo)
	{
		if (pFishInfo->RemoveFish(1) && !m_bForceFishFilled)
		{
			PostEvent(eEvtForceFillFish);
			m_bForceFishFilled = true;
		}
		if (GetTopKind(pFishInfo) == m_pLargeKind)
		{
			LockFishFill(false);
		}
	}
}

void QcFishControlInfo::LockFishFill(bool bLock) 
{ 
	if (m_bLockFishFill != bLock)
	{
		m_bLockFishFill = bLock;  
		if (bLock)
		{
			m_pFishMgr->SetTimer(eFishLockFillTimer, 18.f, true);	
			m_pFishMgr->KillTimer(eFishGenerateTimer);
		}
		else
		{
			m_pFishMgr->KillTimer(eFishLockFillTimer);
			m_pFishMgr->SetTimer(eFishGenerateTimer, m_fFishFillTime);
			PostEvent(eEvtForceFillFish, 0, true);
			PostEvent(eEvtFillFish, 0, true);
		}
	}
}

void QcFishControlInfo::OnEvent(const QsEvent& evt)
{
	QcSceneMgr::QeSceneState eSceneState = QmSceneMgr->GetSceneState();
	if (!(eSceneState == QcSceneMgr::eSceneNormal 
		|| eSceneState == QcSceneMgr::eSceneEnd
		|| eSceneState == QcSceneMgr::eSceneSetttingReady
		) )
	{
		return;
	}
	switch(evt.m_iEvt)
	{
	case eEvtForceFillFish:
		{
			if (!m_pFishMgr->IsFrozen() && m_bForceFishFilled && !m_bLockFishFill)
 			{
    			QcList<QcFish*> fishList;
     			fishList.reserve(32);
   
   				m_bFillMinFish = true;
     			CalculateFillMin(NULL);
   				m_bFillMinFish = false;
     
     			FishGenerate(fishList);
     			m_pFishMgr->AddFish(fishList);
 
 				m_bForceFishFilled = false;
 			}
			break;
		}
	case eEvtFillFish:
		{
			if (m_pFishMgr->GetFishCount() < (m_bLockFishFill ? 40 : 60))
			{
				QcList<QcFish*> fishList;
				fishList.reserve(16);

				//普通鱼规律鱼
				FillFish(10);
				FishGenerate(fishList);

				f32 fStartTime = 0.f;
				QcList<QcFish*>::iterator iter(fishList.begin()); 
				QcList<QcFish*>::iterator endIter(fishList.end());
				for (; iter != endIter; ++iter)
				{
					(*iter)->SetStartTime(fStartTime);
					fStartTime += 0.2f;
				}

				while ( (m_pFishMgr->GetFishCount() + fishList.size()) < (m_bLockFishFill ? 40U : 60U) )
					FillExtraFish(fishList);

				m_pFishMgr->AddFish(fishList);
			}
			m_pFishMgr->SetTimer(eFishGenerateTimer, m_fFishFillTime);
			break;
		}
	}
}

u32 QcFishControlInfo::FillFish(u32 nMaxGenerate)
{
	u32 iGenerate = nMaxGenerate;
	u32 nNum = 0;
	if (m_pLargeKind && m_pLargeKind->CanFillFish())
	{
		nNum = CalculateFillSecond(m_pLargeKind, iGenerate);
		if (nNum)
		{
			LockFishFill(true);
		}
	}
	if (nNum == 0)
	{
		QcList<QsFishKind*>::iterator iter(m_arKindList.begin());
		QcList<QsFishKind*>::iterator endIter(m_arKindList.end());
		for (; iter != endIter; ++iter)
		{
			if (m_bLockFishFill || m_pLargeKind == (*iter))
				continue;

			if ((*iter)->CanFillFish())
			{
				nNum = CalculateFillSecond((*iter), iGenerate);
				iGenerate -= nNum;
			}
		}
	}

	return nNum;
}

void QcFishControlInfo::FillExtraFish(QcList<QcFish*>& fishList)
{
	f32 fRandom = Randomf();

	if (fRandom < 0.6f)
	{
		MakeQueueFish(fishList);
	}
	else if (fRandom < 0.9f)
	{
		if (Randomf() < 0.5f)
		{
			MakeGroupFish(fishList);
		}
		else
		{
			MakeGroup2Fish(fishList);
		}
	}
	else
	{
		MakeCirclePattern(fishList);
	}
}

u32 QcFishControlInfo::CalculateFillMin(QsFishKind* pFishKind)
{
	u32 iAdded = 0;
	if (pFishKind == NULL)
	{
		QcList<QsFishKind*>::iterator iter(m_arKindList.begin());
		QcList<QsFishKind*>::iterator endIter(m_arKindList.end());
		for (; iter != endIter; ++iter)
		{
			CalculateFillMin((*iter));
			if ((*iter)->IsLessWarning())
			{
				CalculateFillSecond(static_cast<QsFishKind*>(*iter), (*iter)->m_nMin - (*iter)->m_iCurrentNum);
			}
			iAdded += (*iter)->m_iAdded;
		}
	}
	else
	{
		QcList<QsFishInfor*>& arFishInfo = pFishKind->m_objList;
		QcList<QsFishInfor*>::iterator iter(arFishInfo.begin());
		QcList<QsFishInfor*>::iterator endIter(arFishInfo.end());
		for (; iter != endIter; ++iter)
		{
			if ((*iter)->IsKind())
			{
				CalculateFillMin(static_cast<QsFishKind*>(*iter));
				if ((*iter)->IsLessWarning())
				{
					CalculateFillSecond(static_cast<QsFishKind*>(*iter), (*iter)->m_nMin - (*iter)->m_iCurrentNum);
				}
			}
			else
			{
				//少于最小值
				if ((*iter)->IsLessWarning())
				{
					(*iter)->AddFish((*iter)->m_nMin - (*iter)->m_iCurrentNum);
				}
			}
		}
		iAdded = pFishKind->m_iAdded;
	}
	return iAdded;
}

u32 QcFishControlInfo::CalculateFillSecond(QsFishKind* pFishKind, u32 nNum)
{
	u32 iNum = 0;
	if (pFishKind->m_iCurrentNum < pFishKind->m_nMax)
		iNum = QcMath::min_(nNum, pFishKind->m_nMax - pFishKind->m_iCurrentNum);

	nNum = 0;
	while(iNum > 0 && pFishKind->CanFillFish(m_bFillMinFish))
	{
		int iTime = 0;
		int index = 0;
		QsFishInfor* pFishInfo = pFishKind->FindFishInfo(index);
		if (pFishInfo == NULL)
			break;

		int iAddNum = 0;
		do 
		{
			iTime++;
			if (pFishInfo->IsKind())
			{
				iAddNum = CalculateFillSecond(static_cast<QsFishKind*>(pFishInfo), iNum);
			}
			else
			{
				if (pFishInfo->CanFillFish(m_bFillMinFish))
				{
					if (m_bFillMinFish)
						iAddNum =  pFishInfo->m_nMax - pFishInfo->m_iCurrentNum;
					else
						iAddNum = (int)(RandomInt2(pFishInfo->m_iCurrentNum, pFishInfo->m_nMax)) - pFishInfo->m_iCurrentNum;

					if (iAddNum <= 0)
						iAddNum = 1;
				}
				if (iAddNum <= 0)
				{
					index = QcMath::surround(index + 1, (int)pFishKind->m_objList.size());
					pFishInfo = pFishKind->m_objList[index];
				}
			}
		} 
		while (iAddNum <= 0 && iTime < (int)pFishKind->m_objList.size());

		if (iAddNum > 0)
		{
			iAddNum = QcMath::min_(iAddNum, (int)iNum);

			if (!pFishInfo->IsKind())
			{
				pFishInfo->AddFish(iAddNum);
			}

			iNum -= iAddNum;
			nNum += iAddNum;
		}
		else
		{
			break;
		}
	}
	return nNum;
}

void QcFishControlInfo::FishGenerate(QcList<QcFish*>& fishList)
{
	QcList<QsFishKind*>::iterator iter(m_arKindList.begin());
	QcList<QsFishKind*>::iterator endIter(m_arKindList.end());
	for (; iter != endIter; ++iter)
	{
		(*iter)->FishGenerate(fishList);
	}
}

s32 QcFishControlInfo::AddRandomSameKindFish(const QcResFishDesc* pFishDesc,s32 iCount)
{
	if (QcSameKindBombFish::CanGenSameKindFish(pFishDesc))
	{
		return RandomInt(0, iCount);
	}
	return -1;
}

void QcFishControlInfo::MakeQueueFish(QcList<QcFish*>& fishList)
{
	static const QcObjType arFishPool[] = 
	{
		QmObjName(fish01),
		QmObjName(fish02),
		QmObjName(fish03),
		QmObjName(fish04),
		QmObjName(fish05),
	};
	static s32 iFishPool = RandomInt(0, QmCountOf(arFishPool));
	iFishPool = QcMath::surround(iFishPool + 1, 0, QmCountOf(arFishPool));

	s32 iFishCount = 0;
	if (iFishPool == 0 || iFishPool == 1)
		iFishCount = RandomInt(6, 10);
	else
		iFishCount = RandomInt(3, 7);

	QcResManager* pResMgr = QmResMgr;
	QcPathMgr* pPathMgr = pResMgr->GetPathMgr();
	QcResPathDesc* pPathDesc = NULL;
	u32 fishFlag = 0;
	if (Randomf() < QmBottomRandom)
	{
		pPathDesc = pPathMgr->GetUsableScenePathDesc(QmSceneMgr->GetSceneId(), eSmallFishPath);
		fishFlag |= QcFish::eBottom;
	}
	if (pPathDesc == NULL)
	{
		//pPathDesc = pPathMgr->GetUsablePathDesc(eSmallFishPath);
		pPathDesc = QcResPathDesc::MakeSplinePath(250.f);
		fishFlag = QcFish::eQueue;
	}

	QcResFishDesc* pFishDesc = pResMgr->GetImageDesc(arFishPool[iFishPool])->As<QcResFishDesc*>();
	MakeFishQueue(fishList, pPathDesc, pFishDesc, iFishCount, fishFlag);
}

static void MakeQueuePathKeyPoint(
	QsPathKeyPointList& keyPointList,
	f32 fTotalLen, 
	f32 fGapLen,
	f32 fInitSpeed, 
	f32 fSpeedUpFactor, 
	f32 fTimeUp, 
	f32 fTimeDown,
	f32 fConstantTime
	)
{
	f32 fCurrentSpeed = fInitSpeed;
	f32 fCurrentLen = fGapLen;
	QsPathKeyPoint keyPoint(fCurrentLen);

	//vt + 0.5 * a * t * t
	f32 fLenSpeedUp = fCurrentSpeed * fTimeUp  + 0.5f * fSpeedUpFactor * fTimeUp * fTimeUp;
	fCurrentSpeed += fSpeedUpFactor * fTimeUp;
	f32 fSpeedDownFactor = 0 - fSpeedUpFactor * fTimeUp/fTimeDown;				//减速度
	f32 fLenSpeedDown = fCurrentSpeed * fTimeDown + 0.5f * fSpeedDownFactor * fTimeDown * fTimeDown;
	fCurrentSpeed = fInitSpeed;
	f32 fLenConstantSpeed =  fCurrentSpeed * fConstantTime;

	while (fCurrentLen < fTotalLen)
	{
		//加速起点
		keyPoint.m_fLen = fCurrentLen;
		keyPoint.m_fSpeedFactor = fSpeedUpFactor;
		keyPoint.m_fTime = fTimeUp;
		keyPoint.m_fSpeed = fInitSpeed;
		keyPointList.push_back(keyPoint);
		fCurrentLen += fLenSpeedUp;

		//减少起点
		keyPoint.m_fLen = fCurrentLen;
		keyPoint.m_fSpeedFactor = fSpeedDownFactor;
		keyPoint.m_fTime = fTimeDown;
		keyPoint.m_fSpeed = fInitSpeed +  fSpeedUpFactor * fTimeUp;
		keyPointList.push_back(keyPoint);
		fCurrentLen += fLenSpeedDown;

		keyPoint.m_fLen = fCurrentLen;
		keyPoint.m_fSpeedFactor = 0.f;
		keyPoint.m_fTime = 0.f;
		keyPoint.m_fSpeed = fInitSpeed;
		keyPointList.push_back(keyPoint);
		fCurrentLen += fLenConstantSpeed;
	}
}

void QcFishControlInfo::MakeFishQueue(QcList<QcFish*>& fishList, const QcResPathDesc* pPathDesc, QcResFishDesc* pFishDesc, u32 nCount, u32 eFishFlag)
{
	s32 iSameKindIndex = AddRandomSameKindFish(pFishDesc, nCount);

	QcList<QcResPathDesc*>& pathGroup = const_cast<QcResPathDesc*>(pPathDesc)->GetRelatedPath();
	u32 iCount = 0;
	u32 nAverCount = QcMath::max_(u32(nCount/pathGroup.size()), u32(1));

	QsPathKeyPointList keyInfoList;
	f32 fWidth = pFishDesc->GetWidth() + QmGapWidth;
	f32 fInitSpeed = 120.f;
	f32 fStartTime = 0.f;
	if (nCount >= 6 && (eFishFlag & QcFish::eBottom) == 0 && pathGroup.size() == 1)
		MakeQueuePathKeyPoint(keyInfoList, pPathDesc->GetTotalLen(), fWidth, fInitSpeed, (400.f-fInitSpeed)/2.0f, 2.0f, 2.0f, 4.0f);
	else
	{
		if (eFishFlag & QcFish::eBottom)
			fWidth *= 1.5f;
		fInitSpeed = QmGroupSpeed;		//匀速
	}
	f32 fGapTime = (fWidth)/fInitSpeed;

	
	QcList<QcResPathDesc*>::iterator iter(pathGroup.begin());
	QcList<QcResPathDesc*>::iterator endIter(pathGroup.end());
	for (; iCount < nCount && iter != endIter; ++iter)
	{
		const QcResPathDesc* pThisPathDesc = *iter;
		for (u32 i=0; i<nAverCount && iCount < nCount; ++i)
		{
			QcAnimatorMove* pMoveAnimator = pFishDesc->GetAnimator()->CreateObject<QcAnimatorMove>();
			pMoveAnimator->Reset(pThisPathDesc, fInitSpeed);
			if (pThisPathDesc->GetPathType() >= ePathCount)
			{
				const std::vector<f32>& boundInfo = pPathDesc->GetProperty("BoundInfo");
				pMoveAnimator->AddSpeedAnimator(new QcShelterHandle(boundInfo));
			}
			else if (keyInfoList.size())
				pMoveAnimator->AddSpeedAnimator(new QcPathKeyPointHandler(keyInfoList));
			pMoveAnimator->SetStartTime(fStartTime);
	
			QcFish* pFish = NULL;
			if (iSameKindIndex == 0)
			{
				pFish = QcSameKindBombFish::MakeSameKindBombFish(pFishDesc);
				pFish->SetFishFlag(eFishFlag | QcFish::eCalController);
			}
			else
			{
				pFish = pFishDesc->MakeInstance()->As<QcFish*>();
				pFish->SetFishFlag(eFishFlag);
			}
			pFish->SetPosition(pThisPathDesc->GetBasePoint());
			pFish->SetRotation(pThisPathDesc->GetStartRotation());
			pFish->AddAnimator(pMoveAnimator);

			//QmAssert(!pFish->IsIntersectScreen());
			fStartTime += fGapTime;
			fishList.push_back(pFish);

			--iSameKindIndex;
			++iCount;
		}
	}
}

void QcFishControlInfo::MakeGroupFish(QcList<QcFish*>& fishList)
{
	static const QcObjType arFishPool[] = 
	{
		QmObjName(fish01),
		QmObjName(fish02),
		QmObjName(fish03),
		QmObjName(fish04),
	};
	static s32 iFishPool = RandomInt(0, QmCountOf(arFishPool));
	iFishPool = QcMath::surround(iFishPool + 1, 0, QmCountOf(arFishPool));

	s32 iFishCount = 0;
	if (iFishPool == 0 || iFishPool == 1)
		iFishCount = 4;
	else
		iFishCount = 3;

	QcResManager* pResMgr = QmResMgr;
	QcPathMgr* pPathMgr = pResMgr->GetPathMgr();
	QcResFishDesc* pFishDesc = pResMgr->GetImageDesc(arFishPool[iFishPool])->As<QcResFishDesc*>();
	QcResPathDesc* pPathDesc = pPathMgr->GetUsablePathDesc(eGroupPath);
	QcList<QcResPathDesc*>& pathGroup = const_cast<QcResPathDesc*>(pPathDesc)->GetRelatedPath();

	if (RandomInt(0, 2) < 1)
		return MakeFishQueue(fishList, pPathDesc, pFishDesc, pathGroup.size() * iFishCount, QcFish::eQueue);

	f32 fWidth = pFishDesc->GetWidth() + QmGapWidth;
	f32 fHeight = pFishDesc->GetHeight();
	f32 fInitSpeed = 120.f;
	QsPathKeyPointList keyInfoList;
 	if (Randomf() < 0.2f)
 		MakeQueuePathKeyPoint(keyInfoList, pPathDesc->GetTotalLen(), fWidth, fInitSpeed, (350.f-fInitSpeed)/2.0f, 2.0f, 2.0f,  4.0f);
 	else
		fInitSpeed = QmGroupSpeed;		//匀速
	f32 fGapTime = fWidth/fInitSpeed;

	s32 iSameKindIndex = AddRandomSameKindFish(pFishDesc, pathGroup.size() * iFishCount);
	
	QcVector yVec(0.f, -fHeight);
	QcPoint vStartPos(0.f, pathGroup.size() * 0.5f * fHeight);
	QcPoint vItemPos;
	f32 fDelayTime = 0.f;

	QcList<QcResPathDesc*>::iterator iter(pathGroup.begin());
	QcList<QcResPathDesc*>::iterator endIter(pathGroup.end());
	for (; iter != endIter; ++iter)
	{
		const QcResPathDesc* pThisPathDesc = *iter;
		const QcPoint& vBasePos = pThisPathDesc->GetBasePoint();
		f32 fRotation = pThisPathDesc->GetStartRotation();
		f32 fCs = cos(fRotation);
		f32 fSn = sin(fRotation);
		QcPoint vItemStartPos = vStartPos;

		for (s32 i=0; i<iFishCount; ++i)
		{
			QcAnimatorMove* pMoveAnimator = pFishDesc->GetAnimator()->CreateObject<QcAnimatorMove>();
			pMoveAnimator->Reset(pThisPathDesc, fInitSpeed);
			if (keyInfoList.size())
				pMoveAnimator->AddSpeedAnimator(new QcPathKeyPointHandler(keyInfoList));
			pMoveAnimator->SetStartTime(fDelayTime);

			vItemPos = vItemStartPos;
			vItemPos.rotateBy(fCs, fSn);
			vItemPos += vBasePos;

			QcFish* pFish = NULL;
			if (iSameKindIndex == 0)
			{
				pFish = QcSameKindBombFish::MakeSameKindBombFish(pFishDesc);
				pFish->SetFishFlag(QcFish::eCalController | QcFish::eRegular);
			}
			else
			{
				pFish = pFishDesc->MakeInstance()->As<QcFish*>();
				pFish->SetFishFlag(QcFish::eRegular);
			}
			pFish->SetPosition(vItemPos);
			pFish->SetRotation(fRotation);
			
			pFish->AddAnimator(pMoveAnimator);
			fishList.push_back(pFish);
			
			//QmAssert(!pFish->IsIntersectScreen());
			fDelayTime += fGapTime;
			--iSameKindIndex;
		}
		fDelayTime = 0.f;
		vStartPos += yVec;
	}
}

void QcFishControlInfo::MakeGroup2Fish(QcList<QcFish*>& fishList)
{
	static const QcObjType arFishPool[] = 
	{
		QmObjName(fish01),
		QmObjName(fish02),
	};
	static s32 iFishPool = RandomInt(0, QmCountOf(arFishPool));
	iFishPool = QcMath::surround(iFishPool + 1, 0, QmCountOf(arFishPool));

	QcResManager* pResMgr = QmResMgr;
	QcPathMgr* pPathMgr = pResMgr->GetPathMgr();
	QcResFishDesc* pFishDesc = pResMgr->GetImageDesc(arFishPool[iFishPool])->As<QcResFishDesc*>();
	QcResPathDesc* pPathDesc = pPathMgr->GetUsablePathDesc(eGroupPath2);
	QcList<QcResPathDesc*>& pathGroup = const_cast<QcResPathDesc*>(pPathDesc)->GetRelatedPath();

	f32 fWidth = pFishDesc->GetWidth() + QmGapWidth;
	f32 fInitSpeed = 120.f;
	QsPathKeyPointList keyInfoList;
  	if (Randomf() < 0.8f)
  		MakeQueuePathKeyPoint(keyInfoList, pPathDesc->GetTotalLen(), fWidth, fInitSpeed, (350.f-fInitSpeed)/2.0f, 2.0f, 2.0f,  4.0f);
  	else
		fInitSpeed = QmGroupSpeed;		//匀速
	f32 fGapTime = fWidth/fInitSpeed;

	s32 iSameKindIndex = AddRandomSameKindFish(pFishDesc, pathGroup.size());
	QcList<QcResPathDesc*>::iterator iter(pathGroup.begin());
	QcList<QcResPathDesc*>::iterator endIter(pathGroup.end());
	for (; iter != endIter; ++iter)
	{
		const QcResPathDesc* pThisPathDesc = *iter;
		const QcPoint& vBasePos = pThisPathDesc->GetBasePoint();
		f32 fRotation = pThisPathDesc->GetStartRotation();
		f32 fCs = cos(fRotation);
		f32 fSn = sin(fRotation);

		s32 iFishCount = RandomInt2(1,2);
		//QcVector vec(-fWidth * Randomf(0.4f, 0.6f), -fHeight * Randomf(-0.2f, 0.2f));
		f32 fDelayTime = 0.f;
		QcPoint pos;
		for (s32 i=0; i<iFishCount; ++i)
		{
			QcAnimatorMove* pMoveAnimator = pFishDesc->GetAnimator()->CreateObject<QcAnimatorMove>();
			pMoveAnimator->Reset(pThisPathDesc, fInitSpeed);
			if (keyInfoList.size())
				pMoveAnimator->AddSpeedAnimator(new QcPathKeyPointHandler(keyInfoList));
			pMoveAnimator->SetStartTime(fDelayTime);

			QcFish* pFish = NULL;
			if (iSameKindIndex == 0)
			{
				pFish = QcSameKindBombFish::MakeSameKindBombFish(pFishDesc);
				pFish->SetFishFlag(QcFish::eCalController | QcFish::eRegular);
			}
			else
			{
				pFish = pFishDesc->MakeInstance()->As<QcFish*>();
				pFish->SetFishFlag(QcFish::eRegular);
			}
			pFish->SetPosition(vBasePos + pos);
			pFish->SetRotation(fRotation);
			pFish->AddAnimator(pMoveAnimator);
			fishList.push_back(pFish);

			//QmAssert(!pFish->IsIntersectScreen());
			//pos += vec;
			pos.rotateBy(fCs, fSn);
			fDelayTime += fGapTime * 0.8f;
			--iSameKindIndex;
		}
	}
}

static QcVector MakePattern(QcPointList& pointList, u32 nPolyLine, u32 nPointOnceLine)
{
	pointList.reserve(pointList.size() + nPolyLine * nPointOnceLine);
	QcArray<QcPoint, 10> arPointList(nPolyLine, true);
	QcArray<QcVector, 10> arVecList(nPolyLine, true);

	f32 fRad = TWO_PI/nPolyLine;
	arPointList[0].X = 1.0f;
	arPointList[0].Y = 0.0f;
	if (nPolyLine%2 == 0)
	{
		arPointList[0].rotateByRad(fRad * 0.5f);
	}
	f32 fCs = cos(fRad);
	f32 fSn = sin(fRad);
	for (u32 i=1; i<nPolyLine; ++i)
	{
		arPointList[i] = arPointList[i-1];
		arPointList[i].rotateBy(fCs, fSn);
	}
	for (u32 i=0; i<nPolyLine-1; ++i)
	{
		arVecList[i] = arPointList[i + 1] - arPointList[i];
	}
	arVecList[nPolyLine-1] = arPointList[0] - arPointList[nPolyLine-1];
	f32 fLen = arVecList[0].getLength();
	f32 fGapLen = fLen/nPointOnceLine;

	QcPoint tempPoint;
	QcVector tempVec;
	for (u32 i=0; i<nPolyLine; ++i)
	{
		arVecList[i].normalize(fLen);

		tempVec = arVecList[i];
		tempVec *= fGapLen;
		tempPoint = arPointList[i];
		for (u32 j=0; j<nPointOnceLine; ++j)
		{
			pointList.push_back(tempPoint);
			tempPoint += tempVec;
		}
	}
	return arVecList[0] * fGapLen;
}

void QcFishControlInfo::MakeCirclePattern(QcList<QcFish*>& fishList, u32 nCount, f32 fGapWidth)
{
	static const QcObjType mainObjTypePool[] = 
	{
		QmObjName(fish11),
		QmObjName(fish13),
		QmObjName(fish14),
		QmObjName(fish15),
	};
	static const QcObjType subObjTypePool[] = 
	{
		QmObjName(fish01),
		QmObjName(fish02),
	}; 
	const QcResFishDesc* pMainFishDesc = static_cast<const QcResFishDesc*>(QmResMgr->GetImageDesc(mainObjTypePool[RandomInt(0, QmCountOf(mainObjTypePool))]));
	const QcResFishDesc* pSubFishDesc = static_cast<const QcResFishDesc*>(QmResMgr->GetImageDesc(subObjTypePool[RandomInt(0, QmCountOf(subObjTypePool))] ));

	int iLayer = pMainFishDesc->GetFishType();
	int nSubCount = RandomInt(8,13);
	QcPointList pointList;
	MakePattern(pointList, nSubCount, 1);
	f32 fScalePoint = ( (pMainFishDesc->GetWidth() + pMainFishDesc->GetHeight()) * 0.5f + (pSubFishDesc->GetWidth() + pSubFishDesc->GetHeight()) * 0.5f) * 0.5f;

	s32 iSameKindIndex = AddRandomSameKindFish(pSubFishDesc, nSubCount);

	QsPathKeyPointList keyPointList;
	QcResPathDesc* pPathDesc = NULL;
	if (pMainFishDesc->GetObjType() == QmObjName(fish11))
		pPathDesc = QcResPathDesc::MakeGravityPath(keyPointList, pMainFishDesc->GetWidth() + pSubFishDesc->GetWidth() * 0.5f);
	else
		pPathDesc = QcResPathDesc::MakeRegularPath(pMainFishDesc->GetWidth() + (pSubFishDesc->GetWidth() + pSubFishDesc->GetHeight()));

	const QcPoint& vBasePos = pPathDesc->GetBasePoint();
	f32 fRotation = pPathDesc->GetStartRotation();
	f32 fCs = cos(fRotation);
	f32 fSn = sin(fRotation);
	f32 fDelayTime = 0.f;
	for (int i=0; i<(int)nCount; ++i)
	{
		QcSharedPtr<QcAnimatorMove> pMoveAnimator(pMainFishDesc->GetAnimator()->CreateObject<QcAnimatorMove>());
		pMoveAnimator->Reset(pPathDesc, pMainFishDesc->GetSpeed());
		pMoveAnimator->SetStartTime(fDelayTime);
		if (pMoveAnimator->Is<QcGravityFishAnimator*>())
			static_cast<QcGravityFishAnimator*>(pMoveAnimator.Get())->SetKeyPontList(keyPointList);

		QcFish* pGravityFish = pMainFishDesc->MakeInstance()->As<QcFish*>();
		pGravityFish->SetFishFlag(QcFish::eRegular);
		if (pGravityFish->GetObjType() == QmObjName(fish15))
		{
			QcVector vec(1.0f,0);
			vec.rotateByRad(fRotation);
			vec *= 70.f;
			pGravityFish->SetPosition(vBasePos-vec);
		}
		else
			pGravityFish->SetPosition(vBasePos);
		pGravityFish->SetRotation(fRotation);
		pGravityFish->AddAnimator(pMoveAnimator);
		fishList.push_back(pGravityFish);

		QcPointList::iterator iter(pointList.begin());
		QcPointList::iterator endIter(pointList.end());
		for (; iter != endIter; ++iter)
		{
			QcPoint& pos = *iter;
			pos.rotateBy(fCs, fSn);
			pos *= fScalePoint;

			QcFish* pFish = NULL;
			if (iSameKindIndex == 0)
			{
				pFish = QcSameKindBombFish::MakeSameKindBombFish(pSubFishDesc);
				pFish->SetFishFlag(QcFish::eCalController | QcFish::eRegular);
			}
			else
			{
				pFish = pSubFishDesc->MakeInstance()->As<QcFish*>();
				pFish->SetFishFlag(QcFish::eRegular);
			}
			pFish->SetLayer(iLayer);
			pFish->SetPosition(pos + vBasePos);
			pFish->SetRotation(fRotation);
			pFish->AddAnimator(pMoveAnimator);
			fishList.push_back(pFish);

			//QmAssert(!pFish->IsIntersectScreen());
			--iSameKindIndex;
		}
		fDelayTime += (fGapWidth + pMainFishDesc->GetWidth() * 0.5f + pSubFishDesc->GetWidth())/pMainFishDesc->GetSpeed();
	}
}

bool QcFishControlInfo::CanGenSameKindFish(const QcResFishDesc* pFishDesc)
{
	if (NULL == m_pSameKindFishInfo)
		return false;
	return m_oSameKindFishHelper.CanGenSameKindFish(pFishDesc);
}

void QcFishControlInfo::ToDeadGenSameKindFish(const QcResFishDesc* pFishDesc)
{
	if (m_pSameKindFishInfo)
	{
		QsFishPool* pFishInfo = m_oSameKindFishHelper.Find(pFishDesc);
		if (pFishInfo && pFishInfo->m_bInUsed)
		{
			pFishInfo->m_bInUsed = false;
			m_oSameKindFishHelper.Decrease();
		}
	}
}

QcSameKindBombFish* QcFishControlInfo::MakeSameKindBombFish(const QcResFishDesc* pFishDesc)
{
	static const QcResFishDesc* pSameKindFishDesc = QmResMgr->GetImageDesc(QmObjName(SameKindFish))->As<QcResFishDesc*>();
	QsFishPool* pFishInfo = m_oSameKindFishHelper.Find(pFishDesc);
	if (pFishInfo)
	{
		pFishInfo->m_bInUsed = true;
		m_oSameKindFishHelper.Increase();
		QcSameKindBombFish* pSameKindBombFish = new QcSameKindBombFish(pFishInfo->m_pFishDesc);
		pSameKindBombFish->Init(*pSameKindFishDesc, true);
		return pSameKindBombFish;
	}
	return NULL;
}