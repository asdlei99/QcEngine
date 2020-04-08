#include "pch.hpp"
#include "QcDefaultSceneRegularMaker.hpp"
#include "QcRegularMgr.hpp"
#include "QcResSceneRegularDesc.hpp"
#include "QcResFishRegularDesc.hpp"
#include "Res\QcResPathDesc.hpp"

ImplRuntimeClass(QcDefaultSceneRegularMaker);

QcDefaultSceneRegularMaker::QcDefaultSceneRegularMaker(){}

void QcDefaultSceneRegularMaker::Animate(double dCurrentTime,f32 fElapsedTime, QcList<QcFish*>& fishList)
{
	if (m_nFishCount == 0)
	{
		m_iEmptyTime += fElapsedTime;
		if (m_iEmptyTime > 0.5f)
		{
			//加快出鱼速度
			std::vector<QsGenerateInfo>::iterator iter(m_generateInfo.begin());
			std::vector<QsGenerateInfo>::iterator endIter(m_generateInfo.end());
			int i=0;
			for (; iter != endIter; ++iter, ++i)
			{
				if (iter->m_iState == eRegularWaitStart)
				{
					iter->m_fStartTimeCount -= m_iEmptyTime;
				}
			}
			m_iEmptyTime = 0.f;
		}
	}
	else
	{
		m_iEmptyTime = 0.f;
	}
	Generate(fishList, fElapsedTime);
	if (m_fLivingTime > ROUNDING_ERROR_f32)
	{
		m_fLivingTime -= fElapsedTime;
		if (m_fLivingTime < ROUNDING_ERROR_f32)
			m_bFinished = true;
	}
}

void QcDefaultSceneRegularMaker::BeginSceneRegular(QcRegularMgr* pRegularMgr, QcList<QcFish*>& fishList, QcResSceneRegularDesc* pSceneRegularDesc)
{
	m_fLivingTime = pSceneRegularDesc->GetLivingTime();
	m_pRegularMgr = pRegularMgr;
	m_pSceneRegularDesc = pSceneRegularDesc;
	m_nFishCount = 0;
	m_iEmptyTime = 0.f;
	m_bFinished = false;

	QcResSceneRegularDescList& itemList = pSceneRegularDesc->GetItemList();
	m_generateInfo.resize(itemList.size());

	std::vector<QsGenerateInfo>::iterator iter(m_generateInfo.begin());
	std::vector<QsGenerateInfo>::iterator endIter(m_generateInfo.end());
	int i=0;
	for (; iter != endIter; ++iter, ++i)
	{
		iter->m_iState = eRegularWaitStart;
		iter->m_fStartTimeCount = itemList[i].m_fStartTime;
	}
	Generate(fishList, 0.f);
}

void QcDefaultSceneRegularMaker::Generate(QcList<QcFish*>& fishList, f32 fElapsedTime)
{
	fishList.clear();

	int iCount = m_generateInfo.size();
	for (int i=0; i<iCount; ++i)
	{
		QsGenerateInfo& genInfo = m_generateInfo[i];
		switch(genInfo.m_iState)
		{
		case eRegularWaitStart:
			{
				const QcResSceneRegularDesc::QsItemInfo& itemInfo = m_pSceneRegularDesc->GetItem(i);
				if ((itemInfo.m_iStartWaitItem < 0 || m_generateInfo[itemInfo.m_iStartWaitItem].m_iState == eRegularEnd) )
				{
					if (genInfo.m_fStartTimeCount< 0.f)
					{
						int nCount = itemInfo.m_nCount;
						if (nCount == 0)		//持续出
						{
							nCount = 1;
							genInfo.m_iState = eRegularContinue;
							genInfo.m_fTimeCount = (itemInfo.m_pRegularDesc->GetWidth() + itemInfo.m_fGapWidth)/itemInfo.m_fSpeed;
							genInfo.m_fLivingTimeCount = itemInfo.m_fLivingTime;
						}
						else
						{
							genInfo.m_fLivingTimeCount = 0.f;
							genInfo.m_iState = eRegularWaitEnd;
						}
						genInfo.m_pWaitPathDesc = itemInfo.m_pPathDesc;
						//genInfo.m_iSavePathUseCount = itemInfo.m_pPathDesc->IsUsable();
						m_pRegularMgr->MakeFishRegular(fishList, itemInfo.m_pRegularDesc, itemInfo.m_pPathDesc, itemInfo.m_fSpeed, nCount, itemInfo.m_fGapWidth);
					}
					else
					{
						genInfo.m_fStartTimeCount -= fElapsedTime;
					}
				}
				break;
			}
		case eRegularContinue:
			{
				const QcResSceneRegularDesc::QsItemInfo& itemInfo = m_pSceneRegularDesc->GetItem(i);
				genInfo.m_fLivingTimeCount -= fElapsedTime;
				if (genInfo.m_fLivingTimeCount < 0.f && (itemInfo.m_iEndWaitItem == -1 || m_generateInfo[itemInfo.m_iEndWaitItem].m_iState == eRegularEnd) )
				{
					genInfo.m_iState = eRegularWaitEnd;
				}
				else
				{
					genInfo.m_fTimeCount -= fElapsedTime;
					if (genInfo.m_fTimeCount < 0.f)
					{
						genInfo.m_fTimeCount = (itemInfo.m_pRegularDesc->GetWidth() + itemInfo.m_fGapWidth)/itemInfo.m_fSpeed;
						m_pRegularMgr->MakeFishRegular(fishList, itemInfo.m_pRegularDesc, itemInfo.m_pPathDesc, itemInfo.m_fSpeed, 1, itemInfo.m_fGapWidth);
					}
				}
				break;
			}
		case eRegularWaitEnd:
			{
				//if (genInfo.m_pWaitPathDesc->IsUsable() == genInfo.m_iSavePathUseCount)
				{
					genInfo.m_iState = eRegularEnd;
				}
				break;
			}
		case eRegularEnd:
			{
				break;
			}
		}
	}
	m_nFishCount += fishList.size();
}

bool QcDefaultSceneRegularMaker::OnDead(const QcFish* pFish)
{
	QiSceneRegularMaker::OnDead(pFish);
	if (m_nFishCount < 10)
	{
		int iCount = m_generateInfo.size();
		int i=0;
		for (i=0; i<iCount; ++i)
		{
			if (m_generateInfo[i].m_iState != eRegularWaitEnd
				&& m_generateInfo[i].m_iState != eRegularEnd)
			{
				break;
			}
		}
		m_bFinished = (i==iCount);
		return m_bFinished && m_nFishCount == 0;
	}
	return false;
}