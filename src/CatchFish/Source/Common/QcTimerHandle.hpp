#pragma once

#include "Common\QcList.hpp"

class QcTimerHandle
{
	struct QsTimer
	{
		QsTimer(u32 nId, f32 fCount, bool bAutoKill, void* pContext)
			: m_nID(nId), m_fTimeoutInSecs(fCount), m_fCountdown(fCount), m_bAutoKill(bAutoKill), m_pContext(pContext)
		{}

		bool operator==(const u32& nId) const
		{
			return m_nID == nId;
		}
		bool operator==(const QsTimer& timer) const
		{
			return m_nID == timer.m_nID;
		}
		u32 m_nID; 
		f32 m_fTimeoutInSecs;
		f32 m_fCountdown;
		bool m_bAutoKill;
		void* m_pContext;
	};
public:
	QcTimerHandle::QcTimerHandle()
		: m_pTimerList(NULL)
	{
	}
	virtual ~QcTimerHandle();
	virtual void OnTimer(u32 /*nTimer*/, void* /*pContext*/){}
	void SetTimer(u32 nTimer, f32 fTimeoutInSecs, bool bAutoKill = false, void* pContext = NULL);
	void KillTimer(u32 nTimer);
	void ClearTimer()
	{
		if (m_pTimerList)
			m_pTimerList->Clear();
	}
	void DoAction(f32 fElapsedTime)
	{
		QmAssert(m_pTimerList->size() < 100);

		QcList<QsTimer>& timerList = *m_pTimerList;
		QcList<QsTimer>::Tracker tracker(timerList);
		QcList<QsTimer>::iterator iter(timerList.begin());
		QcList<QsTimer>::iterator endIter(timerList.end());
		for (; iter != endIter; ++iter)
		{
			if (iter->m_nID > 0)
			{
				iter->m_fCountdown -= fElapsedTime;
				if (iter->m_fCountdown < 0)
				{
					u32 nID = iter->m_nID;
					if (iter->m_bAutoKill)
						iter->m_nID = 0;
					OnTimer(nID, iter->m_pContext);
					//当在OnTimer中调用SetTimer时，iter内容会被修改，这时不应将iter->m_nID设置为0
					iter->m_fCountdown = iter->m_fTimeoutInSecs;
				}
			}
		}
	}
private:
	QcList<QsTimer>* m_pTimerList;
};