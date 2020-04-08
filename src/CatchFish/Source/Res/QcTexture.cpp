#include "pch.hpp"
#include "QcTexture.hpp"
#include "QcResPackLoader.hpp"
#include "Common\QcEncryptFileBuf.hpp"

QcTexture::QcTexture(const QsImageInfo& infor)
: m_textureInfo(infor), m_fWidth(0.f), m_fHeight(0.f), m_bUpdate(false)
{
}

QcTexture::~QcTexture()
{
}

bool QcTexture::Create(IDirect3DDevice9* pDevice)
{
	bool bRet = false;
	
	QcEncryptFileBuf imageBuf;
	QcPackFileHandle iHandle = QmInvaidHandle;
	if ( (iHandle = imageBuf.Load(m_textureInfo.m_pTextureInstance->m_sFileName.c_str())) != 0)
	{
		if (QmInvaidHandle != iHandle)
		{
			u32 dataWidth = 0;
			u32 dataHeight = 0;
			u32 iPos;
			u32 iLen;
			int iEn;
			QmGlobalData.m_pFilePacker->GetPackInfo((iHandle - 1), iPos, iLen, iEn, &dataWidth, &dataHeight);
			u32 maxLen = dataWidth * dataHeight * 4;
			if (iEn == 1)
			{
				char* pDestData = new char[maxLen];
				{
					QmProfilerLoading(eUncompressTime);
					if (!UncompressData(pDestData, (unsigned long*)&maxLen, (char*)imageBuf.pointer(), imageBuf.size()))
					{
						QmAssert(false);
					}
				}
				bRet = Create(pDevice, dataWidth, dataHeight, pDestData);
				delete [] pDestData;
			}
			else if (iEn == -1)
			{
				bRet = Create(pDevice, dataWidth, dataHeight, (char*)imageBuf.pointer());
			}
		}
		if (!bRet)
		{
			char* pData = NULL;
			unsigned long dataWidth = 0;
			unsigned long dataHeight = 0;
			{
				bRet = ReadImage(pData, dataWidth, dataHeight, imageBuf.pointer(), imageBuf.size());
			}
			if (bRet)
			{
				bRet = Create(pDevice, dataWidth, dataHeight, pData);
				DestroyBitsData(pData);
			}
		}
	}
	return bRet;
}

bool QcTexture::Create(IDirect3DDevice9* pDevice, u32 width, u32 height, const char* pData)
{
	IDirect3DTexture9* pDest = NULL;
	HRESULT hr;
	u32 iBits = height * width * 4;
	QmResLocker();
	QmProfilerLoading(eTextureTime);
	//place large texture to device texture memory.
	if (QmGlobalData.giForceTexInVideoMem >= 0)
	{
		if ( (QmGlobalData.giForceTexInVideoMem > 0 || iBits >= 256 * 256 * 4) && pDevice->GetAvailableTextureMem() - iBits >= 1024 * 1024 * 128)
		{
			hr = D3DXCreateTexture(pDevice, width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pDest);
			QmAssert(hr == S_OK);
		}
	}
#ifdef QmTestVersion
	QmDebugHelper.CalTexMem(iBits, QmGlobalData.giForceTexInVideoMem >= 0 && pDest == NULL);
#endif
	{		
		while( (hr = D3DXCreateTexture(pDevice, width, height, 1, 0, D3DFMT_A8R8G8B8, pDest == NULL ? D3DPOOL_MANAGED : D3DPOOL_SYSTEMMEM, &m_textureInfo.m_pTextureInstance->m_pTexture) ) != S_OK)
		{
			switch(hr)
			{
			case D3DERR_NOTAVAILABLE:
				{
					QmLogError("D3DXCreateTextureFromFileInMemoryEx:D3DERR_NOTAVAILABLE");
					break;
				}
			case D3DERR_OUTOFVIDEOMEMORY:
				{
					QmLogError("D3DXCreateTextureFromFileInMemoryEx:D3DERR_OUTOFVIDEOMEMORY");
					break;
				}
			case D3DERR_INVALIDCALL:
				{
					QmLogError("D3DXCreateTextureFromFileInMemoryEx:D3DERR_INVALIDCALL");
					break;
				}
			case D3DXERR_INVALIDDATA:
				{
					QmLogError("D3DXCreateTextureFromFileInMemoryEx:D3DXERR_INVALIDDATA");
					break;
				}
			case E_OUTOFMEMORY:
				{
					QmLog("D3DXCreateTextureFromFileInMemoryEx:E_OUTOFMEMORY");
					break;
				}
			default:
				QmLogError("fail to CreateTexture");
				break;
			}
		}
	}
	if (m_textureInfo.m_pTextureInstance->m_pTexture != NULL)
	{
		D3DLOCKED_RECT rc;
		HRESULT hr = m_textureInfo.m_pTextureInstance->m_pTexture->LockRect(0, &rc, NULL, 0);
		if (hr != S_OK)
		{
			QmAssert(false);
			QmLog("Lock TextureRect fail");
		}
		memcpy(rc.pBits, pData, iBits);
		m_textureInfo.m_pTextureInstance->m_pTexture->UnlockRect(0);

		m_textureInfo.m_pTextureInstance->m_fSurfaceWidth = (f32)width;
		m_textureInfo.m_pTextureInstance->m_fSurfaceHeight = (f32)height;

		m_textureInfo.fImgWidth = 1.0f;
		m_textureInfo.fImgHeight = 1.0f;

		m_bUpdate = false;
		Update();
	}
	if (pDest != NULL)
	{
		pDevice->UpdateTexture(m_textureInfo.m_pTextureInstance->m_pTexture, pDest);
		m_textureInfo.m_pTextureInstance->m_pTexture->Release();
		m_textureInfo.m_pTextureInstance->m_pTexture = pDest;
	}
	return true;
}
	