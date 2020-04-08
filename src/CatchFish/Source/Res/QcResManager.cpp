#include "pch.hpp"
#include "QcResManager.hpp"
#include "QcTexture.hpp"
#include "QcResPackLoader.hpp"
#include "QcResImageDesc.hpp"
#include "QcResPathDesc.hpp"
#include "QcPathMgr.hpp"
#include "Regular\QcRegularMgr.hpp"
#include "Common\QcMediaFileFinder.hpp"
#include "RenderSystem\QcDeviceRender.hpp"
#include "Scene\QcSceneAnimation.hpp"
#include "Thread\QcLock.hpp"
#include "Setting\QcSettingMgr.hpp"
#include "Setting\QcResDialogDesc.hpp"
#include "EffectRender\QcEffectMgr.hpp"

#ifdef QmMPParticle
#include "MP_Particle/3d.h"
#endif

#ifdef QmOldParticle
#include "Particle\PyroD3D9.h"
#include "Math\QcMatrix3.hpp"
class QcOldParticleLoader : public QcEncryptFileBuf, public IPyroFileIO
{
public:
	QcOldParticleLoader() : m_iCurrent(0){}

	virtual long Read(void *pDest, long nBytes) 
	{
		int nSize = min(nBytes,  int(size() - m_iCurrent));
		memcpy(pDest, pointer() + m_iCurrent, nSize);
		m_iCurrent += nSize;
		return nSize;
	}
	virtual long Write(void *pSrc, long nBytes) {QmAssert(false); return m_iCurrent;}
	virtual long Seek(long Position, int Where = SEEK_SET)
	{
		switch(Where)
		{
		case  SEEK_SET:
			{
				m_iCurrent = Position;
				break;
			}
		case  SEEK_CUR:
			{
				m_iCurrent += Position;
				break;
			}
		case  SEEK_END:
			{
				m_iCurrent = size() + Position;
				break;
			}
		default:
			{
				QmAssert(false);
				break;
			}
		}
		return 0;
	}
	virtual long Tell()
	{
		return m_iCurrent;
	}
protected:
	long m_iCurrent;
};
#endif


QcResManager::QcResManager(QcDeviceRender& render)
: m_render(render)
{
	QmGlobalData.gpResMgr = this;
	m_pEmptyDesc = new QcResImageDesc();
	m_pPathMgr = new QcPathMgr();
	m_pRegularMgr = new QcRegularMgr();
	m_pSettingMgr = new QcSettingMgr(this);
	m_pEffectMgr = new QcEffectMgr();

#ifdef QmOldParticle
	m_pPyroLib = NULL;
	m_pPyroGraphic = NULL;
	m_pPyroFile = NULL;
#endif
}

QcResManager::~QcResManager()
{
#ifdef QmOldParticle
	if (m_pPyroFile)
	{
		m_pPyroFile->DestroyTextures();
		delete m_pPyroFile;
		m_pPyroFile = NULL;
	}
	if (m_pPyroLib) 
		m_pPyroLib->Done();
	SAFE_DELETE(m_pPyroGraphic);
#endif
#ifdef QmMPParticle
	d3d9.Clear();
#endif
}

void QcResManager::Init()
{
	LoadFromFile("offsetPoint.xml", (void*)1);
	m_pEffectMgr->Init("ResEffect.xml", m_render);
}

void QcResManager::FinishLoading()
{
	QmGlobalData.m_pFilePacker = NULL;
	ClearProgressbarRes();
}


int QcResManager::FindImageFile(const char* psFileName)
{
	int iRet = 0;
	if (QmGlobalData.m_pFilePacker)
	{
		iRet =  QmGlobalData.m_pFilePacker->FindFile(psFileName) != QmInvaidHandle ? 1 : 0;
	}
	if (iRet == 0)
	{
#if 0
		//仅合图应用
		if (GetTexture(psFileName))
			return 2;
#endif
		QcString file;
		if (QcMediaFileFinderInstance->FindFile(psFileName, file))
			return 3;
	}
	return iRet;
}

