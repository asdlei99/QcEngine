//-----------------------------------------------------------------------------
// File: mp_wrap.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "pch.hpp"
#include "mp_wrap.h"

#pragma warning ( disable : 4996)

extern d3d d3d9;

int MP_Frame_WRAP::gsOffset=0;
int MP_Frame_WRAP::k_particles=0;
VERTEX_PARTICLE* MP_Frame_WRAP::vertex=NULL;
static DWORD gsRenderFlag = 0;
static DWORD gsRS_DestBlend = 0;
static DWORD gsRS_Lighting = 0;
static DWORD gsRS_Stencil = 0;
static DWORD gsTSS_ARG1 = 0;
static DWORD gsTSS_ARG2 = 0;
static DWORD gsTSS_ALP1 = 0;
static DWORD gsTSS_ALP2 = 0;
static DWORD gsTSS_TTF = 0;
static DWORD gsSAM_MAG = 0;
static DWORD gsSAM_MIN = 0;
static IDirect3DBaseTexture9* gsSaveTexture = NULL;
static IDirect3DBaseTexture9* gsCurrentTexture = NULL;
#ifdef QmDynamicVertex2
static VERTEX_PARTICLE particeVertexBuf[SIZE_SEGMENT * 6] = {0};
#else
static VERTEX_PARTICLE particeVertexBuf[SIZE_SEGMENT * 4] = {0};
#endif


MP_Frame_WRAP::MP_Frame_WRAP(MP_FrameList* owner) : MP_Frame(owner)
{
};

MP_Frame_WRAP::~MP_Frame_WRAP()
{
}

// eng: Initialization of texture frame

MP_Atlas*  MP_Frame_WRAP::InitFrame(MAGIC_TEXTURE* magic_texture)
{
	return MP_Frame::InitFrame(magic_texture);
}

// eng: Drawing the particle

void MP_Frame_WRAP::Draw(MAGIC_PARTICLE* magic_particle)
{
	/*LPDIRECT3DDEVICE9 device_3d=d3d9.GetDevice3D();*/

	MAGIC_VERTEX_RECTANGLE* vertex_rectangle=Magic_GetParticleRectangle(magic_particle,magic_texture);

	unsigned long color=magic_particle->color;

	VERTEX_PARTICLE* pBase = vertex;

	vertex->x=vertex_rectangle->x1;
	vertex->y=vertex_rectangle->y1;
	vertex->u=magic_texture->left;
	vertex->v=magic_texture->top;
	vertex->color=color;
	vertex->z=0;
	vertex->rhw=1.0f;
	++vertex;

	vertex->x=vertex_rectangle->x2;
	vertex->y=vertex_rectangle->y2;
	vertex->u=magic_texture->right;
	vertex->v=magic_texture->top;
	vertex->color=color;
	vertex->z=0;
	vertex->rhw=1.0f;
	++vertex;

	vertex->x=vertex_rectangle->x3;
	vertex->y=vertex_rectangle->y3;
	vertex->u=magic_texture->right;
	vertex->v=magic_texture->bottom;
	vertex->color=color;
	vertex->z=0;
	vertex->rhw=1.0f;
	++vertex;

	vertex->x=vertex_rectangle->x4;
	vertex->y=vertex_rectangle->y4;
	vertex->u=magic_texture->left;
	vertex->v=magic_texture->bottom;
	vertex->color=color;
	vertex->z=0;
	vertex->rhw=1.0f;
	++vertex;

#ifdef QmDynamicVertex2
	*vertex = *pBase;
	++vertex;

	*vertex = *(pBase + 2);
	++vertex;
#endif

	++k_particles;

	if (k_particles>=SIZE_SEGMENT)
	{
		DrawSegment();
	}
};

// eng: Sets the intensity

void MP_Frame_WRAP::SetIntense(bool intense)
{
};


