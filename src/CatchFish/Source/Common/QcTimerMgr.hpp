#pragma once

class QcTimerHandle;
class QcTimerMgr : public QcObject, public QcSingleton<QcTimerMgr>
{
	friend class QcTimerHandle;
public:
	f32 GetElapsedTime(){ return m_fElapsedTime;}
	f64 GetCurrentTime() {return  m_fCurrentTime;}
	void HandleTimer(f64 fCurrentTime, f32 fElapsedTime);
	void ClearAllTimer();
protected:
	void AddTimer(QcTimerHandle* pObject)
	{
		m_arTimerList.Insert(pObject);
	}
	void RemoveTimer(QcTimerHandle* pObject)
	{
		m_arTimerList.Erase(pObject);
	}
	void DoAction(f32 fElapsedTime);
private:
	QcPointerList<QcTimerHandle> m_arTimerList;
	f32 m_fElapsedTime;
	f64 m_fCurrentTime;
};