void QcResManager::Parse(const QcXmlNodeIterator& pRootNode, void* pParam)
{
	QcXmlNodeIterator topNode = pRootNode.FirstNode();
	switch((int)pParam)
	{
	case 1:
		{
			QcXmlNodeIterator node = topNode.FirstNode();
			while (node)
			{
				QcString imageName(node.Attribute("File"));
				QcPoint offsetPoint;

				offsetPoint = node.Attribute("Offset", offsetPoint);
				m_textureOffsetMap[imageName] = offsetPoint;

				node = node.NextSibling();
			}
			break;
		}
#if 0
	case 2:
		{
			QcXmlNodeIterator node = topNode.FirstNode();
			std::map<QcString, QsTextureInstance*> tempMap; 
			while (node)
			{
				QcString sImageName(node.SubValue("name"));
				QcString sTexture(node.SubValue("texture"));
				sTexture += ".png";

				std::map<QcString, QsTextureInstance*>::iterator iter = tempMap.find(sTexture);
				QsImageInfo info;
				if (iter == tempMap.end())
				{
					info.m_pTextureInstance = new QsTextureInstance(sTexture);
					tempMap.insert(make_pair(sTexture, info.m_pTextureInstance.Get()));
				}
				else
				{
					info.m_pTextureInstance = iter->second;
				}

				const QcPoint& offsetPoint = GetOffsetPoint(sImageName);

				info.m_fOffsetX = offsetPoint.X;
				info.m_fOffsetY = offsetPoint.Y;

				istringstream uv(node.SubValue("rect"));
				uv >> info.fImgPosX >> info.fImgPosY >> info.fImgWidth >> info.fImgHeight;

				QcTexture* pTexture = new QcTexture(info);
				m_textureMap[sImageName] = pTexture;

				node = node.NextSibling();
			}
			break;
		}
#endif
	}
}

QcResDesc* QcResManager::LoadRes(QcResDesc* psDesc)
{
	if (psDesc->Is<QcResImageDesc*>())
		psDesc = LoadImageRes(static_cast<QcResImageDesc*>(psDesc));
	else if (psDesc->Is<QcResDialogDesc*>())
		GetSettingMgr()->LoadDesc(static_cast<QcResDialogDesc*>(psDesc));
	return psDesc;
}

void QcResManager::LoadParticle(QcDeviceRender& render)
{
	QmResLocker();
	QmProfilerLoading(eLoadingParticleTime);
#ifdef QmOldParticle
	{
		m_pPyroLib = CreateParticleLibrary(PYRO_SDK_VERSION);
		m_pPyroGraphic = new CGraphicsDevice_D3D9(render.GetDevice(), m_pPyroLib->GetMaxRenderParticleQuads(), FALSE);
		m_pPyroLib->Init(m_pPyroGraphic);

		QcOldParticleLoader temp;
		if (temp.Load("Particles.pyro"))
		{
			m_pPyroFile = m_pPyroLib->LoadPyroFile(&temp);
			m_pPyroFile->CreateTextures();
		}
		else
		{
			QmLogError("Particles.pyro");
		}
	}
#endif

#ifdef QmMPParticle
	{	
		d3d9.Init(render.GetDevice());
		m_MPManager.Initialization(MAGIC_INTERPOLATION_DEFAULT, MAGIC_NOLOOP, MAGIC_CHANGE_EMITTER_ONLY);
		QcEncryptFileBuf temp;
		if (temp.Load("yule_.ptc"))
		{
			m_MPManager.LoadEmittersFromFile(temp.pointer(), true);
			m_MPManager.LoadTextures(0,1920,1080);
			m_MPManager.Stop();
			m_MPManager.CloseFiles();
		}
		else
		{
			QmLogError("fail to load yule_.ptc");
		}
	}
#endif
}

