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

	//dwNumBuffers ͬһ��Դͬʱ���ŵ��������
    bool Create(const QcObjType& objType, QcSoundMgr* pSoundMgr, const char* pFileExt = NULL, DWORD dwNumBuffers = 1);

	void Play(bool bLoop = false);
    void Stop();

	//�ز�
	void RePlay(bool bLoop = false)
	{
		m_bPause = false;
		if (m_pDXUTSound)
		{
			m_pDXUTSound->Reset();
			m_pDXUTSound->Play(0, bLoop ? DSBPLAY_LOOPING : 0);
		}
	}
	//��ͣ
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
	����֪ͨ�¼�
	1: ����CreateSoundBufferʱ����: DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2
	2: ͨ����ѯ��������ӿڣ� LPDIRECTSOUNDBUFFER::QueryInterface(IID_IDirectSoundNotify, (VOID**)&pDSNotify)�õ�֪ͨ����ӿ�
	3: ����֪ͨ��(SetNotificationPositions) �����ŵ�DSBPOSITIONNOTIFY::dwOffsetʱ����Ӧ���¼�DSBPOSITIONNOTIFY::hEventNotify�ͻᱻ����
	4: ���߳��в��ϲ�ѯ ��ȡ�ȴ� DSBPOSITIONNOTIFY::hEventNotify�¼� (WaitForMultipleObjects), SetEventData
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