#pragma once

#include "Scene\QcSceneNode.hpp"
class QcBackGround : public QcSceneNode
{
	DeclareRuntimeClass(QcBackGround);
public:
	struct QsHoleInfo
	{
		QcPointList m_area;
		QcSceneNodePtr m_pNode;
	};
	explicit QcBackGround();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	void MirrorBgNode();
    void Animate(double dCurrentTime,f32 fElapsedTime);
    void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	void RenderBgSceneNode(QcDeviceRender& render, bool bIngoreSameBg = false);

	const QcObjType& GetMusic() const{ return m_bgMusic;}
	QcResImageDesc* GetWave() const{ return m_bgWave;}
	const f32 GetWaveAlpha() const {return m_fWaveAlphaValue;}
protected:
	void MirrorSubBgNode(QcSceneNodePtrList& nodeList);
	void MirrorXBgNode(QcSceneNode* pNode);
protected:
	QcObjType m_bgMusic;
	QcSceneNodePtrList m_arNodeList;
    QcSceneNodePtrList m_arDownsideObj;
    QcSceneNodePtrList m_arUpsideObj;
	QcResImageDesc* m_bgWave;
	f32 m_fWaveAlphaValue;
};