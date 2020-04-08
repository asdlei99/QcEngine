#pragma once

#include "QcFish.hpp"

class QcJumpScoreFishAnimation;
class DcJumpScoreFish : public QcFish
{
    DeclareRuntimeClass(DcJumpScoreFish);

public:
    DcJumpScoreFish();
	~DcJumpScoreFish();

    virtual void Animate(double dCurrentTime,f32 fElapsedTime);
    virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix);

    virtual void OnTimer(u32 nTimer, void* pContext);

    virtual void HitToDie(QcBullet* pBullet);
    virtual void ExploseToDie(QcFish* pFish);

    virtual void ToDead();

    virtual bool IsDelayCalScore() const {return true;}
	void FinishAnimatorCallBack(u32 pPara);
private:
    void MakeScoreZoomAnimate();

protected:
    f32                      m_fJumpingScore;
    QcSceneDigitalPtr        m_pJumpingScore;
    bool                     m_bJumping;
	PyroParticles::IPyroParticleEmitter*		m_pMyParticle;
	//QcJumpScoreFishAnimation*  m_pJumpFishScoreBgAnimation;
	QcSharedPtr<QcJumpScoreFishAnimation> m_pJumpFishScoreBgAnimation;
};