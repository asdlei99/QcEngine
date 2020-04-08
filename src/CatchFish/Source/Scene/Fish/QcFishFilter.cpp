#include "pch.hpp"
#include "QcFish.hpp"
#include "QcFishFilter.hpp"
#include "Res\QcResFishDesc.hpp"

bool QcBombFishFilter::filter(QcFish* pFish)
{
	return pFish->IsBombFish() || pFish->IsChainLinkFish() || pFish->IsDelayCalScore();
}

bool QcSameKindBombFishFilter::filter(QcFish* pFish) 
{
	return m_iFishType != pFish->GetFishType();
}

bool QcDelayBombFishFilter::filter(QcFish* pFish)
{
	if (!pFish->IsBombFish() && !pFish->IsChainLinkFish() && !pFish->IsDelayCalScore())
	{
		if (pFish->GetExplodeScore() <= m_iExplodeScore)
        {
            m_iExplodeScore -= pFish->GetExplodeScore();
            return false;
        }
	}
	return true;
}

bool DcChainLinkFishFilter::filter( QcFish* pFish )
{
    return (pFish->IsDelayCalScore() || pFish->GetScore()<m_iMin || pFish->GetScore()>m_iMax);
}