QcResImageDesc* QcResManager::LoadImageRes(QcResImageDesc* psImageDesc)
{
	QcResImageDesc* pSource = GetImageDesc(psImageDesc->GetObjType());
	if (pSource != NULL)
		return pSource;

	AddImageDesc(psImageDesc);
	{
		QmAutoRevert(s32, QmGlobalData.giForceTexInVideoMem);
		if (QmGlobalData.giForceTexInVideoMem == 0)
			QmGlobalData.giForceTexInVideoMem = psImageDesc->IsForceInVideo();

		QcList<QsStateParameter>& stateList = psImageDesc->GetStateParameterList(); 
		QcList<QsStateParameter>::iterator iter(stateList.begin());
		QcList<QsStateParameter>::iterator endIter(stateList.end());
		for (; iter != endIter; ++iter)
		{
			QsStateParameter& state = *iter;
			state.m_pTextureList.Clear();

			QcStringList fileList;
			psImageDesc->GetStateImageList(state, fileList);
			QcStringList::iterator fileIter(fileList.begin());
			QcStringList::iterator endFileIter(fileList.end());
			for (; fileIter != endFileIter; ++fileIter)
			{
				state.m_pTextureList.PushBack(LoadTexture(*fileIter));
			}
		}
#ifdef QmTestVersion
		QmDebugHelper.FinishedLoadingDesc(psImageDesc->GetObjType());
#endif
	}

	QcList<QcResImageDescPtr>& subList = psImageDesc->GetSubDescList();
	if (subList.size())
	{
		QcList<QcResImageDescPtr>::iterator subListIter(subList.begin());
		for (; subListIter != subList.end(); ++subListIter)
		{
			(*subListIter) = LoadImageRes(*subListIter);
		}
	}
	psImageDesc->PostLoad(this);

	return psImageDesc;
}

void QcResManager::AddImageDesc(QcResImageDesc* psImageDesc)
{
	if (m_currentLoadingFlag == eProcessBarRes)
		m_progressbarDescMap[psImageDesc->GetObjType()] = psImageDesc;
	else
		m_imageDescMap[psImageDesc->GetObjType()] = psImageDesc;
}

QcTexture* QcResManager::LoadTexture(const QcString& fileName)
{
	QcTexture* pTexture = GetTexture(fileName);
	if (pTexture == NULL)
	{
		QsImageInfo info;
		info.m_pTextureInstance = new QsTextureInstance(fileName);

		const QcPoint& offsetPoint = GetOffsetPoint(fileName);
		info.m_fOffsetX = offsetPoint.X;
		info.m_fOffsetY = offsetPoint.Y;

		pTexture = new QcTexture(info);
		AddTexture(fileName, pTexture);
	}

	if (pTexture->GetTexture() == NULL)
	{
		if (!pTexture->Create(m_render.GetDevice()))
		{
			QmAssert(false);
		}
	}
	pTexture->Update();
	return pTexture;
}

QcTexture* QcResManager::GetTexture(const QcString& fileName)
{
	QcTexturePtrMapIter iter = m_textureMap.find(fileName);
	if (iter != m_textureMap.end())
		 return iter->second.Get();
	iter = m_progressbarTexMap.find(fileName);
	if (iter != m_progressbarTexMap.end())
		return iter->second.Get();
	return NULL;
}

QcSceneNode* QcResManager::MakeGameInstance(const QcResImageDesc* pImageDesc, bool bLoadSubNode)
{
	QcSceneNode* pRet = NULL;
	if (pImageDesc)
	{
		if ((pRet = const_cast<QcResImageDesc*>(pImageDesc)->MakeInstance(bLoadSubNode)) == NULL)
		{
			QmAssert(false);
		}
	}
	return pRet;
}

QcEffect* QcResManager::MakeEffect(const char* psName)
{
	return GetEffectMgr()->MakeEffect(psName);
}