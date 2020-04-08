//-----------------------------------------------------------------------------
// File: mp.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES
#define MAGIC_PARTICLES

#include "mp_string.h"
#include "magic.h"

class MP_Emitter;
class MP_FrameList;
class MP_Frame;
class MP_Manager;
class MP_Atlas;


// eng: Interpolation mode is taken from emitter settings 
#define MAGIC_INTERPOLATION_DEFAULT		0

// eng: Interpolation is always enabled	 
#define MAGIC_INTERPOLATION_ENABLE		1	

// eng: Interpolation is always disabled
#define MAGIC_INTERPOLATION_DISABLE		2

// eng: Preserve particle positions when changing emitter position or direction 
#define MAGIC_CHANGE_EMITTER_ONLY			0

// eng: Move all the special effect when changing emitter position or direction 
#define MAGIC_CHANGE_EMITTER_AND_PARTICLES	1	

// eng: This class is used to store all the loaded emitters 
// 加入Singleton模板
class MP_Manager /*: public Singleton<MP_Manager>*/
{
protected:
	// eng: The count of loaded emitters
	int k_emitter;
	// eng: The list of loaded emitters
	MP_Emitter** m_emitter;
	// eng: The count of loaded frame-lists
	int k_framelist;
	// eng: The list of loaded frame-lists
	MP_FrameList** m_framelist;
	// eng: The count of loaded atlases
	int k_atlas;
	// eng: The list of loaded atlases
	MP_Atlas** m_atlas;

    // eng: The settings for default initialization of emitters
	
	// eng: Interpolation mode, that will be assigned to all loaded emitters.
	int interpolation;
	// eng: Mode of the emitter behaviour after the end of animation
	int loop;
	// eng: Emitter coordinates mode.
	bool position_mode;	
	// eng: Path to the folder that will be used to store textures. 
	MP_String texture_folder;
	// eng: Path to the folder that will be used to store temporary data. 
	MP_String temp_folder;

public:
	MP_Manager();
	virtual ~MP_Manager();

    // eng: Initialization. 
	void Initialization(int interpolation, int loop, bool position_mode, const char* temp_folder=NULL);

    // eng: Returns the number of emitters. 
	int GetEmitterCount(){return k_emitter;}
	
	// eng: Returns the emitter by its index. 
	MP_Emitter* GetEmitter(int index);
	// eng: Returns the emitter by name. 
	MP_Emitter* GetEmitterByName(const char* name);
	// 上面的是获取第一个同名的Emitter，下面再加一个，获取空闲的该名字的Eimtter，如果没有空闲，就复制一个
	MP_Emitter*	GetFreeEmitterByName(const char* name);

	// eng: Returns the count of frame-lists
	int GetFrameListCount(){return k_framelist;}
	// eng: Returns the frame-list by its index.
	MP_FrameList* GetFrameList(int index);
	// eng: Adds the new frame-list.
	MP_FrameList* AddFrameList();

    // eng: Loading all the emitters and animated folders from the file specified. 
	int LoadEmittersFromFile(const char* file, bool isMemFile = false);

    // eng: Closing files. 
	void CloseFiles();

    // eng: Loading textures for the existing emitters. 
	void LoadTextures(const char* texture_folder, int atals_width, int atlas_height, int frame_step=0, float scale_step=0.1f);

    // eng: Duplicating specified emitter. 
	int DuplicateEmitter(MP_Emitter* from);

    // eng: Emitters update. 
	void Update(double time);

	void Render();

    // eng: Stopping all the emitters. 
	void Stop();

    // eng: Loads folder
	void LoadFolder(HM_FILE file, const char* path);

    // eng: Loads specified emitter.
	void LoadEmitter(HM_FILE file, const char* path);

    // eng: Returns temp folder
	const char* GetTempFolder(){return temp_folder.c_str();}

    // eng: Returns texture's folder
	const char* GetTextureFolder(){return texture_folder.c_str();}

    // eng: Adds frame to the atlas
	MP_Atlas* AddFrameToAtlas(int index, MAGIC_TEXTURE* magic_texture);

    // eng: Returns texture atlas
	MP_Atlas* GetAtlas(int index){return m_atlas[index];}

protected:
    // eng: Adds new emitter
	void AddEmitter(MP_Emitter* emitter);
};

struct MP_POSITION
{
	float x,y,z;

	MP_POSITION(){x=0.f;y=0.f;z=0.f;}
	MP_POSITION(float x,float y,float z){this->x=x;this->y=y;this->z=z;}
	MP_POSITION(float x,float y){this->x=x;this->y=y;z=0.f;}

	MP_POSITION operator+(const MP_POSITION& pos)
	{
		x+=pos.x;
		y+=pos.y;
		z+=pos.z;
		return *this;
	}

	MP_POSITION operator+=(const MP_POSITION& pos)
	{
		*this=*this+pos;
		return *this;
	}
};

