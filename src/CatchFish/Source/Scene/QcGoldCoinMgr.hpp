#pragma once

#include "Scene\QcSceneAnimationMgr.hpp"
class QcGoldCoinMgr : public QcSceneAnimationMgr
{
public:
	QcGoldCoinMgr();

	void Init(QcResManager* pResMgr);
	void MakeCoin(u32 iScore, const QcPoint& startPt, const QcPoint& endPt, f32 fRotation);
protected:
	bool CalculateCoin(u32 iScore, u32& iGoldCoin, u32& iSilverCoin)
	{
		if(iScore <= 6)
		{
			iSilverCoin = iScore;
			return false;
		}
		iGoldCoin = QcMath::clamp<u32>(iScore/ 5, 2, 8);
		return true;
	}
protected:	
	QcResImageDesc* m_pGoldDesc;
	QcResImageDesc* m_pSilverDesc;
	f32 m_fCoinSpeed;
};