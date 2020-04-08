//-----------------------------------------------------------------------------
// File: magic.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_LIBRARY
#define MAGIC_PARTICLES_LIBRARY

#include <math.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int HM_FILE;
typedef int HM_EMITTER;
typedef int HM_IMPORT;

#define MAGIC_SUCCESS	-1
#define MAGIC_ERROR		-2
#define MAGIC_UNKNOWN	-3

#define MAGIC_FOLDER	1
#define MAGIC_EMITTER	2

#define MAGIC_NOLOOP	0
#define MAGIC_LOOP		1
#define MAGIC_FOREVER	2

#define MAGIC_COLOR_STANDARD	0
#define MAGIC_COLOR_TINT		1
#define MAGIC_COLOR_USER		2

#define MAGIC_EMITTER_POINT		0
#define MAGIC_EMITTER_LINE		1
#define MAGIC_EMITTER_CIRCLE	2
#define MAGIC_EMITTER_ELLIPSE	3
#define MAGIC_EMITTER_SQUARE	4
#define MAGIC_EMITTER_RECTANGLE	5
#define MAGIC_EMITTER_IMAGE		6
#define MAGIC_EMITTER_TEXT		7

#define MAGIC_DIAGRAM_LIFE				0
#define MAGIC_DIAGRAM_NUMBER			1
#define MAGIC_DIAGRAM_SIZE				2
#define MAGIC_DIAGRAM_VELOCITY			3
#define MAGIC_DIAGRAM_WEIGHT			4
#define MAGIC_DIAGRAM_SPIN				5
#define MAGIC_DIAGRAM_ANGULAR_VELOCITY	6
#define MAGIC_DIAGRAM_MOTION_RAND		7
#define MAGIC_DIAGRAM_VISIBILITY		8
#define MAGIC_DIAGRAM_DIRECTION			9

#define MAGIC_PI 3.1415926535897932384626433832795028841971693993751058209

// eng: MAGIC_PARTICLE - particle structure, containing all of its properties used for visualization
struct MAGIC_PARTICLE
{
	float x,y;
	float size;
	float angle;
	unsigned long color;
	unsigned int frame;
};

// eng: MAGIC_TEXTURE - structure, containing texture frame-file information
struct MAGIC_TEXTURE
{
	unsigned int length;	// eng: The length of the file (in byth) 
	const char* data;		// eng: The image of the file 
	int crc;				// eng: CRC of the file 
	const char* file;		// eng: The file's name 
	const char* path;

	float left,top,right,bottom;

	int frame_width;
	int frame_height;

	int texture_width;	
	int texture_height;		

	float pivot_x;		
	float pivot_y;		

	float scale;		
};

// eng: MAGIC_ATLAS - structure, containing information on frame file locations within the textural atlas
struct MAGIC_ATLAS
{
	int width;				
	int height;			
	int count;					
	MAGIC_TEXTURE** textures;	
};

// eng: MAGIC_FIND_DATA - is a structure that is used in searching emitters and directories 
struct MAGIC_FIND_DATA
{
	// eng: result
	int type;				
	const char* name;		
	int animate;			

	int mode;		
	
	// eng: additional data
	void* folder;		
	int index;			
};

// eng: MAGIC_VERTEX_RECTANGLE - structure, storing the coordinates of the particle rectangle vertice
struct MAGIC_VERTEX_RECTANGLE
{
	float x1,y1;
	float x2,y2;
	float x3,y3;
	float x4,y4;
};

// eng: MAGIC_TRAJECTORY - structure, containing additional particle trajectory information
struct MAGIC_TRAJECTORY
{
	int start;
	int duration;
	int type;
};

// eng: MAGIC_IMPORT_PARTICLES_TYPE - structure for information about exported particles type 
struct MAGIC_IMPORT_PARTICLES_TYPE
{
	const char* name;
	int parent;
	bool attached;
	int emitter;
	bool is_3d;
};

// eng: MAGIC_IMPORT_EMITTER - structure for information about exported emitters 
struct MAGIC_IMPORT_EMITTER
{
	const char* name;
	int x, y;
};

// eng: MAGIC_IMPORT_PARTICLE_ID - structure of identification information about particle 
struct MAGIC_IMPORT_PARTICLE_ID
{
	unsigned int id;
	unsigned int parent_id;
	unsigned char attached_state;
};

// eng: Loads the ptc-file from the path specified
int Magic_OpenFile(const char* file_name, HM_FILE* hmFile);

// eng: Loads the ptc-file image from the memory
int Magic_OpenFileInMemory(const char* buffer, HM_FILE* hmFile);

// eng: Closes the file, opened earlier by use of Magic_OpenFile or Magic_OpenFileInMemory
int Magic_CloseFile(HM_FILE hmFile);

// eng: Closing all the opened files
void Magic_CloseAllFiles();

