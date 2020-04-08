#pragma once

struct QsTextureInstance : public QcReferenceCounted
{
	QsTextureInstance(const QcString& sFile)
		: m_sFileName(sFile), m_pTexture(NULL), m_fSurfaceWidth(0.f), m_fSurfaceHeight(0.f)
	{
	}

	~QsTextureInstance()
	{
		if (m_pTexture)
		{
			m_pTexture->Release();
			m_pTexture = NULL;
		}
	}
	f32 m_fSurfaceWidth;
	f32 m_fSurfaceHeight;
	QcString m_sFileName;
	IDirect3DTexture9* m_pTexture;
};

struct QsImageInfo
{
	QsImageInfo() : fImgPosX(0.f), fImgPosY(0.f), fImgWidth(0.f), fImgHeight(0.f), m_fOffsetX(0.f), m_fOffsetY(0.f){}

	f32 fImgPosX;
	f32 fImgPosY;
	f32 fImgWidth;
	f32 fImgHeight;
	f32 m_fOffsetX;
	f32 m_fOffsetY;
	QcSharedPtr<QsTextureInstance> m_pTextureInstance;
};

class QcTexture : public QcObject
{
	friend class QcResManager;
public:
	explicit QcTexture(const QsImageInfo& infor);
	virtual ~QcTexture();

	bool Create(IDirect3DDevice9* pDevice);
	void Update()
	{
		if (!m_bUpdate)
		{
			m_fWidth = m_textureInfo.m_pTextureInstance->m_fSurfaceWidth * GetULen();
			m_fHeight = m_textureInfo.m_pTextureInstance->m_fSurfaceHeight * GetVLen();

			m_fPixelUnitX = 1.f/m_textureInfo.m_pTextureInstance->m_fSurfaceWidth;
			m_fPixelUnitY = 1.f/m_textureInfo.m_pTextureInstance->m_fSurfaceHeight;

			m_dispMat.MakeMatrix(GetOffsetX(), 0.f - GetOffsetY(), 0.f, GetWidth(), GetHeight());
			m_bUpdate = true;
		}
	}
	f32 GetWidth() const {return (f32)m_fWidth;}
	f32 GetHeight() const {return (f32)m_fHeight;}
	f32 GetULen() const {return m_textureInfo.fImgWidth;}
	f32 GetVLen() const {return m_textureInfo.fImgHeight;}
	f32 GetUPos() const {return m_textureInfo.fImgPosX;}
	f32 GetVPos() const {return m_textureInfo.fImgPosY;}
	f32 GetOffsetX() const {return m_textureInfo.m_fOffsetX;}
	f32 GetOffsetY() const {return m_textureInfo.m_fOffsetY;}
	f32 GetPixelUnitX() const {return m_fPixelUnitX; }
	f32 GetPixelUnitY() const {return m_fPixelUnitY;}
	IDirect3DTexture9* GetTexture() const {return m_textureInfo.m_pTextureInstance->m_pTexture;}
	const QcMatrix& GetDispMat() const {return m_dispMat;}
protected:
	const QcString& GetTexFileName() const {return m_textureInfo.m_pTextureInstance->m_sFileName;}
	bool Create(IDirect3DDevice9* pDevice, u32 width, u32 height, const char* pData);
private:
	bool m_bUpdate;
	f32 m_fWidth;
	f32 m_fHeight;
	f32 m_fPixelUnitX;
	f32 m_fPixelUnitY;
	QsImageInfo m_textureInfo;
	QcMatrix m_dispMat;
};