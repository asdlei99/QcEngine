#pragma once

#include "Animator\QiAnimator.hpp"

typedef QcAnimatorCb QcTextureCb;
class QcAnimatorTexture : public QiAnimator
{
	struct QsTextureCbInfo
	{
		QsTextureCbInfo(const QcTextureCb& cb, u32 iFrame, u32 iParam) : m_cb(cb), m_iParam(iParam), m_iFrame(iFrame) {}

		QcTextureCb m_cb;
		u32 m_iFrame;
		u32 m_iParam;
	};
	DeclareRuntimeClass(QcAnimatorTexture);
public:
	explicit QcAnimatorTexture() {}
	virtual bool IsTextureAnimator() const {return true;}

	void Reset(const QcTexturePtrList& textures, f32 timePerFrame, bool loop = true)
	{
		QiAnimator::Reset();
		m_arTextureList = textures;
		m_fTimePerFrame = timePerFrame;
		m_bLoop = loop;
		m_fTimeCount = 0.f;
		m_idx = 0;
	}
	void Reset(const QsStateParameter& stateParameter)
	{
		Reset(stateParameter.m_pTextureList, stateParameter.m_fPlayTime/stateParameter.m_nQty, stateParameter.m_bCycle);
		m_arRegionList = stateParameter.m_arFrameRegionList;
	}
	void Reset(u32 index)
	{
		if (index < m_arTextureList.size())
			m_idx = index;
		QiAnimator::Reset();
	}
	void SetFrameRegionList(const QsFrameRegionList& regionList) { m_arRegionList = regionList;}

	void AddTextureCb(const QcTextureCb& cb, u32 iFrame, u32 iParam = 0){  m_pTextureCbList.PushBack(QsTextureCbInfo(cb, iFrame, iParam));}
	void SetLoop(bool loop){ m_bLoop = loop;}
	f32 GetTimePerFrame() const {return m_fTimePerFrame;}
	void SetTimePerFrame(f32 fTime) { m_fTimePerFrame = fTime;}
protected:
	QcRegion* FindCurrentRegion(u32 index);
	void UpdateFrame(QcSceneNode*, u32 index);
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
	virtual void AddToSceneNode(QcSceneNode* pSceneNode);
private:
	QcList<QsTextureCbInfo> m_pTextureCbList;
	QcTexturePtrList m_arTextureList;
	QsFrameRegionList m_arRegionList;
	f32 m_fTimePerFrame;
	u32 m_idx;
	f32	m_fTimeCount;
	bool m_bLoop;
};