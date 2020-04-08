#include "pch.hpp"
#include "QcBackGround.hpp"
#include "Res\QcTexture.hpp"
#include "Res\QcResDescParam.hpp"
#include "Res\QcResBackGound.hpp"
#include "QcBackGroundMgr.hpp"

ImplRuntimeClass(QcBackGround);

QcBackGround::QcBackGround()
{

}

void QcBackGround::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcSceneNode::Init(desc, bLoadSubNode);

	const QcResBackGound& bgRes = static_cast<const QcResBackGound&>(desc);

	m_bgMusic = bgRes.GetBgMusic();
	m_bgWave =  QmResMgr->GetImageDesc(bgRes.GetBgWave());
	m_fWaveAlphaValue = bgRes.GetBgWaveAlpha();
	const QcList<QcResBackGound::QsBgNode>& bgSceneNodeList = bgRes.GetBgSceneNodeList();
	QcList<QcResBackGound::QsBgNode>::const_iterator iter(bgSceneNodeList.begin());
	for (; iter != bgSceneNodeList.end(); ++iter)
	{
		QcSceneNode* pSceneNode = QmResMgr->MakeSceneNodeInstance<QcSceneNode>(iter->m_sNodeName);
		pSceneNode->SetPosition(iter->m_vPos);
		m_arNodeList.PushBack(pSceneNode);
	}

    const QcList<QcObjType> *pObjList = &bgRes.GetDownsideObjList();
    QcList<QcObjType>::const_iterator iter2(pObjList->begin());
    for (; iter2 != pObjList->end(); ++iter2)
        m_arDownsideObj.push_back(QcSceneNodePtr(QmGlobalData.gpResMgr->MakeGameInstance(*iter2)));

    pObjList = &bgRes.GetUpsideObjList();
    iter2 = pObjList->begin();
    for (; iter2 != pObjList->end(); ++iter2)
        m_arUpsideObj.push_back(QcSceneNodePtr(QmGlobalData.gpResMgr->MakeGameInstance(*iter2)));
}

void QcBackGround::MirrorBgNode()
{
	MirrorXBgNode(this);
	//MirrorSubBgNode(m_arNodeList);
	//MirrorSubBgNode(m_arDownsideObj);
	//MirrorSubBgNode(m_arUpsideObj);
}

void QcBackGround::MirrorSubBgNode(QcSceneNodePtrList& nodeList)
{
	QcSceneNodePtrList::iterator iter(nodeList.begin());
	for (; iter != nodeList.end(); ++iter)
	{
		QcSceneNode* pNode = *iter;
		QcPoint point = pNode->GetPosition();
		point.X = -point.X;
		pNode->SetPosition(point);
	}
}

void QcBackGround::MirrorXBgNode(QcSceneNode* pNode)
{
 	QcPoint point = pNode->GetPosition();
 	point.X = -point.X;
 	pNode->SetPosition(point);
	pNode->SetScale(pNode->GetScaleX() * -1.f, eMaskX);
}

void QcBackGround::RenderBgSceneNode(QcDeviceRender& render, bool bIngoreSameBg)
{
	if (!bIngoreSameBg)
	{
 		QcSceneNodePtrList::iterator iter(m_arNodeList.begin());
 		for (; iter != m_arNodeList.end(); ++iter)
 		{
 			(*iter)->Render(render, &GetMatrix());
 		}
	}

	QcSceneNodePtrList::iterator iter = m_arUpsideObj.begin();
	for (; iter != m_arUpsideObj.end(); ++iter)
		(*iter)->Render(render, &GetMatrix());
}

void QcBackGround::Render( QcDeviceRender& render, const QcMatrix* pWorldMatrix /*= NULL*/)
{
	QcSceneNode::Render(render, pWorldMatrix);
	QmAutoRevertAlpha(render);
	render.SetMaterialColor(GetColor());

	QcSceneNodePtrList::iterator iter(m_arDownsideObj.begin());
	for (; iter != m_arDownsideObj.end(); ++iter)
		(*iter)->Render(render, &GetMatrix());
}

void QcBackGround::Animate( double dCurrentTime,f32 fElapsedTime )
{
    QcSceneNode::Animate(dCurrentTime, fElapsedTime);
    m_arDownsideObj.Animate(dCurrentTime, fElapsedTime);
    m_arUpsideObj.Animate(dCurrentTime, fElapsedTime);
}
