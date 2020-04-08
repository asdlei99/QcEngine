#pragma once

class QcFishFilter
{
public:
	virtual bool filter(QcFish* pFish)  = 0;
};

class QcBombFishFilter : public QcFishFilter
{
public:
	virtual bool filter(QcFish* pFish);
};

class QcSameKindBombFishFilter : public QcFishFilter
{
	QmDisableCopyAssignment(QcSameKindBombFishFilter);
public:
	QcSameKindBombFishFilter(u32 fishType) : m_iFishType(fishType){}

	virtual bool filter(QcFish* pFish);
protected:
	u32   m_iFishType;
};

class QcDelayBombFishFilter : public QcFishFilter
{
	QmDisableCopyAssignment(QcDelayBombFishFilter);
public:
	QcDelayBombFishFilter(u32 iExplodeScore) : m_iExplodeScore(iExplodeScore){}
	virtual bool filter(QcFish* pFish);
protected:
	u32 m_iExplodeScore;
};

class DcChainLinkFishFilter : public QcFishFilter
{
    QmDisableCopyAssignment(DcChainLinkFishFilter);

public:
    DcChainLinkFishFilter(u32 iMinScore, u32 iMaxScore) : m_iMax(iMaxScore), m_iMin(iMinScore){}
    virtual bool filter(QcFish* pFish);

protected:
    u32     m_iMin;
    u32     m_iMax;
};
