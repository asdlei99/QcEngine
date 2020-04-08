//-----------------------------------------------------------------------------
// File: mp.cpp
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#include "pch.hpp"
#include "MP_Particle/mp_wrap.h"

#pragma warning ( disable : 4996)

// ------------------------------------------------------------------------------------------

// eng: Adds the symbol "/" at the end of the path
void SetEndOfPath(MP_String& path)
{
	int len=(int)path.length();
	if (len)
	{
		char s=path[len-1];
		if (s!='/' && s!='\\')
			path+='/';
	}
}

// ------------------------------------------------------------------------------------------
// eng: This class is used to store all the loaded emitters 
MP_Manager::MP_Manager()
{
	// eng: The count of loaded emitters
	k_emitter=0;
	// eng: The list of loaded emitters
	m_emitter=NULL;
	// eng: The count of loaded frame-lists
	k_framelist=0;
	// eng: The list of loaded frame-lists
	m_framelist=NULL;
	// eng: The count of loaded atlases
	k_atlas=0;
	// eng: The list of loaded atlases
	m_atlas=NULL;

	// eng: The settings for default initialization of emitters

	// eng: Interpolation mode, that will be assigned to all loaded emitters.
	interpolation=MAGIC_INTERPOLATION_ENABLE;
	// eng: Mode of the emitter behaviour after the end of animation
	loop=MAGIC_LOOP ;
	// eng: Emitter coordinates mode.
	position_mode=MAGIC_CHANGE_EMITTER_ONLY;
}

MP_Manager::~MP_Manager()
{
	int i;
	for (i=0;i<k_emitter;i++)
	{
		delete m_emitter[i];
		m_emitter[i]=NULL;
	}

	if (m_emitter)
	{
		delete []m_emitter;
		m_emitter=NULL;
	}

	k_emitter=0;

	for (i=0;i<k_framelist;i++)
	{
		delete m_framelist[i];
		m_framelist[i]=NULL;
	}

	if (m_framelist)
	{
		delete []m_framelist;
		m_framelist=NULL;
	}

	k_framelist=0;


	for (i=0;i<k_atlas;i++)
	{
		delete m_atlas[i];
		m_atlas[i]=NULL;
	}

	if (m_atlas)
	{
		delete []m_atlas;
		m_atlas=NULL;
	}

	k_atlas=0;
}

// eng: Initialization. 
void MP_Manager::Initialization(int interpolation, int loop, bool position_mode, const char* temp_folder)
{
	this->interpolation=interpolation;
	this->loop=loop;
	this->position_mode=position_mode;

	if (temp_folder)
	{
		this->temp_folder=temp_folder;
		SetEndOfPath(this->temp_folder);
	}
}

// eng: Returns the emitter by its index. 
MP_Emitter* MP_Manager::GetEmitter(int index)
{
	if (index>=0 && index<k_emitter)
		return m_emitter[index];
	return NULL;
}

// eng: Returns the emitter by name. 
MP_Emitter* MP_Manager::GetEmitterByName(const char* name)
{
	MP_Emitter *org = 0;
	MP_Emitter *find = 0;
	for (int i=0;i<k_emitter;++i)
	{
		MP_Emitter &emitter=*GetEmitter(i);
		const char* emitter_name=emitter.GetEmitterName();
		if (!strcmp(name,emitter_name))
		{
			// eng: The names is equals
			if (!find)
				find = &emitter;
			if (!emitter.IsLink())
			{
				org = &emitter;
				break;
			}
		}
	}
	if (!org && find)
	{
		DuplicateEmitter(find);
		org = m_emitter[k_emitter-1];
	}

	return org;
}

MP_Emitter* MP_Manager::GetFreeEmitterByName(const char* name)
{
	MP_Emitter* resulte_e = 0;
	MP_Emitter* tmp_e = 0;
	for (int i=0; i<k_emitter; ++i)
	{
		tmp_e = GetEmitter(i);
		const char* e_name = tmp_e->GetEmitterName();
		if (!strcmp(name, e_name) )
		{
			if (tmp_e->GetState() == MAGIC_STATE_STOP)
			{
				// 找到空闲就返回
				return tmp_e;
			}
			// 未找到空闲则保存最后一个同名的Emitter
			resulte_e = tmp_e;
		}
	}
	// 复制出来并改为待机
	if (resulte_e)
	{
		resulte_e = GetEmitter(DuplicateEmitter(resulte_e));
		resulte_e->SetState(MAGIC_STATE_STOP);
	}
	return resulte_e;
}