// eng: The beginning of the particle's type drawing
void MP_Frame_WRAP::BeginDraw()
{
	MP_Frame::BeginDraw();

	LPDIRECT3DDEVICE9 device_3d=d3d9.GetDevice3D();
	if (Magic_IsIntensive())
	{
		device_3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
	else
	{
		device_3d->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	unsigned int texture_id=owner->GetTextureID();
	MP_Atlas_WRAP* atlas=(MP_Atlas_WRAP*)owner->GetOwner()->GetAtlas(texture_id);
	LPDIRECT3DTEXTURE9 texture=atlas->GetTexture();
	if (texture != gsCurrentTexture)
	{
		device_3d->SetTexture(0, texture);
		gsCurrentTexture = texture;
	}
	gsRenderFlag = 2;
}

void MP_Frame_WRAP::EndDraw()
{
	DrawSegment();
}


// eng: Drawing the segment
void MP_Frame_WRAP::DrawSegment()
{
	if (k_particles)
	{
#ifdef QmDynamicVertex2
		LPDIRECT3DDEVICE9 device_3d=d3d9.GetDevice3D();
		device_3d->DrawPrimitiveUP(D3DPT_TRIANGLELIST, k_particles*2, particeVertexBuf, sizeof(VERTEX_PARTICLE));
#else
		LPDIRECT3DDEVICE9 device_3d=d3d9.GetDevice3D();
		LPDIRECT3DVERTEXBUFFER9 vertex_buffer=d3d9.GetVetexBuffer();
		void* pVertexBuffer = NULL;
		int nVertex = 4 * k_particles;
		int nSize = nVertex * sizeof(VERTEX_PARTICLE);
		HRESULT hr = S_OK;

		{
			if (gsOffset == 0 || gsOffset + nVertex > MAX_SEGMENT * SIZE_SEGMENT * 4)
			{
				gsOffset = 0;
				hr = vertex_buffer->Lock(gsOffset, nSize ,&pVertexBuffer, D3DLOCK_DISCARD);
				QmAssert(hr == S_OK);
			}
			else
			{
				hr = vertex_buffer->Lock(gsOffset * sizeof(VERTEX_PARTICLE), nSize ,&pVertexBuffer, D3DLOCK_NOOVERWRITE);
				QmAssert(hr == S_OK);
			}
			QmAssert(nSize < sizeof(particeVertexBuf));
			memcpy(pVertexBuffer, particeVertexBuf, nSize);
			hr = vertex_buffer->Unlock();
			QmAssert(hr == S_OK);
		}
		{
			hr = device_3d->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,gsOffset,0, nVertex,0, k_particles * 2);
		}
		QmAssert(hr == S_OK);

		gsOffset += nVertex;
#endif
		k_particles=0;
		vertex = &particeVertexBuf[0];
	}
}

void MP_Frame_WRAP::SaveState()
{
	LPDIRECT3DDEVICE9 device_3d=d3d9.GetDevice3D();

	LPDIRECT3DVERTEXBUFFER9 vertex_buffer=d3d9.GetVetexBuffer();
	LPDIRECT3DINDEXBUFFER9 index_buffer=d3d9.GetIndexBuffer();

	device_3d->GetRenderState(D3DRS_DESTBLEND, &gsRS_DestBlend);
    device_3d->GetRenderState(D3DRS_STENCILENABLE, &gsRS_Stencil);
    device_3d->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    device_3d->GetRenderState(D3DRS_LIGHTING, &gsRS_Lighting);
    device_3d->SetRenderState(D3DRS_LIGHTING, FALSE);
    device_3d->GetTextureStageState(0, D3DTSS_ALPHAARG1, &gsTSS_ALP1);
    device_3d->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    device_3d->GetTextureStageState(0, D3DTSS_ALPHAARG2, &gsTSS_ALP2);
    device_3d->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    device_3d->GetTextureStageState(0, D3DTSS_COLORARG1, &gsTSS_ARG1);
    device_3d->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    device_3d->GetTextureStageState(0, D3DTSS_COLORARG2, &gsTSS_ARG2);
    device_3d->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    device_3d->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, &gsTSS_TTF);
    device_3d->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	device_3d->SetVertexShader(NULL);
	device_3d->SetFVF(FVF_PARTICLE);
	device_3d->SetStreamSource(0, vertex_buffer, 0,sizeof(VERTEX_PARTICLE));
	device_3d->SetIndices(index_buffer);
	device_3d->GetTexture(0, &gsSaveTexture);
	gsCurrentTexture = gsSaveTexture;

	gsRenderFlag = 1;
	k_particles=0;
	vertex = &particeVertexBuf[0];
}
void MP_Frame_WRAP::RecoverState()
{
	LPDIRECT3DDEVICE9 device_3d=d3d9.GetDevice3D();
	LPDIRECT3DVERTEXBUFFER9 vertex_buffer=d3d9.GetVertexBufferBack();
	LPDIRECT3DINDEXBUFFER9 index_buffer=d3d9.GetIndexBufferBack();
	device_3d->SetStreamSource(0, vertex_buffer, 0, sizeof(QsVertex));
	device_3d->SetIndices(index_buffer);
	device_3d->SetFVF(QsVertex::FVF);
	device_3d->SetRenderState(D3DRS_DESTBLEND, gsRS_DestBlend);
    device_3d->SetRenderState(D3DRS_STENCILENABLE, gsRS_Stencil);
    device_3d->SetTexture(0, gsSaveTexture);    
    device_3d->SetRenderState(D3DRS_LIGHTING, gsRS_Lighting);
    device_3d->SetTextureStageState(0, D3DTSS_ALPHAARG1, gsTSS_ALP1);
    device_3d->SetTextureStageState(0, D3DTSS_ALPHAARG2, gsTSS_ALP2);
    device_3d->SetTextureStageState(0, D3DTSS_COLORARG1, gsTSS_ARG1);
    device_3d->SetTextureStageState(0, D3DTSS_COLORARG2, gsTSS_ARG2);
    device_3d->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, gsTSS_TTF);

	if (gsSaveTexture)
	{
		gsSaveTexture->Release();
	}
}

