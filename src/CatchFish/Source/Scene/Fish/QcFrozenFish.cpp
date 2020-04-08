#include "pch.hpp"
#include "QcFrozenFish.hpp"
#include "QcFishMgr.hpp"

ImplRuntimeClass(QcFrozenFish);

QcFrozenFish::QcFrozenFish()
{

}

bool QcFrozenFish::ToDie(bool bDieTimer)
{
	QmSceneMgr->GetFishMgr()->BeginFrozen(QmSysSetting->GetFrozenTime());
	return QcFish::ToDie(bDieTimer);
}