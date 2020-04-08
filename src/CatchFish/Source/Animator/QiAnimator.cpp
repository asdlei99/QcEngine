#include "pch.hpp"
#include "QiAnimator.hpp"
#include "Scene\QcSceneNode.hpp"

void QiAnimator::DoFinishedAction(QcSceneNode* pSceneNode)
{
	QiAnimatorPtr save(this);
	if (m_pFinishCallBackerList.size())
	{
		std::vector<QsCallBackInfo> tempCbList;
		tempCbList.swap(m_pFinishCallBackerList);
		std::vector<QsCallBackInfo>::iterator iter(tempCbList.begin());
		std::vector<QsCallBackInfo>::iterator endIter(tempCbList.end());
		for (; iter != endIter; ++iter)
		{
			if (iter->m_cb)
				iter->m_cb(iter->m_iParam);
		}
	}
	if (m_bFinished && !m_bInDeque)
		pSceneNode->FinishAnimator(this);
}

void QiAnimator::Suspend( bool bSuspend )
{
    m_bSuspend = bSuspend;
    std::vector<QsCallBackInfo> &CallBackList = m_bSuspend ? m_pSuspendCallBackerList : m_pResumeCallBackerList;
    if (!CallBackList.empty())
    {
        std::vector<QsCallBackInfo>::iterator iter(CallBackList.begin());
        std::vector<QsCallBackInfo>::iterator endIter(CallBackList.end());
        for (; iter != endIter; ++iter)
        {
            if (iter->m_cb)
                iter->m_cb(iter->m_iParam);
        } 
    }   
}

void QiAnimator::DoStartAction()
{
    if (!m_pStartCallBackerList.empty())
    {
        std::vector<QsCallBackInfo>::iterator iter(m_pStartCallBackerList.begin());
        std::vector<QsCallBackInfo>::iterator endIter(m_pStartCallBackerList.end());
        for (; iter != endIter; ++iter)
        {
            if (iter->m_cb)
                iter->m_cb(iter->m_iParam);
        }
    }
}
