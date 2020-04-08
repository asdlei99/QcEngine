#include "pch.hpp"
#include "QcTimerMgr.hpp"

void QcTimerMgr::HandleTimer(f64 fCurrentTime, f32 fElapsedTime)
{
	m_fElapsedTime = fElapsedTime;
	m_fCurrentTime = fCurrentTime;
	DoAction(m_fElapsedTime);
}

void QcTimerMgr::DoAction(f32 fElapsedTime)
{
	QcList<QcTimerHandle*>::Tracker tracker(m_arTimerList);
	QcList<QcTimerHandle*>::iterator iter(m_arTimerList.begin());
	QcList<QcTimerHandle*>::iterator endIter(m_arTimerList.end());
	for (; iter != endIter; ++iter)
	{
		if (*iter != NULL)
		{
			QmProfileFragment(QcTimerMgr::HandleTimer);
			(*iter)->DoAction(fElapsedTime);
		}
	}
}

void QcTimerMgr::ClearAllTimer()
{
	QcList<QcTimerHandle*>::iterator iter(m_arTimerList.begin());
	for (; iter != m_arTimerList.end(); ++iter)
	{
		if (*iter != NULL)
			(*iter)->ClearTimer();
	}
}