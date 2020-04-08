#include "pch.hpp"
#include "QcResCombineFishDesc.hpp"
#include "Scene\Fish\QcCombineFish.hpp"

ImplRuntimeClass2(QcResCombineFishDesc, 10);

QcResCombineFishDesc::QcResCombineFishDesc()
{
	m_pRuntimeClass = QmRuntimeClass(QcCombineFish);
}

bool QcResCombineFishDesc::LoadDesc(const QcXmlNodeIterator& node)
{
	return QcResFishDesc::LoadDesc(node) && LoadCombineInfo(node);
}

bool QcResCombineFishDesc::LoadCombineInfo(const QcXmlNodeIterator& node)
{
	QcXmlNodeIterator combineNode = node.FirstNode("CombineDesc");
	if (combineNode)
	{
		QcObjType circleImage = combineNode.Attribute("circleImage");
		QcSize size = combineNode.Attribute("size", QcSize(0.f,0.f));
		m_bSameFish = combineNode.Attribute("Same", 0) > 0;
		QfSplitString(combineNode.Attribute("FishPool"), m_arFishPool, ';');

		QsInfo info;
		QcXmlNodeIterator combineInfoNode = combineNode.FirstNode();
		while (combineInfoNode)
		{
			info.m_pos = combineInfoNode.Attribute("Pos", info.m_pos);
			info.m_size = combineInfoNode.Attribute("size", size);
			info.m_combineFish = combineInfoNode.Attribute("Fish");
			info.m_circleImage = combineInfoNode.Attribute("circleImage");
			if (info.m_circleImage.empty())
				info.m_circleImage = circleImage;
			m_arCombineFishInfo.push_back(info);

			combineInfoNode = combineInfoNode.NextSibling();
		}
	}
	return true;
}

void QcResCombineFishDesc::PostLoad(QcResManager* pResMgr)
{
	if (m_fWidth < ROUNDING_ERROR_f32 || m_fHeight < ROUNDING_ERROR_f32)
	{
		QcRect rect;
		QcList<QsInfo>::iterator iter(m_arCombineFishInfo.begin());
		for (; iter != m_arCombineFishInfo.end(); ++iter)
		{
			QcRect subRect(iter->m_pos, iter->m_size);
			rect.unionRect(subRect);
		}
		m_fWidth = rect.getWidth();
		m_fHeight = rect.getHeight();
	}
	QcResFishDesc::PostLoad(pResMgr);

	QcList<QsInfo>::iterator iter(m_arCombineFishInfo.begin());
	for (; iter != m_arCombineFishInfo.end(); ++iter)
	{
		QcResImageDesc* pResDesc = pResMgr->GetImageDesc(iter->m_circleImage);
		if (pResDesc == NULL)
		{
			QcResImageDescPtr pImageDesc(new QcResImageDesc());
			pImageDesc->LoadDesc(iter->m_circleImage.c_str());
			pResMgr->LoadRes(pImageDesc);
		}
	}
}