// eng: Returns the frame-list by its index.
MP_FrameList* MP_Manager::GetFrameList(int index)
{
	if (index>=0 && index<k_framelist)
		return m_framelist[index];
	return NULL;
}

// eng: Loading all the emitters and animated folders from the file specified. 
int MP_Manager::LoadEmittersFromFile(const char* file, bool isMemFile)
{
	HM_FILE mf;
	int (*pFun)(const char*, HM_FILE*);
	pFun = isMemFile ? Magic_OpenFileInMemory : Magic_OpenFile;
	if ((*pFun)(file,&mf)==MAGIC_SUCCESS)
	{
		LoadFolder(mf,"");
		return MAGIC_SUCCESS;
	}
	return MAGIC_ERROR;
}

// eng: Closing files. 
void MP_Manager::CloseFiles()
{
	Magic_CloseAllFiles();
}

// eng: Loading textures for the existing emitters. 
void MP_Manager::LoadTextures(const char* texture_folder, int atals_width, int atlas_height, int frame_step, float scale_step)
{
	this->texture_folder="";
	if (texture_folder)
	{
		this->texture_folder=texture_folder;
		SetEndOfPath(this->texture_folder);
	}

	Magic_CreateAtlases(atals_width, atlas_height, frame_step, scale_step);

	for (int i=0;i<k_emitter;++i)
		m_emitter[i]->LoadTextures();
}

// eng: Duplicating specified emitter. 
int MP_Manager::DuplicateEmitter(MP_Emitter* from)
{
	MP_Emitter* emitter=new MP_Emitter(0,this);
	*emitter=*from;
	AddEmitter(emitter);
	return k_emitter-1;
}

// eng: Emitters update. 
void MP_Manager::Update(double time)
{
	for (int i=0;i<k_emitter;++i)
	{
		int state=m_emitter[i]->GetState();
		if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
		{
			m_emitter[i]->Update(time);
		}
	}
}

void MP_Manager::Render()
{
	for (int i=0;i<k_emitter;++i)
	{
		int state=m_emitter[i]->GetState();
		if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
		{
			m_emitter[i]->Render();
		}
	}
}

// eng: Stopping all the emitters. 
void MP_Manager::Stop()
{
	int k_emitter=this->GetEmitterCount();
	for (int i=0;i<k_emitter;++i)
	{
		this->GetEmitter(i)->SetState(MAGIC_STATE_STOP);
	}
}

// eng: Loads folder
void MP_Manager::LoadFolder(HM_FILE file, const char* path)
{
	Magic_SetCurrentFolder(file, path);

	MAGIC_FIND_DATA find;
	const char* name=Magic_FindFirst(file,&find,MAGIC_FOLDER | MAGIC_EMITTER);
	while (name)
	{
		if (find.animate)
			LoadEmitter(file,name);
		else
			LoadFolder(file,name);

		name=Magic_FindNext(file,&find);
	}

	Magic_SetCurrentFolder(file,"..");
}

// eng: Loads specified emitter.
void MP_Manager::LoadEmitter(HM_FILE file, const char* path)
{
	// eng: We need extract the emitter from the file
	HM_EMITTER emitter;
	if (Magic_LoadEmitter(file,path,&emitter)==MAGIC_SUCCESS)
	{
		MP_Emitter* em=new MP_Emitter(emitter,this);
		AddEmitter(em);
		// eng: Default initialization
		if (interpolation!=MAGIC_INTERPOLATION_DEFAULT)
		{
			bool _interpolation=false;
			if (interpolation==MAGIC_INTERPOLATION_ENABLE)
				_interpolation=true;
			Magic_SetInterpolationMode(emitter,_interpolation);
		}
		Magic_SetLoopMode(emitter,loop);
		Magic_SetEmitterPositionMode(emitter,position_mode);
		Magic_SetEmitterDirectionMode(emitter,position_mode);
	}
}

// eng: Adds new emitter
void MP_Manager::AddEmitter(MP_Emitter* emitter)
{
	if (m_emitter)
	{
		MP_Emitter** vm_emitter=new MP_Emitter*[k_emitter+1];
		for (int i=0;i<k_emitter;++i)
			vm_emitter[i]=m_emitter[i];
		delete []m_emitter;
		m_emitter=vm_emitter;
	}
	else
	{
		m_emitter=new MP_Emitter*[1];
	}

	m_emitter[k_emitter]=emitter;
	++k_emitter;
}

