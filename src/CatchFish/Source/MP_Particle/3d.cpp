//-----------------------------------------------------------------------------
// File: 3d.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "Pch.hpp"
#include "3d.h"

d3d::d3d()
{
	ZeroMemory(this, sizeof(*this));
}

d3d::~d3d()
{
	
}

bool d3d::Init(LPDIRECT3DDEVICE9 pDevice)
{
	if (!pDevice)
		return false;

	device_3d = pDevice;
	
	UINT a, b;
	pDevice->GetStreamSource(0, &myVertexBack, &a, &b);
	pDevice->GetIndices(&myIndexBack);

	InitBuffer();			

	return true;
}

void d3d::Clear()
{
	DestroyBuffer();
}

void d3d::InitBuffer()
{
	vertex_buffer = NULL;
	index_buffer = NULL;
#ifndef QmDynamicVertex2
	int buffer_vertex_size=MAX_SEGMENT*SIZE_SEGMENT;
	device_3d->CreateVertexBuffer(buffer_vertex_size*4*sizeof(VERTEX_PARTICLE),D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,FVF_PARTICLE,D3DPOOL_DEFAULT,&vertex_buffer,NULL);
	int buffer_index_size=SIZE_SEGMENT;
	device_3d->CreateIndexBuffer(buffer_index_size*6*sizeof(WORD),D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,D3DPOOL_DEFAULT,&index_buffer, NULL);

	WORD* index;
	index_buffer->Lock(0,buffer_index_size*6*sizeof(WORD),(void**)&index,0);
	for (int i=0;i<buffer_index_size;i++)
	{
		int vertex=i*4;
		WORD* ind=&(index[i*6]);
		*ind=(WORD)vertex;
		ind++;
		*ind=(WORD)vertex+1;
		ind++;
		*ind=(WORD)vertex+2;
		ind++;
		*ind=(WORD)vertex+3;
		ind++;
		*ind=(WORD)vertex;
		ind++;
		*ind=(WORD)vertex+2;
		ind++;
	}
	index_buffer->Unlock();
#endif
}

void d3d::DestroyBuffer()
{
	if (vertex_buffer)
	{
		vertex_buffer->Release();
		vertex_buffer=NULL;
	}
	if (index_buffer)
	{
		index_buffer->Release();
		index_buffer=NULL;
	}
	if (myVertexBack)
	{
		myVertexBack->Release();
		myVertexBack=NULL;
	}
	if (myIndexBack)
	{
		myIndexBack->Release();
		myIndexBack=NULL;
	}
}

d3d d3d9;