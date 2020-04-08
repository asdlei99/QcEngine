#pragma once

#include "Regular\QiSceneRegularMaker.hpp"

class QcRegularMgr;
class QcResSceneRegularDesc;
class QcDefaultSceneRegularMaker : public QiSceneRegularMaker
{
	DeclareRuntimeClass(QcDefaultSceneRegularMaker);
public:
	enum
	{
		eRegularWaitStart,
		eRegularContinue,
		eRegularWaitEnd,
		eRegularEnd,
	};
	struct QsGenerateInfo
	{
		int m_iState;
		f32 m_fTimeCount;
		f32 m_fStartTimeCount;
		f32 m_fLivingTimeCount;
		QcSharedPtr<QcResPathDesc> m_pWaitPathDesc;
	};
	QcDefaultSceneRegularMaker();

	virtual void Animate(double dCurrentTime,f32 fElapsedTime, QcList<QcFish*>& fishList);
	virtual void BeginSceneRegular(QcRegularMgr* pRegularMgr, QcList<QcFish*>& fishList, QcResSceneRegularDesc* pSceneRegularDesc);
	virtual bool OnDead(const QcFish* pFish);
protected:
	void Generate(QcList<QcFish*>& fishList, f32 fElapsedTime);
protected:
	f32 m_iEmptyTime;
	QcRegularMgr* m_pRegularMgr;
	QcResSceneRegularDesc* m_pSceneRegularDesc;
	std::vector<QsGenerateInfo> m_generateInfo;
};
