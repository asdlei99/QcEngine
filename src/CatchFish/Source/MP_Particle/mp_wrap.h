//-----------------------------------------------------------------------------
// File: mp_wrap.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_DX
#define MAGIC_PARTICLES_DX

#include "mp.h"
#include "3d.h"

// eng: This class stores texture frame-file.
class MP_Frame_WRAP : public MP_Frame
{
protected:
	// eng: The current segment of the vertex buffer
	static int gsOffset;	
	// eng: The number of particles loaded in the segment
	static int k_particles;
	// eng: The next particle in the segment
	static VERTEX_PARTICLE* vertex;	
	// eng: Intensity
public:
	MP_Frame_WRAP(MP_FrameList* owner);
	virtual ~MP_Frame_WRAP();

	static void SaveState();
	static void RecoverState();

    // eng: Initialization of texture frame

	virtual MP_Atlas* InitFrame(MAGIC_TEXTURE* magic_texture);

    // eng: Drawing the particle

	virtual void Draw(MAGIC_PARTICLE* magic_particle);

    // eng: The beginning of the particle's type drawing

	virtual void BeginDraw();

    // eng: The end of the particle's type drawing

	virtual void EndDraw();

    // eng: Sets the intensity

	virtual void SetIntense(bool intense);

    // eng: Drawing the segment

	static void DrawSegment();
};

// eng: This class stores texture atlas.

class MP_Atlas_WRAP : public MP_Atlas
{
protected:
	LPDIRECT3DTEXTURE9 texture;
	
public:
	MP_Atlas_WRAP(int width, int height);
	virtual ~MP_Atlas_WRAP();

    // eng: Loads texture frame

	void LoadFrame(MAGIC_TEXTURE* magic_texture, const char* texture_folder);

    // eng: Returns the texture

	LPDIRECT3DTEXTURE9 GetTexture(){return texture;}
};


#endif