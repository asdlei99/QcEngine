#pragma once

class QcSceneRegularMaker : public QcObject
{
public:
	QcSceneRegularMaker();

	bool Animate(double dCurrentTime,f32 fElapsedTime, QcList<QcFish*>& fishList);
	void MakeFishRegular(QcList<QcFish*>& fishList, const QcObjType& objType = ID_NULL);
	void BeginSceneRegular(QcList<QcFish*>& fishList, const QcObjType& objType = ID_NULL);
	void EndSceneRegular();
protected:
	QiSceneRegularMaker* m_pSceneRegularMaker;
};