// eng: Adds the new frame-list.
MP_FrameList* MP_Manager::AddFrameList()
{
	if (m_framelist)
	{
		MP_FrameList** vm_framelist=new MP_FrameList*[k_framelist+1];
		for (int i=0;i<k_framelist;++i)
			vm_framelist[i]=m_framelist[i];
		delete []m_framelist;
		m_framelist=vm_framelist;
	}
	else
	{
		m_framelist=new MP_FrameList*[1];
	}

	MP_FrameList* framelist=new MP_FrameList(this);
	m_framelist[k_framelist]=framelist;
	k_framelist++;

	return framelist;
}

// Adds frame to the atlas
MP_Atlas* MP_Manager::AddFrameToAtlas(int index, MAGIC_TEXTURE* magic_texture)
{
	if (k_atlas<=index)
	{
		// eng: Expanding the array of atlases
		int i;
		MP_Atlas** vm_atlas=new MP_Atlas*[index+1];
		for (i=0;i<k_atlas;++i)
			vm_atlas[i]=m_atlas[i];
		delete []m_atlas;
		m_atlas=vm_atlas;

		for (i=k_atlas;i<=index;++i)
			m_atlas[i]=NULL;

		k_atlas=index+1;
	}

	if (!m_atlas[index])
	{
		// eng: Making atlas         
		m_atlas[index]=new MP_Atlas_WRAP(magic_texture->texture_width,magic_texture->texture_height);
	}

	if (magic_texture->data || magic_texture->file)
	{
		// eng: Loads frame-file in the atlas
		m_atlas[index]->LoadFrame(magic_texture, texture_folder.c_str());
	}

	return m_atlas[index];
}

// ------------------------------------------------------------------------------------------------

// eng: Class for work with the emitters. 

// eng: The count of saved copyes
int MP_Emitter::copy_count=0;

MP_Emitter::MP_Emitter(HM_EMITTER emitter, MP_Manager* owner)
{
	this->emitter=emitter;
	this->owner=owner;
	link = false;
	use_user_color = false;
	user_color = 0;

	z=0.f;

	first_restart=true;

	// eng: The file with copy
	copy_file="";

	// eng: Emitter is updated and rendered 
	state=MAGIC_STATE_UPDATE;	
}

MP_Emitter::~MP_Emitter()
{
	if (!copy_file.empty())
	{
		remove(copy_file.c_str());
		copy_file="";
	}

	Magic_UnloadEmitter(emitter);
}

// eng: Operator "="
MP_Emitter& MP_Emitter::operator=(const MP_Emitter& from)
{
	if (!copy_file.empty())
	{
		remove(copy_file.c_str());
		copy_file="";
	}


	// eng: Sets the state of the emitter. 
	state=from.state;

	z=from.z;

	// eng: The file with copy
	copy_file=from.copy_file;

	Magic_DuplicateEmitter(from.emitter,&emitter);

	return *this;
}

// eng: Returns the name of the emitter. 
const char* MP_Emitter::GetEmitterName()
{
	return Magic_GetEmitterName(emitter);
}

// eng: Loading textures for the emitter. 
void MP_Emitter::LoadTextures()
{
	int k_emitter_in_folder=Magic_GetEmitterCount(emitter);
	for (int i=0;i<k_emitter_in_folder;++i)
	{
		HM_EMITTER emitter_in_folder=Magic_GetEmitter(emitter,i);

		int k_particles=Magic_GetParticlesTypeCount(emitter_in_folder);
		for (int j=0;j<k_particles;++j)
		{
			Magic_LockParticlesType(emitter_in_folder,j);

			MP_FrameList* framelist=owner->AddFrameList();
			unsigned int texture_id=Magic_GetTextureID();
			framelist->SetTextureID(texture_id);
			int k_framelist=owner->GetFrameListCount();

			// eng: Saving index of the new texture for the fast access
			Magic_SetTextureID((unsigned int)k_framelist-1); 

			int k_frame=Magic_GetTextureCount();
			for (int n=0;n<k_frame;++n)
			{
				MAGIC_TEXTURE* magic_texture=Magic_GetTexture(n);
				MP_Frame* frame=new MP_Frame_WRAP(framelist);

				framelist->AddFrame(frame);
				frame->InitFrame(magic_texture);
			}

			Magic_UnlockParticlesType();
		}
	}
}

