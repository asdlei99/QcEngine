#pragma once

#include "Res\QcResFishDesc.hpp"
struct QsFishPool
{
	QsFishPool(const QcObjType& objType, const QcObjType& objFlag)
	{
		m_pFishDesc = QmResMgr->GetImageDesc(objType)->As<QcResFishDesc*>();
		//m_pFishFlag = QmResMgr->GetImageDesc(objFlag)->As<QcResImageDesc*>();
		m_bInUsed = false;
	}
	QcResFishDesc* m_pFishDesc;
	//QcResImageDesc* m_pFishFlag;
	bool m_bInUsed;
};

class QcSameKindFishHelper
{
	friend class QcSameKindBombFish;
	void Load()
	{
		if (m_poolList.size() == 0)
		{
			QcObjType objList[][2] = 
			{
				{QmObjName(fish01), QmObjName(SameKindFishFlag_00)},
				{QmObjName(fish02), QmObjName(SameKindFishFlag_01)},
				{QmObjName(fish03), QmObjName(SameKindFishFlag_02)},
				{QmObjName(fish04), QmObjName(SameKindFishFlag_03)},
				{QmObjName(fish05), QmObjName(SameKindFishFlag_04)},
				{QmObjName(fish06), QmObjName(SameKindFishFlag_05)},
				{QmObjName(fish07), QmObjName(SameKindFishFlag_06)},
				{QmObjName(fish08), QmObjName(SameKindFishFlag_07)},
				{QmObjName(fish09), QmObjName(SameKindFishFlag_08)},
				/*{QmObjName(fish10), QmObjName(SameKindFishFlag_09)},*/
			};
			QcObjType* p = &(objList[0][0]);
			for (int i=0; i<sizeof(objList)/sizeof(QcObjType); i += 2)
			{
				m_poolList.push_back(QsFishPool(p[i], p[i+1]));
			}
		}
	}
	QsFishPool* FindPtr(const QcObjType& objType)
	{
		std::vector<QsFishPool>::iterator iter(m_poolList.begin());
		std::vector<QsFishPool>::iterator endIter(m_poolList.end());
		for (; iter != endIter; ++iter)
		{
			if (objType == iter->m_pFishDesc->GetObjType())
				return &(*iter);
		}
		return NULL;
	}
	QsFishPool* FindPtr(const QcResFishDesc* pFishDesc)
	{
		std::vector<QsFishPool>::iterator iter(m_poolList.begin());
		std::vector<QsFishPool>::iterator endIter(m_poolList.end());
		for (; iter != endIter; ++iter)
		{
			if (pFishDesc == iter->m_pFishDesc)
				return &(*iter);
		}
		return NULL;
	}
public:
	QcSameKindFishHelper() 
		: m_iCurrentCount(0)
		, m_iMinCount(0)
		, m_iMaxCount(0)
		, m_iMinTime(0)
		, m_iMaxTime(0)
		, m_iDestTime(0)
		, m_iMinDuringTime(0)
		, m_iMaxDuringTime(0)
		, m_fRandom(1.0f)
	{}

	void Reset(u32 nMinCount, u32 nMaxCount, u32 nMinTime, u32 nMaxTime, u32 MinDuringTime, u32 MaxDuringTime, f32 fRandom)
	{
		m_iMinCount = nMinCount;
		m_iMaxCount = nMaxCount;
		m_iMinTime = nMinTime;
		m_iMaxTime = nMaxTime;
		m_iMinDuringTime = MinDuringTime;
		m_iMaxDuringTime = MaxDuringTime;
		m_fRandom = fRandom;
		m_iDestTime = GetTickCount() + RandomInt(0, MinDuringTime);
	}
	QsFishPool* Find(const QcObjType& objType)
	{
		Load();
		return FindPtr(objType);
	}
	QsFishPool* Find(const QcResFishDesc* pFishDesc)
	{
		Load();
		return FindPtr(pFishDesc);
	}
	QsFishPool* FindUsableKind()
	{
		Load();
		int iCount = m_poolList.size();
		int iBegin = RandomInt(5, iCount);
		int iSave = iBegin;
		for (; iBegin<iCount; ++iBegin)
		{
			if (!m_poolList[iBegin].m_bInUsed)
			{
				return &m_poolList[iBegin]; 
			}
		}
		iBegin = 0;
		for (; iBegin < iSave; ++iBegin)
		{
			if (!m_poolList[iBegin].m_bInUsed)
			{
				return &m_poolList[iBegin]; 
			}
		}
		assert(false);
		return &m_poolList[iBegin];
	}
	bool CanGenSameKindFish(const QcResFishDesc* pFishDesc)
	{
		//总次数，以及总量达到
		if (m_iCurrentCount >= m_iMaxCount|| m_iMaxTime <= 0)
			return false;
		//已存在同类型的
		QsFishPool* pFishInfo = Find(pFishDesc);
		if (pFishInfo == NULL || pFishInfo->m_bInUsed)
			return false;
		//填充最小值
		if (m_iCurrentCount < m_iMinCount)
			return true;

		DWORD thisTime = GetTickCount();
		if (m_iMinDuringTime > 0)
		{
			if (thisTime > m_iDestTime)
			{
				m_iDestTime = thisTime + RandomInt(m_iMinDuringTime, m_iMaxDuringTime);
				return true;
			}
			return false;
		}
		return Randomf() < m_fRandom;
	}
	void Increase()
	{
		--m_iMinTime;
		--m_iMaxTime;
		++m_iCurrentCount;
	}
	void Decrease()
	{
		--m_iCurrentCount;
	}
protected:
	std::vector<QsFishPool> m_poolList;
	int m_iCurrentCount;
	int m_iMinCount;
	int m_iMaxCount;

	int m_iMinTime;
	int m_iMaxTime;
	u32 m_iMinDuringTime; //间隔时间
	u32 m_iMaxDuringTime; //间隔时间
	u32 m_iDestTime;
	f32 m_fRandom;
};