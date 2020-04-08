#pragma once

#include "QcFish.hpp"

class DcChainLinkFish : public QcFish
{
    DeclareRuntimeClass(DcChainLinkFish);

public:
    DcChainLinkFish();
    ~DcChainLinkFish();

    virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode);
    virtual void Animate(double dCurrentTime,f32 fElapsedTime);
    virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix);

    virtual void HitToDie(QcBullet* pBullet);
    virtual bool IsDelayCalScore() const { return true; }

    virtual bool IsChainLinkFish() const { return true; }

    virtual void OnEvent(const QsEvent& evt);
    virtual void OnTimer(u32 nTimer, void* pContext);

    virtual u32 GetExplodeScore() const;
    

    //static bool CanGenChainLinkFish() { return g_iChainLinkCount == 0; }
    //static DcChainLinkFish* MakeChainLinkFish(const QcObjType& objType);
    //static DcChainLinkFish* MakeChainLinkFish(const QcResFishDesc* pFishDesc);
    //static void ResetChainLinkCount() { g_iChainLinkCount = 0; }
	QcSceneNode* GetMainFish(){return m_pFish;}
private:

    void AnimatorFinishCallBack(u32 iParam);

    void FindFishForDrag();
    void MakeStartDragAni();

    QcSceneNodePtr          m_pFish;
    QcSceneNodePtr          m_pChainShell;
    QcSceneNodePtr          m_pChainRope;
    QcSceneNodePtr          m_pChainEff;
    QcSceneNodePtr          m_pChainIdBg;

    QcSceneAnimationPtr     m_pChainLogo;
    QcSceneDigitalPtr       m_pChainScaleDigit;
    QcSceneDigitalPtr       m_pChainScaleDigitShadow;
    QcDigitalNode           m_oChainFishIdDigital;

    int                     m_iChainScore;
    int                     m_iCurScore;
    
    QcSceneNodePointerList<DcChainLinkFish>              m_arChainList;


    //static int              g_iChainLinkCount;
};