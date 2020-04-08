#pragma once

#include "Scene\Fish\QcSameKindFishHelper.hpp"
#include "Common\QcEncryptXmlLoader.hpp"

class QcFish;
class QcFishMgr;

#define QmBottomRandom (0.0f)
#define QmGapWidth (15.f)
#define QmGroupSpeed (250.f)
class QcFishControlInfo : public QcObjectEx
{
public:
	QcFishControlInfo();
	~QcFishControlInfo();

	struct QsFishKind;
	struct QsFishInfor : public QcObject
	{
		QsFishInfor() 
			: m_pFishDesc(NULL)
			, m_ePathType(0)
			, m_eScenePathType(0)
			, m_nMin(0)
			, m_nMax(0)
			, m_fRandom(0.f)
			, m_iDestTime(0)
			, m_iMinDuringTime(0)
			, m_iMaxDuringTime(0)
			, m_iCurrentNum(0)
			, m_iAdded(0)
			, m_iTimer(0)
			, m_pParentKind(NULL)
			, m_MaxTime(0xffffffff)
		{}
		virtual ~QsFishInfor(){}
		virtual bool IsKind() {return false;}
		virtual void Reset()
		{
			m_iCurrentNum = 0;
			m_iAdded = 0;
			m_iTimer = 0;
			m_iDestTime = GetTickCount() + RandomInt(0, m_iMinDuringTime);
		}
		virtual bool FishGenerate(QcList<QcFish*>& fishList);
		inline void AddFish(u32 uNum)
		{
			m_iCurrentNum += uNum;
			m_iAdded += uNum;
			m_iTimer += uNum;
			if (m_pParentKind)
			{
				m_pParentKind->AddFish(uNum);
			}
		}

		//当前的鱼数少于最小值时，立即出鱼
		inline bool RemoveFish(u32 uNum)
		{
			if (m_iCurrentNum >= uNum)
				m_iCurrentNum -= uNum;
			else
				m_iCurrentNum = 0;
			if (m_pParentKind)
			{
				return m_pParentKind->RemoveFish(uNum) || (m_iCurrentNum < m_nMin);
			}
			return m_iCurrentNum < m_nMin;
		}
		inline bool IsLessWarning()
		{
			return m_iCurrentNum < m_nMin;
		}
		inline bool CanFillFish(bool bFillMin = false)
		{
			if (m_iCurrentNum < m_nMax)
			{
				if (bFillMin)
					return true;

				if (m_iTimer >= m_MaxTime)
					return false;

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
			return false;
		}
		
		s32 m_ePathType;
		s32 m_eScenePathType;
		u32 m_nMin;			//鱼的范围
		u32 m_nMax;
		f32 m_fRandom;			//概率
		u32 m_iDestTime;		//目标时间
		u32 m_iMinDuringTime; //间隔时间
		u32 m_iMaxDuringTime; //间隔时间
		u32 m_iCurrentNum;	//当前这种鱼的数量
		u32 m_iAdded;		//待出的数量
		u32 m_iTimer;		//出的次数
		u32 m_MinTime;		//最少次数
		u32 m_MaxTime;		//最多运行出的次数

		QsFishKind* m_pParentKind;
		QcResFishDesc* m_pFishDesc;
		QcFishControlInfo* m_pControlInfo;
	};
	struct QsFishKind : public QsFishInfor
	{
		virtual ~QsFishKind()
		{
			QcList<QsFishInfor*>::iterator iter(m_objList.begin());
			for (; iter != m_objList.end(); ++iter)
			{
				delete (*iter);
			}
			m_objList.Clear();
		}
		virtual bool IsKind() {return true;}
		virtual void Reset()
		{
			QsFishInfor::Reset();
			QcList<QsFishInfor*>::iterator iter(m_objList.begin());
			QcList<QsFishInfor*>::iterator endIter(m_objList.end());
			for (; iter != endIter; ++iter)
			{
				(*iter)->Reset();
			}
		}
		virtual bool FishGenerate(QcList<QcFish*>& fishList)
		{
			bool bRet = false;
			m_iAdded = 0;
			QcList<QsFishInfor*>::iterator iter(m_objList.begin());
			QcList<QsFishInfor*>::iterator endIter(m_objList.end());
			for (; iter != endIter; ++iter)
			{
				bRet = bRet | (*iter)->FishGenerate(fishList);
			}
			if (bRet)
				m_iDestTime = GetTickCount() + RandomInt(m_iMinDuringTime, m_iMaxDuringTime);
			return bRet;
		}

		QsFishInfor* FindFishInfo(int& index)
		{
			if (m_objList.size() == 0)
				return NULL;
			index = RandomInt(0, m_objList.size());
			return m_objList[index];
		}
		QcList<QsFishInfor*> m_objList;
	};

	void Init(QcFishMgr* pFishMgr, const char* psConfig);

	virtual void OnEvent(const QsEvent& evt);
	virtual void Parse(const QcXmlNodeIterator& topNode, void* pParam);

	void LoadFishKind(const QcXmlNodeIterator& kindFishNode, QsFishKind* pParent);
	void LoadFishInfo(const QcXmlNodeIterator& fishInfoNode, QsFishKind* pParent);
	void Reset(bool);
	f32 GetFishFillTime() const {return m_fFishFillTime;}
	void LockFishFill(bool bLock);
	void OnDead(const QcFish* pFish);
	void MakeQueueFish(QcList<QcFish*>& fishList);
	void MakeGroupFish(QcList<QcFish*>& fishList);
	void MakeGroup2Fish(QcList<QcFish*>& fishList);
	void MakeCirclePattern(QcList<QcFish*>& fishList, u32 nCount = 1, f32 fGapWidth = 10.f);
	void FillExtraFish(QcList<QcFish*>& fishList);

	bool CanGenSameKindFish(const QcResFishDesc* pFishDesc);
	void ToDeadGenSameKindFish(const QcResFishDesc* pFishDesc);
	QcSameKindBombFish* MakeSameKindBombFish(const QcResFishDesc* pFishDesc);
protected:
	s32 AddRandomSameKindFish(const QcResFishDesc* pFishDesc,s32 iCount);
	void MakeFishQueue(QcList<QcFish*>& fishList, const QcResPathDesc* pPathDesc, QcResFishDesc* pFishDesc, u32 nCount, u32 eFishFlag);
	u32 FillFish(u32 nMaxGenerate = 10);
	void CalculateRandom(QsFishKind* pFishKind);
	//计算鱼
	u32 CalculateFillMin(QsFishKind* pFishKind = NULL);
	//返回实际产生的数量
	u32 CalculateFillSecond(QsFishKind* pFishKind, u32 nNum);
	void FishGenerate(QcList<QcFish*>& fishList);
	QsFishInfor* FindFishKind(const QcObjType& objType);
	QsFishKind* GetTopKind(QsFishInfor* pFishInfo)
	{
		while (pFishInfo->m_pParentKind)
			pFishInfo = pFishInfo->m_pParentKind;
		return (QsFishKind*)pFishInfo;
	}
private:
	typedef std::map<QcObjType, QsFishInfor*, QcNoCaseCompare> QcFishInforMap;
	QcFishMgr* m_pFishMgr;
	QcFishInforMap m_arFishInfoList;
	QcList<QsFishKind*> m_arKindList;
	QsFishKind* m_pLargeKind;

	QcSharedPtr<QsFishInfor> m_pSameKindFishInfo;
	QcSameKindFishHelper m_oSameKindFishHelper;

	bool m_bLockFishFill;
	bool m_bFillMinFish;
	bool m_bForceFishFilled; 
	f32 m_fFishFillTime;
};