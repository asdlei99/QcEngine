#pragma once

class QcCriticalSection;
class QcEventHandle;
struct QsEvent
{
	QsEvent(QcEventHandle* pSender, QcEventHandle* pReceiver, u32 iEvt, u32 iParam)
		: m_pSender(pSender), m_pReceiver(pReceiver), m_iEvt(iEvt), m_pParam((void*)iParam)
	{}

	bool operator==(const QsEvent& evt) const
	{
		return m_iEvt == evt.m_iEvt;
	}
	bool operator==(const u32& iEvt) const
	{
		return m_iEvt == iEvt;
	}
	QcEventHandle* m_pSender;
	QcEventHandle* m_pReceiver;
	u32 m_iEvt;
	union
	{
		u32 m_iParam;
		void* m_pParam;
	};
};

class QcEventHandle
{
public:
	virtual void OnEvent(const QsEvent& evt){};
	void PostEvent(u32 eEvt, u32 iParam = 0, bool bRemoveBefore = false, QcEventHandle* pReceiver = NULL);
	void SendEvent(u32 eEvt, u32 iParam = 0, QcEventHandle* pReceiver = NULL)
	{
		if (pReceiver)
			pReceiver->OnEvent(QsEvent(this, pReceiver, eEvt, iParam));
		else
			OnEvent(QsEvent(this, pReceiver, eEvt, iParam));
	}
	static void DispatchAllEvent();
protected:
	static std::deque<QsEvent>& GetEventDeque();
	static QcCriticalSection& GetCs();
};