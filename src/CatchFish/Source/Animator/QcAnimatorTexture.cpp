#include "pch.hpp"
#include "QcAnimatorTexture.hpp"
#include "Scene\QcSceneNode.hpp"

ImplRuntimeClass2(QcAnimatorTexture, 700);

void QcAnimatorTexture::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	m_fTimeCount += fElapsedTime;
	while (m_fTimeCount >= m_fTimePerFrame)
	{
		++m_idx;
		if (m_idx >= m_arTextureList.size())
		{
			if (m_bLoop)
				m_idx = 0;
			else
			{
				m_bFinished = true;
				UpdateFrame(pSceneNode, u32(-1));
				return;
			}
		}
		m_fTimeCount -= m_fTimePerFrame;

		if (m_pTextureCbList.size())
		{
			QcList<QsTextureCbInfo>::Tracker track(m_pTextureCbList);
			QcList<QsTextureCbInfo>::iterator iter(m_pTextureCbList.begin());
			QcList<QsTextureCbInfo>::iterator endIter(m_pTextureCbList.end());
			for (; iter != endIter; ++iter)
			{
				if (iter->m_iFrame == m_idx && iter->m_cb)
				{
					iter->m_cb(iter->m_iParam);
				}
			}
		}
	}
	UpdateFrame(pSceneNode, m_idx);
}

void QcAnimatorTexture::AddToSceneNode(QcSceneNode* pSceneNode)
{
	UpdateFrame(pSceneNode, m_idx);
}

void QcAnimatorTexture::UpdateFrame(QcSceneNode* pSceneNode, u32 index)
{
	if (m_idx < m_arTextureList.size())
	{
		pSceneNode->SetCurrentTexture(m_arTextureList[m_idx]);
	}
	if (m_arRegionList.size())
	{
		pSceneNode->UpdateFrameRegion(FindCurrentRegion(m_idx));
	}
}

QcRegion* QcAnimatorTexture::FindCurrentRegion(u32 index)
{
	QsFrameRegionList::iterator iter(m_arRegionList.begin());
	QsFrameRegionList::iterator endIter(m_arRegionList.end());
	for (; iter != endIter; ++iter)
	{
		if (u32(iter->m_iBeginFrame) >= index)
			return iter->m_pRegion;
	}
	return NULL;
}