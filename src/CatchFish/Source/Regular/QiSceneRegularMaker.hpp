#pragma once

class QcResSceneRegularDesc;
class QcRegularMgr;
class QcFish;
class QiSceneRegularMaker : public QcObject
{
public:
	QiSceneRegularMaker() : m_bFinished(false){}
	virtual void Animate(double dCurrentTime,f32 fElapsedTime, QcList<QcFish*>& fishList) = 0;
	virtual void BeginSceneRegular(QcRegularMgr* pRegularMgr, QcList<QcFish*>& fishList, QcResSceneRegularDesc* pSceneRegularDesc) = 0;
	virtual void EndSceneRegular(){ m_bFinished = true;}
	virtual bool OnDead(const QcFish* pFish){ if (m_nFishCount > 0) --m_nFishCount; return m_nFishCount==0;}

	const QcObjType& GetObjType() const {return m_vObjType;}
	void SetObjType(const QcObjType& objType){ m_vObjType = objType;}
	bool IsFinished() {return m_bFinished;}
protected:
	bool m_bFinished;
	f32 m_fLivingTime;
	u32 m_nFishCount;
	QcObjType m_vObjType;
};