//-----------------------------------------------------------------------------
// File: 3d.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef D3D_COMPI
#define D3D_COMPI

#include <d3d9.h>
#include <d3dx9tex.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#include "magic.h"

#define MAX_SEGMENT	1
#define SIZE_SEGMENT 500
#define QmDynamicVertex2 

#define FVF_PARTICLE (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct VERTEX_PARTICLE
{
	// eng: Coordinates
	float x, y, z, rhw;	
	unsigned long color;
	// eng: Texture coordinates
	float u, v;			
};

class d3d
{
protected:
	LPDIRECT3DDEVICE9 device_3d;

	LPDIRECT3DVERTEXBUFFER9 vertex_buffer;
	LPDIRECT3DINDEXBUFFER9 index_buffer;

	LPDIRECT3DVERTEXBUFFER9 myVertexBack;
	LPDIRECT3DINDEXBUFFER9  myIndexBack;

public:
	d3d();
	virtual ~d3d();

	LPDIRECT3DDEVICE9 GetDevice3D(){return device_3d;}

	bool Init(LPDIRECT3DDEVICE9 pDevice);

	void Clear();

	LPDIRECT3DVERTEXBUFFER9 GetVetexBuffer(){return vertex_buffer;}
	LPDIRECT3DINDEXBUFFER9 GetIndexBuffer(){return index_buffer;}

	LPDIRECT3DVERTEXBUFFER9 GetVertexBufferBack(){return myVertexBack;}
	LPDIRECT3DINDEXBUFFER9 GetIndexBufferBack(){return myIndexBack;}

protected:
	void InitBuffer();
	void DestroyBuffer();
};

extern d3d d3d9;

#endif
