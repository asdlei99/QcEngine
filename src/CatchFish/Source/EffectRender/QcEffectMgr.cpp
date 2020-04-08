#include "pch.hpp"
#include "QcEffectMgr.hpp"
#include "QcEffect.hpp"
#include "Common\QcEncryptFileBuf.hpp"
#include "Res\QcResManager.hpp"
#include "RenderSystem\QcDeviceRender.hpp"
#include "QcBloomEffect.hpp"

QcEffectMgr::QcEffectMgr() : m_pRenderBack(NULL){}

QcEffectMgr::~QcEffectMgr()
{
	QcEffectMap::iterator iter(m_effectMap.begin());
	for (; iter != m_effectMap.end(); ++iter)
	{
		SAFE_RELEASE(iter->second.m_pEffectInstance);
	}
	m_effectMap.clear();

    m_effectRt.Cleanup();
    SAFE_RELEASE(m_pRenderBack);
}


void QcEffectMgr::Init(const char* psFile, QcDeviceRender& render)
{
	LoadFromFile(psFile, (void*)(&render));

	//render.SetDefaultEffect(MakeEffect("Default"));
	{
		QmResLocker();
		LPDIRECT3DDEVICE9 pd3dDevice = render.GetDevice();
		for (int i=0; i<2; i++)
		{
			if (FAILED(D3DXCreateTexture(pd3dDevice, TARGET_WIDTH, TARGET_HEIGHT, 1, D3DUSAGE_RENDERTARGET, 
				D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_effectRt.m_pRenderTarget[i])))
				QmLogError("Render Target Create Failed!");
		}

		if (FAILED(D3DXCreateTexture(pd3dDevice, TARGET_WIDTH, TARGET_HEIGHT, 1, D3DUSAGE_RENDERTARGET, 
			D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pRenderBack)))
			QmLogError("Render Target Create Failed!");
	}
}

void QcEffectMgr::Parse(const QcXmlNodeIterator& rootNode, void* pParam)
{
	QcXmlNodeIterator topNode = rootNode.FirstNode();
	if (!topNode)
		return;

	QcXmlNodeIterator node = topNode.FirstNode();
	while (node)
	{
		QsEffectInfo info;
		info.m_pRunClass = QmRuntimeClass(QcEffect);

		const char* psClass = node.Attribute("class");
		if (strlen(psClass) > 0)
		{
			info.m_pRunClass = QmClassFactoryInstance->Find(psClass);
			QmAssert(info.m_pRunClass != NULL);
		}
		QcString sFile(node.Attribute("file"));
		QcString sName(node.Attribute("name"));
		if (sFile.size() == 0)
		{
			sFile = sName + ".fx";
		}
		
		info.m_pEffectInstance = LoadEffect(sFile.c_str(), *static_cast<QcDeviceRender*>(pParam));
		m_effectMap[sName] = info;

		node = node.NextSibling();
	}
}

ID3DXEffect* QcEffectMgr::LoadEffect(const char* psFile, QcDeviceRender& render)
{
	ID3DXEffect* pRet = NULL;
	QcEncryptFileBuf byteArray;
	if (byteArray.Load(psFile, true))
	{
		QmResLocker();

		DWORD dwFlag = 0;
#ifdef QmTestVersion
		//dwFlag = D3DXSHADER_DEBUG | D3DXSHADER_NO_PRESHADER |D3DXSHADER_SKIPOPTIMIZATION;
#endif
		ID3DXBuffer *pError = NULL;
		HRESULT hr = D3DXCreateEffect(render.GetDevice(), byteArray.pointer(), byteArray.size(), NULL, NULL, dwFlag, NULL, &pRet, &pError);
		if ( FAILED( hr ) )
		{
			if ( pError )
			{
				QmLog((const char*)pError->GetBufferPointer());
#ifdef QmTestVersion
				if (IsDebuggerPresent())
					::MessageBoxA( NULL, (const char*)pError->GetBufferPointer(), "Error", MB_OK );
#endif
				
			}
			QmLogError("D3DXCreateEffect");
			pRet = NULL;
		}
		else
			pRet->SetTechnique("Effect");
	}
	else
	{
		QmLog("Fail to Load %s", psFile);
	}
	return pRet;
}

QcEffect* QcEffectMgr::MakeEffect(const char* psEffectName, QcRuntimeClass* pRunningClass)
{
	QcEffect* pRet = NULL;
	QcEffectMap::iterator iter(m_effectMap.find(psEffectName));
	if (iter != m_effectMap.end())
	{
		if (pRunningClass == NULL)
			pRunningClass = iter->second.m_pRunClass;
		pRet = pRunningClass->CreateObject<QcEffect>();
		pRet->Init(iter->second.m_pEffectInstance);
	}
	return pRet;
}

void QcEffectMgr::FlipRt()
{
    m_effectRt.Flip();
}

LPDIRECT3DTEXTURE9 QcEffectMgr::GetPrevTarget()
{
    return m_effectRt.GetPrevTarget();
}

LPDIRECT3DTEXTURE9 QcEffectMgr::GetPrevSource()
{
    return m_effectRt.GetPrevSource();
}

LPDIRECT3DTEXTURE9 QcEffectMgr::GetNextTarget()
{
    return m_effectRt.GetNextTarget();
}

LPDIRECT3DTEXTURE9 QcEffectMgr::GetNextSource()
{
    return m_effectRt.GetNextSource();
}

bool QcEffectMgr::IsFirstRender()
{
    return m_effectRt.m_bFirstRender;
}

void QcEffectMgr::SetFirstRender( bool val )
{
    m_effectRt.m_bFirstRender = val;
}

LPDIRECT3DTEXTURE9 QcEffectMgr::GetRenderBack()
{
    return m_pRenderBack;
}


QcEffectMgr::CRenderTargetChain::CRenderTargetChain() : m_nNext( 0 ),
m_bFirstRender( true )
{
    ZeroMemory( m_pRenderTarget, sizeof( m_pRenderTarget ) );
}

QcEffectMgr::CRenderTargetChain::~CRenderTargetChain()
{
    Cleanup();
}

void QcEffectMgr::CRenderTargetChain::Cleanup()
{
    SAFE_RELEASE( m_pRenderTarget[0] );
    SAFE_RELEASE( m_pRenderTarget[1] );
}

void QcEffectMgr::CRenderTargetChain::Flip()
{
    m_nNext = 1 - m_nNext;
}

LPDIRECT3DTEXTURE9 QcEffectMgr::CRenderTargetChain::GetPrevTarget()
{
    return m_pRenderTarget[1 - m_nNext];
}

LPDIRECT3DTEXTURE9 QcEffectMgr::CRenderTargetChain::GetPrevSource()
{
    return m_pRenderTarget[m_nNext];
}

LPDIRECT3DTEXTURE9 QcEffectMgr::CRenderTargetChain::GetNextTarget()
{
    return m_pRenderTarget[m_nNext];
}

LPDIRECT3DTEXTURE9 QcEffectMgr::CRenderTargetChain::GetNextSource()
{
    return m_pRenderTarget[1 - m_nNext];
}
