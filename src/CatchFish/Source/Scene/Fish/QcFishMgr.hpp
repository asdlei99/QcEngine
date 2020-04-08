#pragma once

#include "Scene\Fish\QcFishControlInfo.hpp"
#include "Scene\QcSceneNodePointerList.hpp"

class QcFishFilter;
class QcRegularMgr;
class QiSceneRegularMaker;
class QcFish;
class QcFishMgr : public QcObjectEx
{
public:
	QcFishMgr();
	~QcFishMgr();

	void Init(QcResManager* pResMgr, const char* psConfig);

	void Animate(double dCurrentTime,f32 fElapsedTime);
	bool HandleCollision(QcBulletPtrList& bulletList);
	void Render(QcDeviceRender& render, u32 iLayer);

	void Reset(bool bFishControlEnable);
	void OnDead(QcFish* pFish);
	void RemoveFish(const QcFish* pFish);
	void AddFish(QcList<QcFish*>& fishList);
    void HoistFishToTop(const QcFish *pFish);
	u32 SearchHitableFish(QcList<QcFish*>& fishList, const QcPoint& pos, f32 fRadiu, QcFishFilter* excluder = NULL);
	u32 SearchHitableFish(QcList<QcFish*>& fishList, const QcRegion* pRegion, QcFishFilter* excluder = NULL);
	u32 SearchHitableFish(QcList<QcFish*>& fishList, const QcRect& rect, QcFishFilter* excluder = NULL);
	QcFish* FindTargetFish();
	QcRegularMgr* GetSceneRegularMaker() { return m_pSceneRegularMaker;}
	s32 GetFishCount() {return m_nFishCount; }
	bool IsFrozen() const {return m_bInFrozen;}
	void BeginFrozen(f32 fTime);

	bool CanGenSameKindFish(const QcResFishDesc* pFishDesc);
	void ToDeadGenSameKindFish(const QcResFishDesc* pFishDesc);
	QcSameKindBombFish* MakeSameKindBombFish(const QcResFishDesc* pFishDesc);
	
	//队列
	const QcObjType& BeginSceneRegular(const QcObjType& objType = ID_NULL);			//过场队列
	//每隔一段时间检测鱼的数量。
	virtual void OnTimer(u32 nTimer, void* pContext);
protected:
	void Animate(QcFishList& fishList, double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render, QcFishList& fishList, bool bLiving, u32 iLayer); 
	void EndFrozen();
	void Frozen(bool bFrozen);
	QcFish* FindTargetFish(QcList<QcFish*>& fishList);
	QcFishList& GetFishList(u32 eLayer) {return m_arFishList[eLayer];}
protected:
	bool m_bInFrozen;
	bool m_bSceneNormal;		//过∑诩洌憧刂破鞑黄鹱饔?

	QcRegularMgr* m_pSceneRegularMaker;
	QcSharedPtr<QiSceneRegularMaker> m_pCurrentSceneRegularMaker;
	f32 m_fRegularTime;

	s32 m_nFishCount;
	QcFishControlInfo m_oControlInfo;
	QcFishList m_arFishList[QmCountOf(QaFishList)];
	QcFishList m_arBottomFish;
    QcFishList m_arTopFish;
	std::vector<QcFish*> m_arHitableFishList;
};