// eng: Returns the current folder path
const char* Magic_GetCurrentFolder(HM_FILE hmFile);
 
// eng: Sets the new current folder
int Magic_SetCurrentFolder(HM_FILE hmFile, const char* path=NULL);

// eng: Searches for the first folder or emitter within the current folder and returns the type of the object found
const char* Magic_FindFirst(HM_FILE hmFile,MAGIC_FIND_DATA* data,int mode);

// eng: Searches for the next folder or emitter within the current folder and returns the type of the object found
const char* Magic_FindNext(HM_FILE hmFile, MAGIC_FIND_DATA* data);

// eng: Returns the name of the file that was opened through the Magic_OpenFile
const char* Magic_GetFileName(HM_FILE hmFile);

// eng: Creates the emitter object and loads its data 
int Magic_LoadEmitter(HM_FILE hmFile, const char* name, HM_EMITTER* hmEmitter);

// eng: Unloads the emitter data and destroys it
int Magic_UnloadEmitter(HM_EMITTER hmEitter);

// --------------------------------------------------------------------------------

// eng: Gets the copy of the emitter
int Magic_DuplicateEmitter(HM_EMITTER hmEmitterFrom, HM_EMITTER* hmEmitterTo);

// eng: Processes the emitter. Creates, displaces and removes the particles
bool Magic_Update(HM_EMITTER hmEmitter, double time);

// eng: Stops the emitter
int Magic_Stop(HM_EMITTER hmEmitter);

// eng: Interrupts/Starts emitter work
int Magic_SetInterrupt(HM_EMITTER hmEmitter, bool interrupt);

// eng: Returns the flag showing that emitter is in interrupted mode
bool Magic_IsInterrupt(HM_EMITTER hmEmitter);

// eng: Returns current animation position 
double Magic_GetPosition(HM_EMITTER hmEmitter);
 
// eng: Sets the current animation position
int Magic_SetPosition(HM_EMITTER hmEmitter,double position);

// eng: Returns animation duration
double Magic_GetDuration(HM_EMITTER hmEmitter);

// eng: Restarts the emitter from the beginning 
int Magic_Restart(HM_EMITTER hmEmitter);

// eng: Returns the Magic Particles (Dev) time increment, used for the animation 
double Magic_GetUpdateTime(HM_EMITTER hmEmitter);

// eng: Returns the emitter behaviour mode at the end of the animation
int Magic_GetLoopMode(HM_EMITTER hmEmitter);

// eng: Sets the emitter behaviour mode at the end of the animation
int Magic_SetLoopMode(HM_EMITTER hmEmitter,int mode);

// eng: Returns the color management mode
int Magic_GetColorMode(HM_EMITTER hmEmitter);

// eng: Sets the color management mode
int Magic_SetColorMode(HM_EMITTER hmEmitter,int mode);

