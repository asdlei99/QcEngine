#pragma once

#include "Scene\QcSceneAnimationDeque.hpp"
#include "Thread\QcThread.hpp"
#include "Res\QiResCallBack.hpp"
class QcSceneStartMgr : public QcThread, public QiResCallBack
{
public:
	explicit QcSceneStartMgr(QcResManager* pResMgr);

	bool Init();
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render);
	bool IsFinished() {return m_bIsFinished && m_arAnimationList.Size() == 0;}
private:
	bool LoadProgressResource(QcResManager* pResManager);
	bool LoadSceneResource(QcResManager* pResManager);
	void CallBack(const QcResDesc* pDesc);
	virtual int Run();
	virtual bool OnIdle(u32 /*nIdle*/){return false;}
private:
	bool m_bIsFinished;
	QcResManager* m_pResMgr;
	QcSceneAnimationDeque m_arAnimationList;
	QcSceneAnimationPtr m_pLastAnimation;
};
