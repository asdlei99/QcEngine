#ifndef INCLUDED_RAND_H
#define INCLUDED_RAND_H

#ifdef _EAGLE
#include "vgtypedef.h"
#endif
#ifdef _PC
#include "pc/pc_gmxsdk.h"
#endif

typedef signed char		S8;
typedef signed short	S16;
typedef signed long		S32;
typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned long	U32;

#ifdef __cplusplus
extern "C" {
#endif

//extern int rand0(void);						//����0��0x7fff֮����������
//extern int getRand0( void );
//extern double ran0(void);					//����0.0��1.0֮���С��
//extern double ran1(void);					//����0.0��1.0֮���С��
//extern double ran2(void);					//����0.0��1.0֮���С��
//extern double ran3(void);					//����0.0��1.0֮���С��
//extern double ran4(void);					//����0.0��1.0֮���С����better may be
//extern void Random(void);

//extern void srand0(S32);


//extern U16 UniformRandom(double a, double b);
extern U16 rand1000();
//extern U16 rand4096();
extern U16 rand10000();
extern U16 rand32768();

///////////////////////// for uniform random begin   ///////////////////////// 
extern U16 UniformRandom(double a, double b);
extern U32 SPDCRandomInt(void);
extern double SPDCRandom(void);
extern void   PlantSeeds(long x);
//extern void   GetSeed(long *x);
//extern void   TestRandom(void);
///////////////////////// for uniform random end   ///////////////////////// 

#ifdef __cplusplus
};
#endif

#endif