// eng: Returns the user defined tint
int Magic_GetTint(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint
int Magic_SetTint(HM_EMITTER hmEmitter,int tint);

// eng: Returns the user defined tint strength
float Magic_GetTintStrength(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint strength
int Magic_SetTintStrength(HM_EMITTER hmEmitter,float tint_strength);

// eng: Returns coordinates of the emitter
int Magic_GetEmitterPosition(HM_EMITTER hmEmitter,float* x, float* y);

// eng: Sets the coordinates of the emitter
int Magic_SetEmitterPosition(HM_EMITTER hmEmitter,float x, float y);

// eng: Returns the mode of the emitter coordinates
bool Magic_GetEmitterPositionMode(HM_EMITTER hmEmitter);

// eng: Sets the mode of the emitter coordinates
int Magic_SetEmitterPositionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Returns emitter direction
int Magic_GetEmitterDirection(HM_EMITTER hmEmitter,float* angle);

// eng: Sets the direction of the emitter 
int Magic_SetEmitterDirection(HM_EMITTER hmEmitter,float angle);

// eng: Gets the emitter's direction mode
bool Magic_GetEmitterDirectionMode(HM_EMITTER hmEmitter);

// eng: Sets emitter's rotation mode
int Magic_SetEmitterDirectionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Moves particles
int Magic_MoveEmitterParticles(HM_EMITTER hmEmitter, float offset_x, float offset_y);

// eng: Rotates particles
int Magic_RotateEmitterParticles(HM_EMITTER hmEmitter, float offset);

// eng: Returns the shape of the emitter itself or the shape of the emitter for the specified particles type
int Magic_GetEmitterType(HM_EMITTER hmEmitter,int index);

// eng: Returns the Intensity flag 
bool Magic_IsIntensive();

// eng: Returns the left position of the visibility range
double Magic_GetInterval1(HM_EMITTER hmEmitter);

// eng: Sets the left position of the visibility range
int Magic_SetInterval1(HM_EMITTER hmEmitter,double position);

// eng: Returns the right position of the visibility range
double Magic_GetInterval2(HM_EMITTER hmEmitter);

// eng: Sets the right position of the visibility range
int Magic_SetInterval2(HM_EMITTER hmEmitter,double position);

// eng: Figures out if the current animation position is within the visibility range
bool Magic_InInterval(HM_EMITTER hmEmitter);

// eng: Returns the number particles type contained in emitter
int Magic_GetParticlesTypeCount(HM_EMITTER hmEmitter);

// eng: Returns the emitter scale
float Magic_GetScale(HM_EMITTER hmEmitter);

// eng: Sets the emitter scale
int Magic_SetScale(HM_EMITTER hmEmitter, float scale);

// eng: Returns the particle positions interpolation usage flag
bool Magic_IsInterpolationMode(HM_EMITTER hmEmitter);

// eng: Sets/resets the particle positions interpolation usage flag
int Magic_SetInterpolationMode(HM_EMITTER hmEmitter,bool mode);

// eng: Returns the flag of stability/randomness of the emitter behaviour 
bool Magic_IsRandomMode(HM_EMITTER hmEmitter);

// eng: Sets the flag of stability/randomness of the emitter behaviour
int Magic_SetRandomMode(HM_EMITTER hmEmitter,bool mode);

// eng: Copying the particles array into emitter from the file
int Magic_LoadArrayFromFile(HM_EMITTER hmEmitter, const char* file);

// eng: Copying the particles array from the emitter into the file
int Magic_SaveArrayToFile(HM_EMITTER hmEmitter, const char* file);

// eng: Sets the user data
int Magic_SetData(HM_EMITTER hmEmitter, int data);

// eng: Returns the user data
int Magic_GetData(HM_EMITTER hmEmitter);

// eng: Returns the name of the emitter 
const char* Magic_GetEmitterName(HM_EMITTER hmEmitter);

// eng: Returns the name of the particles type
const char* Magic_GetParticlesTypeName(HM_EMITTER hmEmitter, int index);

// eng: Returns the animate folder flag
bool Magic_IsFolder(HM_EMITTER hmEmitter);

// eng: Returns the number of emitters contained in animate folder. 1 is returned for emitter 
int Magic_GetEmitterCount(HM_EMITTER hmEmitter);

// eng: Returns the specified emitter from animate folder. Returns itself for emitter
HM_EMITTER Magic_GetEmitter(HM_EMITTER hmEmitter,int index);

// eng: Sets the animation position at the left position of visibility range
int Magic_EmitterToInterval1(HM_EMITTER hmEmitter, const char* file);

// eng: Returns the flag of the animation of emitter that begins from 0 position
bool Magic_IsInterval1(HM_EMITTER hmEmitter);


// --------------------------------------------------------------------------

// eng: Locks the specified particles type for the further processing
int Magic_LockParticlesType(HM_EMITTER hmEmitter, int index);

// eng: Releases previously locked particles type
int Magic_UnlockParticlesType();

// eng: Returns the number used as a user resources texture identificator
unsigned int Magic_GetTextureID();

// eng: Sets the number used as a user resources texture identificator
int Magic_SetTextureID(unsigned int id);

// eng: Returns the number of textural frame-files in locked particles type
int Magic_GetTextureCount();

// eng: Returns MAGIC_TEXTURE structure containing the specified frame-file information 
MAGIC_TEXTURE* Magic_GetTexture(int index);

// eng: Returns the pointer to the next particle. Is used to go through all the existing particles during the visualization process
MAGIC_PARTICLE* Magic_GetNextParticle();

// eng: Returns the pointer to the MAGIC_VERTEX_RECTANGLE structure, containing the coordinates of the visualization rectangle vertice
MAGIC_VERTEX_RECTANGLE* Magic_GetParticleRectangle(MAGIC_PARTICLE* particle, MAGIC_TEXTURE* texture);

// eng: Changes the position of the particle that is got by Magic_GetNextParticle 
void Magic_MoveParticle(float x,float y);

// eng: Rotates the particle that was obtained by Magic_GetNextParticle around the emitter
void Magic_RotateParticle(float angle);


// eng: Creates textural atlases
float Magic_CreateAtlases(int width,int height,int step,float scale_step);

// eng: Returns the number of textural atlases
int Magic_GetAtlasCount();

// eng: Returns the textural atlas specified
const MAGIC_ATLAS* Magic_GetAtlas(int index);


// eng: Returns the specified diagram factor
float Magic_GetDiagramFactor(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// eng: Sets the specified diagram factor 
int Magic_SetDiagramFactor(HM_EMITTER hmEmitter,int type_index, int diagram_index, float factor);

// eng: Returns the offset for the specified diagram 
float Magic_GetDiagramAddition(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// eng: Sets the offset for the specified diagram
int Magic_SetDiagramAddition(HM_EMITTER hmEmitter,int type_index, int diagram_index, float addition);

// eng: Figures out if the diagram is managable
bool Magic_IsDiagramEnabled(HM_EMITTER hmEmitter,int type_index, int diagram_index);

// --------------------------------------------------------------------------------------

// eng: Loads the ptf-file from the path specified
int Import_OpenFile(const char* file_name, HM_IMPORT* hmImport);

// eng: Loads the ptf-file image from RAM
int Import_OpenFileInMemory(const char* buffer, HM_IMPORT* hmImport);

// eng: Closes the file, opened earlier by Import_OpenFile or Import_OpenFileInMemory 
int Import_CloseFile(HM_IMPORT hmImport);

// eng: Returns the frame rate
int Import_GetRate(HM_IMPORT hmImport);

// eng: Returns the frame size
int Import_GetSize(HM_IMPORT hmImport,int* width, int* height);

// eng: Returns the flag of presence the identification information for every particle
bool Import_IsID(HM_IMPORT hmImport);

// eng: Returns the number of animation frames
int Import_GetFrameCount(HM_IMPORT hmImport);

// eng: Returns the current frame
int Import_GetFrame(HM_IMPORT hmImport);

// eng: Sets the new current frame
int Import_SetFrame(HM_IMPORT hmImport, int frame);

// eng: Increases the value of the current frame by 1
int Import_NextFrame(HM_IMPORT hmImport);

// eng: Decreases the value of the current frame by 1
int Import_PreviousFrame(HM_IMPORT hmImport);

// eng: Returns the file name, that was opened using Import_OpenFile
const char* Import_GetFileName(HM_IMPORT hmImport);

// eng: Returns the numbers of exported emitters
int Import_GetEmitterCount(HM_IMPORT hmImport);

// eng: Returns the information about exported emitter
int Import_GetEmitter(HM_IMPORT hmImport, int index, MAGIC_IMPORT_EMITTER* info);

// eng: Returns the number particles type contained in import file
int Import_GetParticlesTypeCount(HM_IMPORT hmImport);

// eng: Returns the information about particles type 
int Import_GetParticlesType(HM_IMPORT hmImport,	int index, MAGIC_IMPORT_PARTICLES_TYPE* info);

// eng: Locks the specified particles type for the further processing
int Import_LockParticlesType(HM_IMPORT hmImport, int index);

// eng: Releases previously locked particles type
int Import_UnlockParticlesType();

// eng: Returns the number of textural frame-files in locked particles type
int Import_GetTextureCount();

// eng: Fills the MAGIC_TEXTURE structure with the specified texture frame-file information
int Import_GetTexture(int index,MAGIC_TEXTURE* texture);

// eng: Returns the number used as a user resources texture identificator
unsigned int Import_GetTextureID();

// eng: Sets the number used as a user resources texture identificator
int Import_SetTextureID(unsigned int id);

// eng: Returns the Intensity flag
bool Import_IsIntensive();

// eng: Returns the pointer to the next particle. Is used to go through all the existing particles during the visualization process
MAGIC_PARTICLE* Import_GetNextParticle();

// eng: Returns the pointer to the struct MAGIC_IMPORT_PARTICLE_ID for the last particle
MAGIC_IMPORT_PARTICLE_ID* Import_GetParticleID();

// eng: Returns the number of the particles of locked type
int Import_GetParticleCount();

// eng: Returns emitter coordinates
int Import_GetEmitterPosition(int* x,int* y);

// ----------------------------------------------------------------------------------------

// eng: Creates the particle trajectories for the loaded import file
int Trajectory_Create(HM_IMPORT hmImport);

// eng: Destroyes trajectories of the particles that were previously created by Trajectory_Create
int Trajectory_Destroy();

// eng: Returns the number of particles, for which trajectories were created using Trajectory_Create
int Trajectory_GetParticleCount();

// eng: Locks the specified particle for the trajectory extraction
int Trajectory_LockParticle(unsigned int index);

// eng: Unlocks earlier locked particle
int Trajectory_UnlockParticle();

// eng: Returns the pointer to the MAGIC_TRAJECTORY structure that contains the information on the particle locked
MAGIC_TRAJECTORY* Trajectory_GetParticleInfo();

// eng: Returns the pointer to the next position of the locked particle. Used to exract the whole trajectory of the particle
MAGIC_PARTICLE* Trajectory_GetNextPosition();

// eng: Returns the pointer to the struct MAGIC_IMPORT_PARTICLE_ID for the last particle
MAGIC_IMPORT_PARTICLE_ID* Trajectory_GetParticleID();

// eng: Returns the coordinates of parent particle for the last particle
int Trajectory_GetParentPosition(float* x,float* y);


#ifdef __cplusplus
}
#endif

#endif
