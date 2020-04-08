#include "Pch.hpp"
#include "QcSceneRegularMaker.hpp"

QcSceneRegularMaker::QcSceneRegularMaker();

bool QcSceneRegularMaker::Animate(double dCurrentTime,f32 fElapsedTime, QcList<QcFish*>& fishList)
{
	return false;
}

void QcSceneRegularMaker::BeginSceneRegular(QcList<QcFish*>& fishList, const QcObjType& objType)
{
	QcResManager* pResMgr = QmGlobalData.GetSceneResMgr();

	const QcList<QsSceneRegularInfor>& sceneRegularList = pResMgr->GetFishRegularMgr()->GetSceneRegularList();
	UInt32 index = 0;
	if (objType == ID_NULL)
	{
		index = RandomInt(0, (UInt16)sceneRegularList.size());
	}
	else
	{
		QcList<QsSceneRegularInfor>::const_iterator iter = std::binary_searchEx(sceneRegularList.begin(), sceneRegularList.end(), objType);
		if (iter == sceneRegularList.end())
			index = RandomInt(0,  (UInt16)sceneRegularList.size());
		else
			index = iter - sceneRegularList.begin();
	}
	
	const QcList<QcResFishRegularDesc*>& fishRegularList = sceneRegularList[index];
}