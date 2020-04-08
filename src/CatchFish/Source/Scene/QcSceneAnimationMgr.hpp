#pragma once
#include "Scene\QcSceneNodePointerList.hpp"

class MP_Emitter;
typedef std::vector<MP_Emitter*> MP_EmitterList;
typedef QcSharedPointerList<QcEffect> QcEffectList;

#define QmAnimationLayerNum (eAnimationLayerEnd-eAnimationLayerBegin)
class QcSceneAnimationMgr : public QcObject
{
	QmDisableCopyAssignment(QcSceneAnimationMgr);
public:
	QcSceneAnimationMgr(const QcPoint& vParticleOffsetPos);
	~QcSceneAnimationMgr();

	void Init(QcResManager* pResMgr);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render, QeLayer eLayer, const QcMatrix* pWorldMatrix = NULL);
	void Clear()
	{
		for (u32 iLayer = eAnimationLayerBegin; iLayer < eAnimationLayerEnd; ++iLayer)
		{
			QcSceneAnimationPtrList& animationList = GetAnimationList(iLayer);
			animationList.Clear();
		}
	}

	void MakeCoin(u32 iScale, u32 iLevel, const QcPoint& startPt, const QcPoint& endPt, f32 fRotation, f32 fStartTime = 0.f);

	void AddAnimation(const QcSceneAnimation* pAnimation, QeLayer eLayer = eAnimationLayerNormal, bool bIgnoreSame = false)
	{
        QcSceneAnimationPtrList &TheList = GetAnimationList(eLayer);

        if (bIgnoreSame && TheList.Find(pAnimation)!=-1)
            return;

		TheList.Insert(pAnimation);
	}

	PyroParticles::IPyroParticleEmitter* AddParticle(QeLayer eLayer, const char* psName, const QcPoint& pos, float MaxDelta = 1.f, f32 fScale = 1.f, f32 fAngle = 0.f);
	MP_Emitter* AddParticleMP(QeLayer eLayer, const char* psName, const QcPoint& pos, float MaxDelta = 1.f, f32 fScale = 1.f, f32 fAngle = 0.f);
	QcEffect* AddEffect(QeLayer eLayer, const char* psName);

#ifdef QmOldParticle
	CEmitterList& GetOldEmitter(u32 eLayer)
	{
		return m_arOldEmitterList[eLayer-eAnimationLayerBegin];
	}
protected:
	CEmitterList m_arOldEmitterList[QmAnimationLayerNum];
	IPyroFile               *m_pPyroFile;
#endif
#ifdef QmMPParticle
	MP_EmitterList& GetEmitter(u32 eLayer)
	{
		return m_arEmitterList[eLayer-eAnimationLayerBegin];
	}
	MP_EmitterList m_arEmitterList[QmAnimationLayerNum];
	MP_Manager* m_pMpMgr;
#endif

protected:
	QcSceneAnimationPtrList& GetAnimationList(u32 eLayer)
	{
		return m_arAnimationList[eLayer-eAnimationLayerBegin];
	}
	QcEffectList& GetEffectList(u32 eLayer)
	{
		return m_arEffectList[eLayer-eAnimationLayerBegin];
	}
protected:
	QcSceneAnimationPtrList m_arAnimationList[QmAnimationLayerNum];
	QcEffectList m_arEffectList[QmAnimationLayerNum];

	const QcPoint m_vParticleOffsetPos;
	f32 m_sBubbleTimeCount;
};

#define QmAnimationMgr QmSceneMgr->GetAnimationMgr()