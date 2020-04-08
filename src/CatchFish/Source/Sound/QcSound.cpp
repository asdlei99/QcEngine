#include "pch.hpp"
#include "QcSound.hpp"
#include "QcSoundMgr.hpp"
#include "Common\QcMediaFileFinder.hpp"

QcSound::~QcSound()
{
	if (m_pDXUTSound)
	{
		m_pDXUTSound->Stop();
		delete m_pDXUTSound;
		m_pDXUTSound = NULL;
	}
    if (m_pChannel)
    {
        m_pChannel->stop();
        m_pChannel = NULL;
    }

    if (m_pSound)
    {
        m_pSound->release();
        m_pSound = NULL;
    }

}

bool QcSound::Create( const QcObjType& objType, QcSoundMgr* pSoundMgr, const char* pFileExt, DWORD dwNumBuffers /*= 1*/ )
{
	if (!pFileExt || !pFileExt[0])
		pFileExt = "wav";

	m_objType = objType;

	char sBuf2[QmFormatBuffer] = {0};
	sprintf_s(sBuf2, "%s.%s", objType.c_str(), pFileExt);
	if (m_soundStream.Load(sBuf2))
	{
#ifdef QmDXUTSoundOn
		if (QcNoCaseCompare(pFileExt) == "mp3")
#endif
		{
			FMOD::System * pSndSystem = pSoundMgr->GetFMODSystem();
			FMOD_CREATESOUNDEXINFO info = {0};
			info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
			info.length = m_soundStream.size();	
			if (1 == dwNumBuffers)
			{
				if (FMOD_OK != pSndSystem->createStream(m_soundStream.pointer(), FMOD_OPENMEMORY|FMOD_2D, &info, &m_pSound))
					return false;
				if (FMOD_OK != pSndSystem->playSound(FMOD_CHANNEL_FREE, m_pSound, true, &m_pChannel))
					return false;
				return true;
			}
			else
			{
				info.numchannels = dwNumBuffers;
				if (FMOD_OK != pSndSystem->createSound(m_soundStream.pointer(), FMOD_OPENMEMORY | FMOD_2D | FMOD_CREATESAMPLE, &info, &m_pSound))
					return false;
				m_soundStream.clear(true);
				return true;
			}
		}
#ifdef QmDXUTSoundOn
		else
		{
			if (pSoundMgr->GetDXUTSoundMgr()->Create(&m_pDXUTSound, (BYTE*)m_soundStream.pointer(), m_soundStream.size(), 0, GUID_NULL, dwNumBuffers) != S_OK)
				return false;
			m_soundStream.Init(0);
			return true;
		}
#endif
	}
	else
	{
		QmLog("Could not find %s", sBuf2);
	}
	return false;
}

void QcSound::Play( bool bLoop /*= false*/ )
{
	if (QcMath::equals(m_fCurrentTime, QmTimerMgr->GetCurrentTime()))
		return;

	m_fCurrentTime = QmTimerMgr->GetCurrentTime();
	if (m_pDXUTSound)
	{
		m_pDXUTSound->Play(0, bLoop ? DSBPLAY_LOOPING : 0);
	}
	else
	{
		FMOD::System *pSys;
		m_pSound->getSystemObject(&pSys);

		if (m_pChannel)
		{
			pSys->playSound(FMOD_CHANNEL_REUSE, m_pSound, false, &m_pChannel);
			m_pChannel->setMode(bLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		}
		else
		{
			FMOD::Channel *pChannel = NULL;
			pSys->playSound(FMOD_CHANNEL_FREE, m_pSound, false, &pChannel);
		}
	}
}

void QcSound::Stop()
{
	if (m_pDXUTSound)
	{
		m_pDXUTSound->Stop();
	}
	else
	{
		if (m_pChannel)
		{
			m_pChannel->setPaused(true);
			m_fCurrentTime = 0.;
		}
	}
}
