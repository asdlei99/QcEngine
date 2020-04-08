#pragma once

class QcResFishRegularDesc;
class QiSceneRegularMaker;
class QcResSceneRegularDesc;
#include "Common\QcEncryptXmlLoader.hpp"
class QcRegularMgr : public QcEncryptXmlLoader
{
public:
	QcRegularMgr();
	
	QiSceneRegularMaker* BeginSceneRegular(QcList<QcFish*>& fishList, const QcObjType& objType = ID_NULL);

	QcResFishRegularDesc* FindNormalFishRegular(f32 fRandom);
	QcResFishRegularDesc* FindFishRegularDesc(const QcObjType& objType);
	QcResSceneRegularDesc* FindSceneRegularDesc(const QcObjType& objType);

	void MakeFishRegular(QcList<QcFish*>& fishList, const QcObjType& objType = ID_NULL, f32 fSpeed= 165.f, u32 nCount = 1, f32 fGapWidth = 10.f);
	void MakeFishRegular(QcList<QcFish*>& fishList, const QcResFishRegularDesc* pFish, const QcResPathDesc* pPathDesc, f32 fSpeed= 165.f, u32 nCount = 1, f32 fGapWidth = 10.f);
	void MakeSpreadOutRegular(const QcObjType& mainFish, const QcObjType* pSubFish, u32 nCount, const QcPoint& startPos, const QcPoint& endPos, f32 fSpeed);
protected:
	virtual void Parse(const QcXmlNodeIterator& pRootNode, void* pParam);
protected:
	QcSharedPointerList<QcResFishRegularDesc> m_arFishRegularList;
	QcSharedPointerList<QcResSceneRegularDesc> m_arSceneRegularList;
};