// eng: Restarts the emitter
void MP_Emitter::Restart()
{
	if (!copy_file.empty())
	{
		remove(copy_file.c_str());
		copy_file="";
	}

	if (Magic_IsInterval1(emitter))
	{
		// eng: Emitter animates not from start
		const char* temp_folder=owner->GetTempFolder();
		if (temp_folder)
		{
			// eng: There is temp folder
			copy_file=temp_folder;
			copy_file+="mp";
			static char s[20];
			sprintf(s,"%d",copy_count);
			++copy_count;
			copy_file+=s;
			if (first_restart)
				remove(copy_file.c_str());
		}

		const char* temp_file=NULL;
		if (!copy_file.empty())
			temp_file=copy_file.c_str();

		Magic_EmitterToInterval1(emitter,temp_file);
	}
	else
	{
		Magic_Restart(emitter);
	}

	first_restart=false;
}

// eng: Returns and sets the emitter position. 
void MP_Emitter::GetPosition(MP_POSITION& position)
{
	Magic_GetEmitterPosition(emitter,&(position.x),&(position.y));
	position.z=z;
}

void MP_Emitter::SetPosition(const MP_POSITION& position)
{
	Magic_SetEmitterPosition(emitter,position.x,position.y);
	z=position.z;
}

// eng: Moving the emitter to the position specified allowing restart. 
void MP_Emitter::Move(MP_POSITION& position, bool restart)
{
	if (restart)
	{
		Restart();
		SetPosition(position);
	}
	else
	{
		bool mode=Magic_GetEmitterPositionMode(emitter);
		if (mode==MAGIC_CHANGE_EMITTER_ONLY)
		{
			// eng: Temporary sets the MAGIC_CHANGE_EMITTER_AND_PARTICLES mode
			Magic_SetEmitterPositionMode(emitter,MAGIC_CHANGE_EMITTER_AND_PARTICLES);
		}

		SetPosition(position);

		if (mode==MAGIC_CHANGE_EMITTER_ONLY)
		{
			// eng: Returns old mode
			Magic_SetEmitterPositionMode(emitter,MAGIC_CHANGE_EMITTER_ONLY);
		}
	}
}

// eng: Offsetting the current emitter position by the value specified. 
void MP_Emitter::Offset(const MP_POSITION& offset)
{
	MP_POSITION pos;
	GetPosition(pos);
	pos+=offset;
	SetPosition(pos);
}

// eng: Returns abd sets emitter's emission direction. 
void MP_Emitter::SetDirection(float angle)
{
	Magic_SetEmitterDirection(emitter,angle);
}

float MP_Emitter::GetDirection()
{
	float angle;
	Magic_GetEmitterDirection(emitter,&angle);
	return angle;
}

// eng: Setting the emitter direction to the specified value with the restart ability. 
void MP_Emitter::Direct(float angle, bool restart)
{
	if (restart)
	{
		Restart();
		SetDirection(angle);
	}
	else
	{
		bool mode=Magic_GetEmitterDirectionMode(emitter);
		if (mode==MAGIC_CHANGE_EMITTER_ONLY)
		{
			// eng: Temporary sets the MAGIC_CHANGE_EMITTER_AND_PARTICLES mode
			Magic_SetEmitterDirectionMode(emitter,MAGIC_CHANGE_EMITTER_AND_PARTICLES);
		}

		SetDirection(angle);

		if (mode==MAGIC_CHANGE_EMITTER_ONLY)
		{
			// eng: Returns old mode
			Magic_SetEmitterDirectionMode(emitter,MAGIC_CHANGE_EMITTER_ONLY);
		}
	}
}

// eng: Rotation of the emitter by the specified value. 
void MP_Emitter::Rotate(float offset)
{
	float angle;
	angle=GetDirection();
	angle+=offset;
	SetDirection(angle);
}

// eng: Returns and sets the scale of the emitter. 
void MP_Emitter::SetScale(float scale)
{
	Magic_SetScale(emitter,scale);
}

float MP_Emitter::GetScale()
{
	return Magic_GetScale(emitter); 
}

// eng: Returns and sets the state of the emitter. 
void MP_Emitter::SetState(int state)
{
	if (this->state!=state)
	{
		if (state==MAGIC_STATE_UPDATE && Magic_IsInterrupt(emitter))
		{
			// eng: We need to stop the interrupt of the emitter
			Magic_SetInterrupt(emitter,false);
		}

		if (state==MAGIC_STATE_STOP && this->state!=MAGIC_STATE_INTERRUPT)
		{
			// eng: Stops the emitter
			Magic_Stop(emitter);
		}
		else
		{
			if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
			{
				// eng: Starts the emitter
				if (!first_restart)
				{
					if (this->state==MAGIC_STATE_STOP || (!Magic_InInterval(emitter)))
					{
						// eng: The position of emitter's animation do not includes in the visibility interval
						// eng: It is need to set it at begin
						const char* temp_file=NULL;
						if (!copy_file.empty())
							temp_file=copy_file.c_str();
						Magic_EmitterToInterval1(emitter,temp_file);
					}
				}

				if (state==MAGIC_STATE_INTERRUPT)
					Magic_SetInterrupt(emitter,true);
			}
		}

		this->state=state;
	}
}