// eng: Emitter is not working 
#define MAGIC_STATE_STOP		0	
#define MAGIC_STATE_UPDATE		1	
// eng: Emitter interrupts, i.e. is working while there are "alive" particles 
#define MAGIC_STATE_INTERRUPT	2	
// eng: Emitter is only rendered 
#define MAGIC_STATE_VISIBLE		3	

// eng: Class for work with the emitters
class MP_Emitter
{
protected:
    // eng: Emitter's state. 
	int state;
    // eng: Manager
	MP_Manager* owner;	
    // eng: Emitter's descriptor
	HM_EMITTER emitter;
	
    // eng: Z-coordinate of the emitter 
	float z;					

    // eng: Is it first restart?
	bool first_restart;			

    // eng: The file with copy
	MP_String copy_file;		

    // eng: The count of saved copyes
	static int copy_count;	

	bool	link;
	bool	use_user_color;
	unsigned	user_color;	

public:
	MP_Emitter(HM_EMITTER emitter, MP_Manager* owner);
	virtual ~MP_Emitter();

    // eng: Operator "="
	MP_Emitter& operator=(const MP_Emitter& from);

    // eng: Returns the descriptor of the emitter to work with API. 
	HM_EMITTER GetEmitter(){return emitter;}

    // eng: Returns the name of the emitter. 
	const char* GetEmitterName();

    // eng: Restarts the emitter
	void Restart();

    // eng: Returns and sets the emitter position. 
	void GetPosition(MP_POSITION& position);
	void SetPosition(const MP_POSITION& position);

    // eng: Moving the emitter to the position specified allowing restart. 
	void Move(MP_POSITION& position, bool restart=false);

    // eng: Offsetting the current emitter position by the value specified. 
	void Offset(const MP_POSITION& offset);

    // eng: Returns abd sets emitter's emission direction. 
	void SetDirection(float angle);
	float GetDirection();

    // eng: Setting the emitter direction to the specified value with the restart ability. 
	void Direct(float angle, bool restart=false);

    // eng: Rotation of the emitter by the specified value. 
	void Rotate(float offset);

    // eng: Returns and sets the scale of the emitter. 
	void SetScale(float scale);
	float GetScale();

    // eng: Returns and sets the state of the emitter. 
	int GetState(){return state;}
	void SetState(int state);

    // eng: Emitter update. 
	void Update(double time);

    // eng: Emitter visualization. Returns the count of rendered particles
	int Render();

    // eng: Loads textures for emitter
	void LoadTextures();

	bool IsLink(){return link;}
	void Link(bool flag){link = flag;}

	void UseUserColor(bool flag){use_user_color = flag;}
	void SetUserColor(unsigned color){user_color = color & 0xffffff;}
};


// eng: This class stores texture frame-files for one particle type
class MP_FrameList
{
protected:
	MP_Manager* owner;

	int k_frame;
	MP_Frame** m_frame;

	unsigned int texture_id;

public:
	MP_FrameList(MP_Manager* owner);
	virtual ~MP_FrameList();

    // eng: Returns owner
	MP_Manager* GetOwner(){return owner;}

    // eng: Returns the count of frames in the texture
	int GetFrameCount(){return k_frame;}

    // eng: Returns the frame by its index.
	MP_Frame* GetFrame(int index);

    // eng: Adds the frame
	void AddFrame(MP_Frame* frame);

    // eng: Sets texture's ID.
	void SetTextureID(unsigned int id){texture_id=id;}
    // eng: Returns texture's ID.
	unsigned int GetTextureID(){return texture_id;}
};

// eng: This class stores texture atlas. This class is abstract
class MP_Atlas
{
protected:
	int atlas_width, atlas_height;

public:
	MP_Atlas(int width, int height){atlas_width=width; atlas_height=height;}
	virtual ~MP_Atlas(){}
 
    // eng: Loads texture frame
	virtual void LoadFrame(MAGIC_TEXTURE* magic_texture, const char* texture_folder)=NULL;

    // eng: Returns size
	void GetSize(int& atlas_width,int& atlas_height){atlas_width=this->atlas_width;atlas_height=this->atlas_height;}
};

// eng: This class stores texture frame-file. This class is abstract
class MP_Frame
{
protected:
	MP_FrameList* owner;

	MAGIC_TEXTURE* magic_texture;

public:
	MP_Frame(MP_FrameList* owner){this->owner=owner;}
	virtual ~MP_Frame(){}

    // eng: Returns owner
	MP_FrameList* GetOwner(){return owner;}

    // eng: Initialization of texture frame
	virtual MP_Atlas* InitFrame(MAGIC_TEXTURE* magic_texture);

    // eng: The beginning of the particle's type drawing
	virtual void BeginDraw(){}

    // eng: The end of the particle's type drawing
	virtual void EndDraw(){}
	
    // eng: Drawing the particle
	virtual void Draw(MAGIC_PARTICLE* magic_particle)=0;
	
    // eng: Sets the intensity
	virtual void SetIntense(bool intense)=0;
};

#endif
