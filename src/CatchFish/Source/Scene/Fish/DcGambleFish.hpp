#pragma once

#include "QcFish.hpp"

const QcObjType QcGambleCVList[] = 
{
	QmObjName(GambleFishPlayerWin03),
	QmObjName(GambleFishPlayerWin04),
	QmObjName(GambleFishPlayerWin06),
	QmObjName(GambleFishPlayerWin07),
	QmObjName(GambleFishPlayerWin08),
	QmObjName(GambleFishPlayerWin09),
	QmObjName(GambleFishPlayerWin10),
	QmObjName(GambleFishPlayerWin11),
    QmObjName(GambleFishPlayerWin12),
    QmObjName(GambleFishPlayerWin01),
    QmObjName(GambleFishPlayerWin02),
    QmObjName(GambleFishPlayerWin05),
};

class DcGambleFish : public QcFish
{
    DeclareRuntimeClass(DcGambleFish);

public:
    DcGambleFish();
    ~DcGambleFish(){}

    virtual void Animate(double dCurrentTime,f32 fElapsedTime);
    virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix);

    virtual void HitToDie(QcBullet* pBullet);
    virtual bool IsDelayCalScore() const{ return true; }

    virtual void OnTimer(u32 nTimer, void* pContext);
    virtual void OnEvent(const QsEvent& evt);

    void AnimationFinishCallBack(u32 iParam);

private:
    void ExitGamble();
    void GambleSuccess();
    void GambleFail();

private:
    QcDigitalNode       m_oScoreDigit;
    QcDigitalNode       m_oScoreShadowDigit;
    QcDigitalNode       m_oTimesDigit;

    QcSceneNodePtr      m_pScaleTips;

    QcSceneNodePtr      m_pScoreBg;
    QcSceneNodePtr      m_pResult;

    QcSceneNodePtr      m_pDice;
    QcSceneNodePtr      m_pSelectUi[3];

    QcSceneNodePtr      m_pCounter;
    QcSceneNodePtr      m_pOver;

    u32              m_iSelectId;
    f32                 m_fCurScore;
    u32              m_iWinTimes;

    bool                m_bAcceptControl;
    f32                 m_fScoreStep;
	QcSound*     m_pBgm;
	QcSound*	   m_pCurrentBgm;
};