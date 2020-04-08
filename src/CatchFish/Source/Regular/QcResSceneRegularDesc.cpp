#include "Pch.hpp"
#include "QcResSceneRegularDesc.hpp"
#include "QcResFishRegularDesc.hpp"
#include "QcDefaultSceneRegularMaker.hpp"
#include "QcRegularMgr.hpp"
#include "Res\QcPathMgr.hpp"
#include "Res\QcResPathDesc.hpp"

ImplRuntimeClass(QcResSceneRegularDesc);

QcResSceneRegularDesc::QcResSceneRegularDesc()
{

}

bool QcResSceneRegularDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	bool bRet = QcResDesc::LoadDesc(node);

	m_objType = node.Attribute("name");
	m_fLivingTime = node.Attribute("LivingTime", 0.f);
	m_pRunClass = QmClassFactoryInstance->Find(node.Attribute("Maker"));
	if (m_pRunClass == NULL)
		m_pRunClass = QmRuntimeClass(QcDefaultSceneRegularMaker);

	QcRegularMgr* pFishRegularMgr = QmResMgr->GetRegularMgr();
	QcPathMgr* pPathMgr = QmResMgr->GetPathMgr();

	QcXmlNodeIterator itemNode = node.FirstNode("Item");
	while (itemNode)
	{
		QsItemInfo itemInfo;
		QcObjType path = itemNode.Attribute("path");
		itemInfo.m_pRegularDesc = pFishRegularMgr->FindFishRegularDesc(QcObjType(itemNode.Attribute("regular")));
		if (NULL == itemInfo.m_pRegularDesc)
		{
			itemInfo.m_pRegularDesc = QcResFishRegularDesc::MakeSingleFishRegular(QcObjType(itemNode.Attribute("fish")));
		}
		if (path.size())
		{
			itemInfo.m_pPathDesc = const_cast<QcResPathDesc*>(pPathMgr->GetPathDesc(path));
			if (NULL == itemInfo.m_pPathDesc)
			{
				itemInfo.m_pPathDesc = new QcResPathDesc();
				itemInfo.m_pPathDesc  = itemInfo.m_pPathDesc->Load(path);
			}
		}
		else
		{
			const char* psDynPath = itemNode.Attribute("DynPath");
			QcPoint point;
			f32 fRoration;
			QmAssert2(sscanf_s(psDynPath, "%f,%f,%f", &point.X, &point.Y, &fRoration) == 3);
			itemInfo.m_pPathDesc = QcResPathDesc::MakeRegularPath(point, degToRad(fRoration), itemInfo.m_pRegularDesc->GetWidth());
		}
		itemInfo.m_fSpeed = itemNode.Attribute("speed", 160.f);
		itemInfo.m_nCount = itemNode.Attribute("count", 1);
		itemInfo.m_fGapWidth = itemNode.Attribute("gapWidth", 10.f);

		itemInfo.m_id = itemNode.Attribute("id", -1);
		itemInfo.m_fLivingTime = itemNode.Attribute("livingTime", 0.f);
		itemInfo.m_fStartTime = itemNode.Attribute("startTime", 0.f);
		itemInfo.m_iStartWaitItem = itemNode.Attribute("startWait", -1);
		itemInfo.m_iEndWaitItem = itemNode.Attribute("endWait", -1);

		m_arRegularList.push_back(itemInfo);

		itemNode = itemNode.NextSibling("Item");
	}
	
	QcList<QsItemInfo>::iterator iter(m_arRegularList.begin());
	QcList<QsItemInfo>::iterator endIter(m_arRegularList.end());
	for (; iter != endIter; ++iter)
	{
		if (iter->m_iStartWaitItem >= 0)
			iter->m_iStartWaitItem = FindItem(iter->m_iStartWaitItem);
		if (iter->m_iEndWaitItem >= 0)
			iter->m_iEndWaitItem = FindItem(iter->m_iEndWaitItem);
	}

	return bRet;
}

int QcResSceneRegularDesc::FindItem(int id)
{
	int nCount = (int)m_arRegularList.size();
	for (int i=0; i<nCount; ++i)
	{
		if (m_arRegularList[i].m_id == id)
			return i;
	}
	return -1;
}