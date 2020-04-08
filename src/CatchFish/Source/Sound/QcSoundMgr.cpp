#include "pch.hpp"
#include "QcSoundMgr.hpp"
#include "Sound\QcSound.hpp"

struct QcSoundHelper
{
	bool operator ()(const QcSharedPtr<QcSound>& first, const QcSharedPtr<QcSound>& second) const
	{
		const QcObjType& left = first->GetObjType();
		const QcObjType& right = second->GetObjType();
		return left < right;
	}
	bool operator()(const QcSharedPtr<QcSound>& first, const QcObjType& objType) const
	{
		const QcObjType& left = first->GetObjType();
		const QcObjType& right = objType;
		return left < right;
	}
	bool operator()(const QcObjType& first, const QcSharedPtr<QcSound>& second) const
	{
		const QcObjType& left = first;
		const QcObjType& right = second->GetObjType();
		return left < right;
	}
};

QcSoundMgr::QcSoundMgr()
{
#ifdef QmDXUTSoundOn
	m_oDXUTSoundMgr = new CSoundManager();
#else
	m_oDXUTSoundMgr = NULL;
#endif
	m_pFmodSys = NULL;
}

QcSoundMgr::~QcSoundMgr()
{
	Reset();
}

void QcSoundMgr::Reset()
{
	m_arSound.Clear();
 	if (m_oDXUTSoundMgr)
 	{
 		delete m_oDXUTSoundMgr;
 		m_oDXUTSoundMgr = NULL;
 	}
	if (m_pFmodSys)
	{
		FMOD::ChannelGroup *pChannelGroup;
		m_pFmodSys->getMasterChannelGroup(&pChannelGroup);
		pChannelGroup->stop();
		m_pFmodSys->release();
		m_pFmodSys = NULL;
	}
}

void QcSoundMgr::Init(HWND hWnd, const char* psFile)
{
#ifdef QmDXUTSoundOn
	m_oDXUTSoundMgr->Initialize(hWnd, DSSCL_PRIORITY);
#endif

    FMOD::System_Create(&m_pFmodSys);
    m_pFmodSys->init(200, FMOD_INIT_NORMAL, 0);

	LoadFromFile(psFile);
}

void QcSoundMgr::Parse(const QcXmlNodeIterator& rootNode, void* pParam)
{
	QcXmlNodeIterator topNode = rootNode.FirstNode();
	if (!topNode)
		return;

	QcXmlNodeIterator node = topNode.FirstNode();
	while (node)
	{
		const char* psName = node.Attribute("name");
        const char* psExt = node.Attribute("ext");
		int iBuf = node.Attribute("MaxBuf", 1);
		QcSharedPtr<QcSound> pSound(new QcSound());
		if (pSound->Create(QcObjType(psName), this, psExt, iBuf))
		{
			m_arSound.push_back(pSound);
		}
		else
		{
			QmLog("fail to load %s", psName);
		}
		node = node.NextSibling();
	}
	std::sort(m_arSound.begin(), m_arSound.end(), QcSoundHelper());
}

void QcSoundMgr::Play(const QcObjType& objType, bool bLoop, const QcFinishSoundCb& cb)
{
	QcSound* pSound = FindSound(objType);
	if (pSound)
		pSound->Play(bLoop);
}
void QcSoundMgr::Stop(const QcObjType& objType)
{	
	QcSound* pSound = FindSound(objType);
	if (pSound)
		pSound->Stop();
}
QcSound* QcSoundMgr::FindSound(const QcObjType& objType)
{
	QcList<QcSharedPtr<QcSound>>::iterator iter = std::binary_searchEx(m_arSound.begin(), m_arSound.end(), objType, QcSoundHelper());
	if (iter != m_arSound.end())
		return *iter;
	QmAssert(false);
	return NULL;
}

void QcSoundMgr::Update()
{
	if (m_pFmodSys)
		m_pFmodSys->update();
}
