#include "pch.hpp"
#include "QcResFishRegularDesc.hpp"
#include "Res\QcResPathDesc.hpp"
#include "Res\QcResFishDesc.hpp"
#include "Res\QcPathMgr.hpp"
#include "Animator\FishAnimator\QcDefaultFishAnimator.hpp"

ImplRuntimeClass(QcResFishRegularDesc);

QcResFishRegularDesc::QcResFishRegularDesc() : m_iLayer(0){}

QcResFishRegularDesc* QcResFishRegularDesc::MakeSingleFishRegular(const QcObjType& objType)
{
	QcResFishRegularDesc* pFishRegularDesc = new QcResFishRegularDesc();
	QsRegularItem item;
	item.m_sFishName = objType;
	item.m_fScale = 1.0f;
	item.m_pointList.push_back(QcPoint(0.f, 0.f));

	static int iDynNum = 0;
	char buf[128] = {0};
	sprintf_s(buf, "DynFishRegular%d", iDynNum++);
	pFishRegularDesc->m_pAnimator =  QmRuntimeClass(QcDefaultFishAnimator);
	pFishRegularDesc->m_objType = buf;
	pFishRegularDesc->m_fRandom = 0.f;
	pFishRegularDesc->m_bOnlyInSceneRegular = true;
	const QcResFishDesc* pFishDesc = QmResMgr->GetImageDesc(objType)->As<const QcResFishDesc*>();
	pFishRegularDesc->m_iLayer = pFishDesc->GetFishType();
	pFishRegularDesc->m_fWidth = pFishDesc->GetWidth();
	pFishRegularDesc->m_arRegularItem.PushBack(item);

	return pFishRegularDesc;
}

bool QcResFishRegularDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	QcResDesc::LoadDesc(node);

	QcPathMgr* pPathMgr = QmResMgr->GetPathMgr();

	QcString referencePath;
	m_objType = node.Attribute("name");
	referencePath = node.Attribute("referencePath");
	m_fRandom = node.Attribute("random", 0.0f);
	m_bOnlyInSceneRegular = node.Attribute("SceneRegularOnly", 0) != 0;

	const char* psPathList[] = 
	{
		"Path",
		"SceneAPath",
		"SceneBPath",
		"SceneCPath",
	};
	for (int i=0; i<QmCountOf(psPathList); ++i)
	{
		const char* psList = node.Attribute(psPathList[i]);
		if (*psList == '*')
		{
			m_arWalkablePathList[i] = (i==0) ? pPathMgr->GetNormalPathList(eRegularPath) : pPathMgr->GetScenePathList(i-1, eRegularPath);
		}
		else
		{
			QcStringList pathList;
			QfSplitString(psList, pathList, ';');
			QcStringListIter iter(pathList.begin());
			QcStringListIter endIter(pathList.end());
			for (; iter != endIter; ++iter)
			{
				m_arWalkablePathList[i].push_back(pPathMgr->GetPathDesc(*iter));
			}
		}
	}

	m_pAnimator = QmClassFactoryInstance->Find(node.Attribute("Animator"));
	if (m_pAnimator == NULL)
		m_pAnimator =  QmRuntimeClass(QcDefaultFishAnimator);

	LoadRegularItem(node);

	m_fWidth = 0.f;
	bool bCalWidth = QcMath::equals(m_fWidth, 0.f);
	f32 fMinWidth = gfMax;
	f32 fMaxWidth = gfMin;
	f32 fMinHeight = gfMax;
	f32 fMaxHeight = gfMin;
	bool bRotate = false;
	f32 fCs = 0.f;
	f32 fsn = 0.f;
#if 1
	if (referencePath.size())
	{
		const QcResPathDesc* pPathDesc = pPathMgr->GetPathDesc(referencePath);
		f32 fStartRotation = pPathDesc->GetStartRotation();
		fCs = cos(- fStartRotation);
		fsn = sin(- fStartRotation);
		bRotate = true;
	}
#endif
	m_iLayer = QmCountOf(QaFishList) + 1;
	QcList<QsRegularItem>::iterator itemIter(m_arRegularItem.begin());
	QcList<QsRegularItem>::iterator endItemIter(m_arRegularItem.end());
	for (; itemIter != endItemIter; ++itemIter)
	{
		f32 fHalfWidth = 0.f;
		f32 fHalfHeight = 0.f;

		const QcResFishDesc* pFishDesc = QmResMgr->GetImageDesc(itemIter->m_sFishName)->As<const QcResFishDesc*>();
		if (bCalWidth)
		{
			fHalfWidth = pFishDesc->GetWidth() * 0.5f * itemIter->m_fScale;
			fHalfHeight = pFishDesc->GetHeight() * 0.5f * itemIter->m_fScale;
		}
		m_iLayer = QcMath::min_(pFishDesc->GetFishType(), m_iLayer);
		QcPointList::iterator ptIter(itemIter->m_pointList.begin());
		QcPointList::iterator ptEndIter(itemIter->m_pointList.end());
		for (; ptIter != ptEndIter; ++ptIter)
		{
			QcPoint& pt = *ptIter;
			
			if (bRotate)
				pt.rotateBy(fCs, fsn);
			if (bCalWidth)
			{
				fMinWidth = QcMath::min_(pt.X - fHalfWidth, fMinWidth);
				fMaxWidth = QcMath::max_(pt.X + fHalfWidth, fMaxWidth);

				fMinHeight = QcMath::min_(pt.Y - fHalfHeight, fMinHeight);
				fMaxHeight = QcMath::max_(pt.Y + fHalfHeight, fMaxHeight);
			}
		}
	}
	if (bCalWidth)
	{
		m_fWidth = fMaxWidth - fMinWidth;

		//ÐÞÕý×ø±ê
		QcPoint centerPt((fMinWidth + fMaxWidth) * 0.5f, (fMinHeight + fMaxHeight) * 0.5f);
		itemIter = (m_arRegularItem.begin());
		endItemIter = (m_arRegularItem.end());
		for (; itemIter != endItemIter; ++itemIter)
		{
			QcPointList::iterator ptIter(itemIter->m_pointList.begin());
			QcPointList::iterator ptEndIter(itemIter->m_pointList.end());
			for (; ptIter != ptEndIter; ++ptIter)
			{
				QcPoint& pt = *ptIter;
				pt -= centerPt;
			}
		}
	}
	return true;
}

void QcResFishRegularDesc::LoadRegularItem(const QcXmlNodeIterator& node)
{
	QcXmlNodeIterator itemNode = node.FirstNode("Item");
	while(itemNode)
	{
		QsRegularItem item;
		item.m_sFishName = itemNode.Attribute("Kind");
		item.m_fScale = itemNode.Attribute("Scale", 1.0f);

		QcUtil::StringToPointList(itemNode.Attribute("FishPos"), item.m_pointList, "(", ",");
		m_arRegularItem.push_back(item);
		itemNode = itemNode.NextSibling("Item");
	}
}