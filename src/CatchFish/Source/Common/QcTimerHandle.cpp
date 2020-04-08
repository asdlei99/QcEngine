#include "pch.hpp"
#include "QcTimerHandle.hpp"
#include "QcTimerMgr.hpp"

QcTimerHandle::~QcTimerHandle()
{
	if (m_pTimerList)
	{
		delete m_pTimerList;
		QmTimerMgr->RemoveTimer(this);
	}
}

void QcTimerHandle::SetTimer(u32 nTimer, f32 fTimeoutInSecs, bool bAutoKill, void* pContext)
{
	if (m_pTimerList == NULL)
	{
		m_pTimerList = new QcList<QsTimer>();
		QmTimerMgr->AddTimer(this);
	}

	QcList<QsTimer>& timerList = *m_pTimerList;
	int iPos = -1;
	{
		QcList<QsTimer>::iterator iter(timerList.begin());
		QcList<QsTimer>::iterator endIter(timerList.end());
		for (; iter != endIter; ++iter)
		{
			//有可能在
			if (iter->m_nID == nTimer)
			{
				iter->m_fTimeoutInSecs = fTimeoutInSecs;
				iter->m_fCountdown = fTimeoutInSecs;
				iter->m_pContext = pContext;
				iter->m_bAutoKill = bAutoKill;
				return;
			}
			else if (iPos == -1 && iter->m_nID == 0)
			{
				iPos = iter - timerList.begin();
			}
		}
	}
	if (iPos == -1)
		timerList.PushBack(QsTimer(nTimer, fTimeoutInSecs, bAutoKill, pContext));
	else
		timerList[iPos] = QsTimer(nTimer, fTimeoutInSecs, bAutoKill, pContext);
}
void QcTimerHandle::KillTimer(u32 nTimer)
{
	if (m_pTimerList)
	{
		int iPos = m_pTimerList->Find(nTimer);
		if (iPos != -1)
			(*m_pTimerList)[iPos].m_nID = 0;
	}
}