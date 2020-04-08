#pragma once

#include "Common\QcEncryptXmlLoader.hpp"
class QcEffectMgr : public QcEncryptXmlLoader
{
	struct QsEffectInfo
	{
		ID3DXEffect* m_pEffectInstance;
		QcRuntimeClass* m_pRunClass;
	};
	typedef std::map<QcString,QsEffectInfo, QcNoCaseCompare> QcEffectMap;
public:
	QcEffectMgr();
	virtual ~QcEffectMgr();

	void Init(const char* psFile, QcDeviceRender& render);
	QcEffect* MakeEffect(const char* psEffectName, QcRuntimeClass* pRunningClass = NULL);

    void  FlipRt();

    LPDIRECT3DTEXTURE9  GetPrevTarget();
    LPDIRECT3DTEXTURE9  GetPrevSource();
    LPDIRECT3DTEXTURE9  GetNextTarget();
    LPDIRECT3DTEXTURE9  GetNextSource();

    LPDIRECT3DTEXTURE9  GetRenderBack();

    bool  IsFirstRender();
    void  SetFirstRender(bool val);

protected:
	virtual void Parse(const QcXmlNodeIterator& rootNode, void* pParam);
	virtual ID3DXEffect* LoadEffect(const char* psFile, QcDeviceRender& render);
protected:
	QcEffectMap m_effectMap;
    struct CRenderTargetChain
    {
        int m_nNext;
        bool m_bFirstRender;
        LPDIRECT3DTEXTURE9  m_pRenderTarget[2];

    public:
        CRenderTargetChain();

        ~CRenderTargetChain();

        void  Cleanup();

        void  Flip();

        LPDIRECT3DTEXTURE9  GetPrevTarget();
        LPDIRECT3DTEXTURE9  GetPrevSource();
        LPDIRECT3DTEXTURE9  GetNextTarget();
        LPDIRECT3DTEXTURE9  GetNextSource();
    }m_effectRt;
    
    LPDIRECT3DTEXTURE9      m_pRenderBack;
};