// ----------------------------------------------------------------------------------------------------

VOID WINAPI TextureFill(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, const D3DXVECTOR2* pTexelSize, LPVOID pData)
{
	*pOut=D3DXVECTOR4(0,0,0,0.0f);
}

// eng: This class stores texture atlas

MP_Atlas_WRAP::MP_Atlas_WRAP(int width, int height) : MP_Atlas(width,height)
{
	LPDIRECT3DDEVICE9 device_3d=d3d9.GetDevice3D();

	device_3d->CreateTexture(width,height,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,&texture,NULL);

	// eng: Cleaning the texture
	D3DXFillTexture(texture, TextureFill, NULL); 
}

MP_Atlas_WRAP::~MP_Atlas_WRAP()
{
	if (texture)
	{
		texture->Release();
		texture=NULL;
	}
}

// eng: Loads texture frame

void MP_Atlas_WRAP::LoadFrame(MAGIC_TEXTURE* magic_texture, const char* texture_folder)
{
	LPDIRECT3DDEVICE9 device_3d=d3d9.GetDevice3D();

    D3DXMATRIX Mat;
    device_3d->GetTransform(D3DTS_WORLD, &Mat);

	IDirect3DSurface9* dest=NULL;
	texture->GetSurfaceLevel(0,&dest);

	IDirect3DSurface9* back=NULL;
	HRESULT hr=device_3d->GetRenderTarget(0,&back);

	hr=device_3d->SetRenderTarget(0,dest);

	device_3d->BeginScene();

	LPDIRECT3DTEXTURE9 texture_frame=NULL;

	D3DXIMAGE_INFO info;
	if (magic_texture->data)
	{
		// eng: Loads from the memory
		
		hr=D3DXCreateTextureFromFileInMemoryEx(device_3d,magic_texture->data,magic_texture->length,0,0,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,D3DX_FILTER_NONE,D3DX_DEFAULT,0,&info,NULL,&texture_frame);
	}
	else
	{
		// eng: Loads from the file
		
		MP_String file=texture_folder;
		file+=magic_texture->file;
		hr=D3DXCreateTextureFromFileExA(device_3d,file.c_str(),0,0,D3DX_DEFAULT,0,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,D3DX_FILTER_NONE,D3DX_DEFAULT,0,&info,NULL,&texture_frame);
	}

	ID3DXSprite* sprite=NULL;

	D3DXCreateSprite(device_3d, &sprite);

	RECT rect_from;
	rect_from.left=0;
	rect_from.top=0;
	rect_from.right=info.Width;
	rect_from.bottom=info.Height;

	int frame_wi=magic_texture->frame_width;
	int frame_he=magic_texture->frame_height;

	float scale_x=((float)frame_wi)/((float)info.Width);
	float scale_y=((float)frame_he)/((float)info.Height);

	D3DXMATRIX scale_matrix;
	D3DXMatrixScaling(&scale_matrix,scale_x,scale_y,1);
	sprite->SetTransform(&scale_matrix);

	float left=magic_texture->left;
	if (left>magic_texture->right)
		left=magic_texture->right;
	float top=magic_texture->top;
	if (top>magic_texture->bottom)
		top=magic_texture->bottom;
	int x=(int)(left*atlas_width);
	int y=(int)(top*atlas_height);

	D3DXVECTOR3 vec3(((float)x)/scale_x, ((float)y)/scale_y, 0.f);
	sprite->Begin(0);
	hr=sprite->Draw(texture_frame,
	&rect_from,
	NULL,
	&vec3, 0xffffffff);
		
	sprite->End();

	sprite->Release();
	sprite=NULL;

	texture_frame->Release();
	texture_frame=NULL;

	dest->Release();

	device_3d->SetRenderTarget(0,back);

	back->Release();

	device_3d->EndScene();
}
