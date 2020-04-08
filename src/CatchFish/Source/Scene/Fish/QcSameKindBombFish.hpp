#pragma once

#include "Scene\Fish\QcBombFish.hpp"
#include "Scene\QcBullet.hpp"
#include "Scene\QcPlayer.hpp"
#include "Animator\QcAnimatorMoveLine.hpp"

struct QsFishPool;
class QcSameKindBombFish : public QcBombFish
{
	typedef struct QsSameKindFishInfo
	{
		QcFishPtr m_pFish;
		QcSceneAnimationPtr m_pIceNode;
		QcSceneAnimationPtr m_pScore;
	};
	DeclareRuntimeClass(QcSameKindBombFish);
public:
	QcSameKindBombFish(QcResFishDesc* pFishDesc = NULL);
	~QcSameKindBombFish();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	static QcSameKindBombFish* MakeSameKindBombFish(const QcResFishDesc* pFishDesc);
	static bool CanGenSameKindFish(const QcResFishDesc* pFishDesc);

	virtual void ToDead();
	virtual void HitByBullet(QcBullet* pBullet);
	virtual void HitToDie(QcBullet* pBullet);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	virtual void OnTimer(u32 nTimer, void* pContext);
	virtual bool IsDelayCalScore() const {return true;}

    const QcObjType& GetFishKind() const;
protected:
	void MakeTornadoAnimation();
	void Drag();
	void MakeIce(u32 idx);
	void FinishAnimator(u32 iParam);    //回调函数 用于判断动画是否完成
	void Explose();
protected:
	QcResFishDesc* m_pFishInfo;
	u32 m_iExplodeScore;
	QcSceneAnimationPtr m_pXuanFengAnimation;
	//MP_Emitter* m_pXuanFengEffect;
	QcList<QsSameKindFishInfo> m_arSameKindFishInfoList;
	u32 m_iFinishedCount;
	QcPoint m_vDestPos;
    QcSceneNodePtr m_pRing;
};