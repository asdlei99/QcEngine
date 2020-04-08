#pragma once

#include "Common\QcEncryptFileBuf.hpp"

//#define QmDXUTSoundOn 

class QcSoundMgr;
class QcSound : public QcObject
{
public:
	QcSound() : m_bPause(false), m_pDXUTSound(NULL), m_pSound(NULL), m_pChannel(NULL), m_fCurrentTime(0.) {}
	virtual ~QcSound(); 

	bool operator == (const QcObjType& objType) const
	{
		return m_objType == objType;
	}
	const QcObjType& GetObjType() const
	{
		return m_objType;
	}

	//dwNumBuffers 同一资源同时播放的最大上限
    bool Create(const QcObjType& objType, QcSoundMgr* pSoundMgr, const char* pFileExt = NULL, DWORD dwNumBuffers = 1);

	void Play(bool bLoop = false);
    void Stop();

	//重播
	void RePlay(bool bLoop = false)
	{
		m_bPause = false;
		if (m_pDXUTSound)
		{
			m_pDXUTSound->Reset();
			m_pDXUTSound->Play(0, bLoop ? DSBPLAY_LOOPING : 0);
		}
	}
	//暂停
	void Pause()
	{
		m_bPause = true;
		if (m_pDXUTSound)
		{
			m_pDXUTSound->Stop();
		}
	}
	void BgmPause(bool bPause)
	{
		if (m_pChannel)
		{
			m_pChannel->setPaused(bPause);
			m_fCurrentTime = 0.;
		}
	}

	bool IsPlaying()
	{
		if (m_pDXUTSound)
			m_pDXUTSound->IsSoundPlaying();
		return true;
	}
	bool IsPause()
	{
		return m_bPause;
	}
	/*
	设置通知事件
	1: 创建CreateSoundBuffer时设置: DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2
	2: 通过查询声音缓冲接口： LPDIRECTSOUNDBUFFER::QueryInterface(IID_IDirectSoundNotify, (VOID**)&pDSNotify)得到通知对象接口
	3: 设置通知点(SetNotificationPositions) 当播放到DSBPOSITIONNOTIFY::dwOffset时，对应的事件DSBPOSITIONNOTIFY::hEventNotify就会被激活
	4: 在线程中不断查询 获取等待 DSBPOSITIONNOTIFY::hEventNotify事件 (WaitForMultipleObjects), SetEventData
	*/
protected:
	CSound* m_pDXUTSound;
	bool m_bPause;
	QcEncryptFileBuf m_soundStream;
    QcObjType m_objType;
	f64 m_fCurrentTime;
    FMOD::Sound     *m_pSound;
    FMOD::Channel   *m_pChannel;
};