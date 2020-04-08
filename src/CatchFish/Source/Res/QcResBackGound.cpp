#include "pch.hpp"
#include "QcResBackGound.hpp"
#include "Res\QcResPathDesc.hpp"
#include "Res\QcPathMgr.hpp"
#include "Res\QcResManager.hpp"

ImplRuntimeClass(QcResBackGound);

QcResBackGound::QcResBackGound()
{
	m_pRuntimeClass = QmClassFactoryInstance->Find("QcBackGround");
}

bool QcResBackGound::LoadDesc(const QcXmlNodeIterator& node)
{
	bool bRet = QcResImageDesc::LoadDesc(node);

	m_nSceneID = QmArrayFind(QaSceneList, GetObjType());

	std::map<QcResPathDesc*, std::vector<f32>> pairListMap;
	QsBgNode bgNode;
	QcXmlNodeIterator sceneNode = node.FirstNode("SceneNode");
	while(sceneNode)
	{
		bgNode.m_sNodeName = sceneNode.Attribute("name");
		bgNode.m_vPos = sceneNode.Attribute("pos", bgNode.m_vPos);
		bgNode.m_vPointList.clear();

		QcUtil::StringToPointList(sceneNode.Attribute("Bound"), bgNode.m_vPointList, "(", ",");
		bgNode.m_vPointList.push_back(bgNode.m_vPointList.front());

		m_arNodeList.push_back(bgNode);

		f32 fOffset1 = 60.f;
		f32 fOffset2 = 80.f;
		f32 fOffset3 = 60.f;

		for (int i=0; i<ePathCount; ++i)
		{
			QcList<QcResPathDesc*>& bottomPathList = QmResMgr->GetPathMgr()->GetScenePathList(m_nSceneID, (QePathType)i);

			QcList<QcResPathDesc*>::iterator iter(bottomPathList.begin());
			QcList<QcResPathDesc*>::iterator endIter(bottomPathList.end());
			for (; iter != endIter; ++iter)
			{
				QcResPathDesc* pPathDesc = const_cast<QcResPathDesc*>(*iter);

				QcVector vec;
				QcList<QcResPathDesc*>::iterator pathIter(pPathDesc->GetRelatedPath().begin());
				QcList<QcResPathDesc*>::iterator endPathIter(pPathDesc->GetRelatedPath().end());
				for (; pathIter != endPathIter; ++pathIter)
				{
					QcResPathDesc* pPathDesc = *pathIter;
					if (CalculateIntersectPt(bgNode.m_vPointList, pPathDesc, vec))
					{
						QmAssert( (vec.Y - vec.X) > (fOffset1 + fOffset2 + fOffset3) * 2.f);
						pairListMap[pPathDesc].push_back(vec.X);
						pairListMap[pPathDesc].push_back(vec.X + fOffset1);
						pairListMap[pPathDesc].push_back(vec.X + fOffset1 + fOffset2);
						pairListMap[pPathDesc].push_back(vec.X + fOffset1 + fOffset2 + fOffset3);
						pairListMap[pPathDesc].push_back(vec.Y - fOffset1 - fOffset2 - fOffset3);
						pairListMap[pPathDesc].push_back(vec.Y - fOffset1 - fOffset2);
						pairListMap[pPathDesc].push_back(vec.Y - fOffset1);
						pairListMap[pPathDesc].push_back(vec.Y);
					}
					else
					{
						QmAssert(false);
					}
				}
			}
		}
		sceneNode = sceneNode.NextSibling("SceneNode");
	}

    sceneNode = node.FirstNode("DownsideObj");
    while (sceneNode)
    {
        m_arDownsideObj.push_back(sceneNode.Attribute("name"));
        sceneNode = sceneNode.NextSibling("DownsideObj");
    }

    sceneNode = node.FirstNode("UpsideObj");
    while (sceneNode)
    {
        m_arUpsideObj.push_back(sceneNode.Attribute("name"));
        sceneNode = sceneNode.NextSibling("UpsideObj");
    }

	std::map<QcResPathDesc*, std::vector<f32>>::iterator iter(pairListMap.begin());
	std::map<QcResPathDesc*, std::vector<f32>>::iterator endIter(pairListMap.end());
	for (; iter != endIter; ++iter)
	{
		std::sort(iter->second.begin(), iter->second.end(), std::less<f32>());
		iter->first->SetProperty("BoundInfo", iter->second);
	}
	return bRet;
}

bool QcResBackGound::LoadAttibute(const QcXmlNodeIterator& node)
{
	bool bRet = QcResImageDesc::LoadAttibute(node);

	m_oBgMusic = node.Attribute("BgMusic");
	m_oBgWave = node.Attribute("Wave");
	m_fWaveAlphaValue = node.Attribute("WaveAlphaValue",1.0f);
	return bRet;
}

void QcResBackGound::PostLoad(QcResManager* pResMgr)
{
	QcResImageDesc::PostLoad(pResMgr);

	QcList<QsBgNode>::iterator iter(m_arNodeList.begin());
	QcList<QsBgNode>::iterator endIter(m_arNodeList.end());
	for (; iter != endIter; ++iter)
	{
		QcResImageDescPtr pImageDesc(new QcResImageDesc());
		pImageDesc->LoadDesc(iter->m_sNodeName.c_str());
		pResMgr->LoadRes(pImageDesc);
	}
}

bool QcResBackGound::CalculateIntersectPt(const QcPointList& pointList, QcResPathDesc* pPathDesc, QcVector& vec)
{
	const QcList<QsPathNodeInfo>& pathNodeInfo = pPathDesc->GetSeqList();

	QcPolygon polygon(pointList);
	//找出路径与潜入区域的
	QcList<QsPathNodeInfo>::const_iterator iter(pathNodeInfo.begin());
	QcList<QsPathNodeInfo>::const_iterator endIter(pathNodeInfo.end());

	vec.X = 0.f;
	vec.Y = 0.f;
	int iCount = 0;

	QcPoint temp1;
	QcPoint temp2;
	int iTempCount = 0;
	QcLine line;

	for (; iter != endIter; ++iter)
	{
		line.start = iter->m_vPos;
		line.end = iter->m_vPos + iter->m_vec * iter->m_fLen;

		iTempCount = polygon.intersectWith(line, temp1, &temp2);
		if (iTempCount)
		{
			if (iTempCount == 2)
			{
				QmAssert(iCount == 0);

				f32 fLen1 = QcVector(temp1 - iter->m_vPos).getLength();
				f32 fLen2 = QcVector(temp2 - iter->m_vPos).getLength();

				if (fLen1 < fLen2)
				{
					vec.X += fLen1;
					vec.Y += fLen2;
				}
				else
				{
					vec.X += fLen2;
					vec.Y += fLen1;
				}
				iCount = 2;
				break;
			}
			else if (iCount == 0)
			{
				vec.X += (QcVector(temp1 - iter->m_vPos).getLength());
				iCount = 1;
			}
			else
			{
				vec.Y += (QcVector(temp1 - iter->m_vPos).getLength());
				iCount = 2;
				break;
			}
		}
		if (iCount == 0)
		{
			vec.X += iter->m_fLen;
			vec.Y += iter->m_fLen;
		}
		else if (iCount == 1)
		{
			vec.Y += iter->m_fLen;
		}
	}
	return iCount == 2;
}
