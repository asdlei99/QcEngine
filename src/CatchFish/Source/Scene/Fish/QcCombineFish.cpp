#include "Pch.hpp"
#include "QcCombineFish.hpp"
#include "Res\QcResCombineFishDesc.hpp"

ImplRuntimeClass(QcCombineFish);

QcCombineFish::QcCombineFish()
{

}

void QcCombineFish::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcFish::Init(desc, bLoadSubNode);
	const QcResCombineFishDesc& combineFishDesc = static_cast<const QcResCombineFishDesc&>(desc);
	const QcStringList& fishPool = combineFishDesc.GetFishPool();
	u32 iRand = RandomInt(0, (u16)fishPool.size());
	const QcList<QcResCombineFishDesc::QsInfo>& combineInfo = combineFishDesc.GetCombineFishInfo();
	QcList<QcResCombineFishDesc::QsInfo>::const_iterator iter(combineInfo.begin());

	u32 iScore = GetScore();
	bool bAddScore = iScore == 0;
	for (; iter != combineInfo.end(); ++iter)
	{
		QcObjType fishType = iter->m_combineFish;
		if (fishType.empty())
		{
			if (!combineFishDesc.IsSameFish())
			{
				iRand = RandomInt(0, (u16)fishPool.size());
			}
			fishType = fishPool[iRand];
		}
		QcResFishDesc* pFishDesc = QmResMgr->GetImageDesc(fishType)->As<QcResFishDesc*>();
		if (bAddScore)
		{
			iScore += pFishDesc->GetScore();
		}
		QcSceneNode* pSceneNode = QfMakeSceneNodeInstance<QcSceneNode>(iter->m_circleImage);
		AddSubSceneNode(pSceneNode);
		pSceneNode->SetPosition(iter->m_pos);
		if (iter->m_size.Width < pSceneNode->GetWidth() || iter->m_size.Height < pSceneNode->GetHeight())
			pSceneNode->ScaleTo(iter->m_size.Width, iter->m_size.Height);

		pSceneNode = QfMakeSceneNodeInstance<QcSceneNode>(fishType);
		AddSubSceneNode(pSceneNode);
		pSceneNode->SetPosition(iter->m_pos);
		if (iter->m_size.Width < pSceneNode->GetWidth() || iter->m_size.Height < pSceneNode->GetHeight())
			pSceneNode->ScaleTo(iter->m_size.Width, iter->m_size.Height);
	}
	SetScore(iScore);
}