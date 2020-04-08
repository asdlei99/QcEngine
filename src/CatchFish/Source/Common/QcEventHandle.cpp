#include "Pch.hpp"
#include "Thread\QcCriticalSection.hpp"
#include "Thread\QcThread.hpp"

void QcEventHandle::PostEvent(u32 eEvt, u32 iParam, bool bRemoveBefore, QcEventHandle* pReceiver)
{
	QmLocker(GetCs());
	if (bRemoveBefore)
		QfErase(GetEventDeque(), eEvt);
	GetEventDeque().push_back(QsEvent(this, pReceiver, eEvt, iParam));
}
void QcEventHandle::DispatchAllEvent()
{
	QmLocker(GetCs());
	std::deque<QsEvent>& evtDeque = GetEventDeque();
	while(!evtDeque.empty())
	{
		const QsEvent& sEvent = evtDeque.front();
		if (sEvent.m_pReceiver)
			sEvent.m_pReceiver->OnEvent(sEvent);
		else
			sEvent.m_pSender->OnEvent(sEvent);
		evtDeque.pop_front();
	}
}

std::deque<QsEvent>& QcEventHandle::GetEventDeque()
{
	static std::deque<QsEvent> gsEvtDeque;
	return gsEvtDeque;
}
QcCriticalSection& QcEventHandle::GetCs()
{
	static QcCriticalSection cs;
	return cs;
}