// eng: Emitter update. 
void MP_Emitter::Update(double time)
{
	if (state==MAGIC_STATE_UPDATE || state==MAGIC_STATE_INTERRUPT)
	{
		if (first_restart)
			Restart();

		if (!Magic_IsInterpolationMode(emitter))
		{
			// It is possible only fixed step without interpolation
			time=Magic_GetUpdateTime(emitter);
		}

		if (!Magic_Update(emitter,time))
		{
			// The emitter is stopped
			SetState(MAGIC_STATE_STOP);
		}
	}
}

// eng: Emitter visualization. Returns the count of rendered particles
int MP_Emitter::Render()
{
	QmProfileFragment(MP_EmitterList::Render);
	bool bSaveState = false;
	int count=0;
	if (state!=MAGIC_STATE_STOP)
	{
		int k_emitter_in_folder=Magic_GetEmitterCount(emitter);
		// eng: enumerate all the emitters within the emitter (for the animated folders)
		for (int i=k_emitter_in_folder-1;i>=0;--i)
		{
			HM_EMITTER emitter_in_folder=Magic_GetEmitter(emitter,i);

			if (Magic_InInterval(emitter_in_folder))
			{
				int k_particles=Magic_GetParticlesTypeCount(emitter_in_folder);

				// eng: enumerate all particles types within the emitter
				for (int j=k_particles-1;j>=0;--j)
				{
					Magic_LockParticlesType(emitter_in_folder,j);

					int framelist_index=Magic_GetTextureID();
					MP_FrameList* framelist=owner->GetFrameList(framelist_index);

					if (framelist->GetFrameCount()>0)
					{
						if (!bSaveState)
						{
							MP_Frame_WRAP::SaveState();
							bSaveState = true;
						}
						
						// eng: Instead of the first frame, you can get any other. It is need only for BeginDraw/EndDraw call
						MP_Frame* first_frame=framelist->GetFrame(0); 
						first_frame->BeginDraw();

						// eng: Drawing particles
						MAGIC_PARTICLE* particle=NULL;
						for (;;)
						{
							particle=Magic_GetNextParticle();

							if (!particle)
								break;

							if (use_user_color)
								particle->color = (particle->color & 0xff000000) | user_color;

							MP_Frame* frame=framelist->GetFrame(particle->frame);
							frame->Draw(particle);
							++count;
						}
						first_frame->EndDraw();
					}

					Magic_UnlockParticlesType();
				}
			}
		}
	}
	if (bSaveState)
	{
		MP_Frame_WRAP::RecoverState();
	}
	return count;
}

// --------------------------------------------------------------------------------------------------

// eng: This class stores texture frame-files for one particle type
MP_FrameList::MP_FrameList(MP_Manager* owner)
{
	this->owner=owner;

	k_frame=0;
	m_frame=NULL;

	texture_id=0xffffffff;
}

MP_FrameList::~MP_FrameList()
{
	for (int i=0;i<k_frame;i++)
	{
		delete m_frame[i];
		m_frame[i]=NULL;
	}

	if (m_frame)
	{
		delete []m_frame;
		m_frame=NULL;
	}

	k_frame=0;
}

// eng: Returns the frame by its index.
MP_Frame* MP_FrameList::GetFrame(int index)
{
	if (index>=0 && index<k_frame)
		return m_frame[index];
	return NULL;
}

// eng: Adds the frame
void MP_FrameList::AddFrame(MP_Frame* frame)
{
	if (m_frame)
	{
		MP_Frame** vm_frame=new MP_Frame*[k_frame+1];
		for (int i=0;i<k_frame;++i)
			vm_frame[i]=m_frame[i];
		delete []m_frame;
		m_frame=vm_frame;
	}
	else
	{
		m_frame=new MP_Frame*[1];
	}

	m_frame[k_frame]=frame;
	++k_frame;
}

// --------------------------------------------------------------------------------------------------

// eng: Initialization of texture frame
MP_Atlas* MP_Frame::InitFrame(MAGIC_TEXTURE* magic_texture)
{
	this->magic_texture=magic_texture;
	MP_Manager* manager=owner->GetOwner();
	unsigned int texture_id=owner->GetTextureID();
	MP_Atlas* atlas=manager->AddFrameToAtlas(texture_id,magic_texture);
	return atlas;
}
