#pragma once

#include "Common\QcEncryptXmlLoader.hpp"

//#define QmMPParticle
//#define QmOldParticle

#ifdef QmOldParticle
#include "Particle\EmitterList.h"
using namespace PyroParticles;
#else
namespace PyroParticles{class IPyroParticleEmitter;}
#endif


#ifdef QmMPParticle
#include "MP_Particle\mp.h"
#endif



template<class T>
T* QfMakeSceneNodeInstance(const QcResImageDesc* pImageDesc)
{
	T* pRet = NULL;
	if (pImageDesc)
	{
		pRet = new T();
		pRet->Init(*pImageDesc, false);
	}
	return pRet;
}


class QcTexture;
class QcResDesc;
class QcResImageDesc;
class QcResFishDesc;
class QcSceneAnimation;
class QcPathMgr;
class QcRegularMgr;
class QcResPathDesc;
class QcResFishRegularDesc;
class QcSettingMgr;
class QcEffectMgr;
class QcResPackLoader;
class QcResManager : public QcEncryptXmlLoader
{
	QmDisableCopyAssignment(QcResManager);
public:
	QcResManager(QcDeviceRender& render);
	~QcResManager();
	enum QeLoadingFlag
	{
		eProcessBarRes,
		eSceneRes,
	};

	void Init();
	void LoadParticle(QcDeviceRender& render);
	void SetLoadingFlag(QeLoadingFlag eLoadingFlag){ m_currentLoadingFlag = eLoadingFlag;}
	void FinishLoading();

	template<class T>
	T* MakeSceneNodeInstance(const QcObjType& objType)
	{
		return MakeSceneNodeInstance<T>(GetImageDesc(objType));
	}
	template<class T>
	T* MakeSceneNodeInstance(const QcResImageDesc* pImageDesc)
	{
		return QfMakeSceneNodeInstance<T>(pImageDesc);
	}

	//使用objType对应的QcResImageDesc创建对象。
	QcSceneNode* MakeGameInstance(const QcObjType& objType, bool bLoadSubNode = true)
	{
		return MakeGameInstance(GetImageDesc(objType), bLoadSubNode);
	}
	QcSceneNode* MakeGameInstance(const QcResImageDesc* pImageDesc, bool bLoadSubNode = true);

	template<class T>
	T* LoadRes2(T* psDesc)
	{
		return static_cast<T*>(LoadRes(psDesc));
	}
	template<class T>
	T* LoadRes2(const QcSharedPtr<T>& psDesc)
	{
		return static_cast<T*>(LoadRes(psDesc.Get()));
	}
	QcResDesc* LoadRes(QcResDesc* psDesc);

	//image desc
	QcTexture* LoadTexture(const QcString& fileName);
	QcTexture* GetTexture(const QcString& fileName);
	QcResImageDesc* LoadImageDesc(const QcObjType& image);
	QcResImageDesc* GetImageDesc(const QcObjType& objType) const
	{
		QcResImageDescMapConstIter iter = m_imageDescMap.find(objType);
		if (iter != m_imageDescMap.end())
			return iter->second.Get();
		iter = m_progressbarDescMap.find(objType);
		if (iter != m_progressbarDescMap.end())
			return iter->second.Get();
		return NULL;
	}
	const QcResImageDescPtr& GetEmptyImageDesc() const
	{
		return m_pEmptyDesc;
	}
	int FindImageFile(const char* sFileName);

	//path desc
	QcPathMgr* GetPathMgr(){ return m_pPathMgr.Get();}
	QcRegularMgr* GetRegularMgr(){ return m_pRegularMgr.Get();}
	QcSettingMgr* GetSettingMgr() {return m_pSettingMgr;}
	//QcEffect
	QcEffectMgr* GetEffectMgr() {return m_pEffectMgr;}
	QcEffect* MakeEffect(const char* psName);
#ifdef QmOldParticle
	IPyroFile* GetPyroFile() {return m_pPyroFile; }
#endif
#ifdef QmMPParticle
	MP_Manager* GetMpMgr() {return &m_MPManager;}
#endif

	QcDeviceRender& GetRender() {return m_render;}
	QcCriticalSection& GetLocker() {return m_csLoading;}
protected:
	virtual void Parse(const QcXmlNodeIterator& pRootNode, void* pParam);
	QcResImageDesc* LoadImageRes(QcResImageDesc* psImageDesc);
	bool LoadRegularList(QcResFishRegularDesc* pRegularDesc);
	const QcPoint& GetOffsetPoint(const QcString& imageName) const
	{
		std::map<QcString,QcPoint, QcNoCaseCompare>::const_iterator iter(m_textureOffsetMap.find(imageName));
		if (iter != m_textureOffsetMap.end())
			return iter->second;
		static QcPoint point;
		return point;
	}
	void AddTexture(const QcString& fileName, QcTexture* pTexture)
	{
		if (m_currentLoadingFlag == eProcessBarRes)
			m_progressbarTexMap[fileName] = pTexture;
		else
			m_textureMap[fileName] = pTexture;
	}
	void AddImageDesc(QcResImageDesc* psImageDesc);
	void ClearProgressbarRes()
	{
		QcResImageDescMap().swap(m_progressbarDescMap);
		QcTexturePtrMap().swap(m_progressbarTexMap);
	}
private:
	QcCriticalSection m_csLoading;
	QcResImageDescPtr m_pEmptyDesc;
	QcDeviceRender& m_render;
	std::map<QcString,QcPoint, QcNoCaseCompare> m_textureOffsetMap;
	QeLoadingFlag m_currentLoadingFlag;
	QcTexturePtrMap m_textureMap;
	QcResImageDescMap m_imageDescMap;
	QcTexturePtrMap m_progressbarTexMap;
	QcResImageDescMap m_progressbarDescMap;
	QcSharedPtr<QcPathMgr> m_pPathMgr;
	QcSharedPtr<QcSettingMgr> m_pSettingMgr;
	QcSharedPtr<QcRegularMgr> m_pRegularMgr;
	QcSharedPtr<QcEffectMgr> m_pEffectMgr;
#ifdef QmOldParticle
	IPyroParticleLibrary    *m_pPyroLib;
	PyroGraphics::IDevice   *m_pPyroGraphic;
	IPyroFile               *m_pPyroFile;
#endif
#ifdef QmMPParticle
	MP_Manager m_MPManager;
#endif
};

template<class T>
T* QfMakeSceneNodeInstance(const QcObjType& objType)
{
	return QmResMgr->MakeSceneNodeInstance<T>(objType);
}
template<class T>
T* QfMakeEmptyInstance()
{
	return QfMakeSceneNodeInstance<T>(QmResMgr->GetEmptyImageDesc());
}