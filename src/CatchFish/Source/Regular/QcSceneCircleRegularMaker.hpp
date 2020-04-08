#pragma once

#include "Regular/QiSceneRegularMaker.hpp"
class QcResSceneRegularDesc;
class QcRegularMgr;

class QcSceneCircleRegularMaker : public QiSceneRegularMaker
{
	DeclareRuntimeClass(QcSceneCircleRegularMaker);
public:
	QcSceneCircleRegularMaker(){}
	virtual void Animate(double dCurrentTime,f32 fElapsedTime, QcList<QcFish*>& fishList);
	virtual void BeginSceneRegular(QcRegularMgr* pRegularMgr, QcList<QcFish*>& fishList, QcResSceneRegularDesc* pSceneRegularDesc);
	virtual void EndSceneRegular(){}
};
