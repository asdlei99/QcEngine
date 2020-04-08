#pragma once

#include "Sound\QcSound.hpp"
#include "Common\QcEncryptXmlLoader.hpp"

typedef boost::function<void(const QcObjType& objType)> QcFinishSoundCb;

#define QmPlaySound(objType)\
	do\
	{\
		static QcSound* psTempSound = QmSoundMgr->FindSound(QmObjName(objType));\
		if (psTempSound)\
			psTempSound->Play(false);\
		else\
			QmAssert(false);\
	}while(0,0)

class QcSoundMgr : public QcEncryptXmlLoader
{
	friend class QcSound;
public:
	QcSoundMgr();
	~QcSoundMgr();

	void Init(HWND hWnd, const char* psFile);
	void Reset();
	virtual void Parse(const QcXmlNodeIterator& pRootNode, void* pParam);

	void Play(const QcObjType& objType, bool bLoop = false, const QcFinishSoundCb& cb = QcFinishSoundCb());
	void Stop(const QcObjType& objType);

    void Update();
	QcSound* FindSound(const QcObjType& objType);
protected:
	FMOD::System* GetFMODSystem() {return m_pFmodSys;}
	CSoundManager* GetDXUTSoundMgr() {return m_oDXUTSoundMgr;}
protected:
	QcList<QcSharedPtr<QcSound>> m_arSound;
	CSoundManager* m_oDXUTSoundMgr;
    FMOD::System    *m_pFmodSys;
};