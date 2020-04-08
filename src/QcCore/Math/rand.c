/*
 ********************************************************************************
 * File: rand.c
 *
 * Description: 产生随机数的几种算法。
 *
 * Related Files: rand.h
 * 
 * History:
 * 
 * yyyymmmdd  Changed by       Change Reason
 * ---------  --------------   ------------------------------------------------
 *******************************************************************************
 */


#if defined DEBUG || defined _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif



#include "rand.h"
//#include "../include/clock.h"
//#include "../include/sram.h"
#include "time.h"

//static long seed_ = 0;

unsigned int seed_ = 1 ;                        //随机数的种子
unsigned int seed2_ = 1;                        //随机数的种子

U8 ranram[4];

///////////////////////// for uniform random begin   ///////////////////////// 
		
#define MODULUS    2147483647 /* DON'T CHANGE THIS VALUE                  */
#define MULTIPLIER 48271      /* DON'T CHANGE THIS VALUE                  */
#define CHECK      399268537  /* DON'T CHANGE THIS VALUE                  */
#define STREAMS    256        /* # of streams, DON'T CHANGE THIS VALUE    */
#define A256       22925      /* jump multiplier, DON'T CHANGE THIS VALUE */
#define DEFAULT    123456789  /* initial seed, use 0 < DEFAULT < MODULUS  */
      
static long seed[STREAMS] = {DEFAULT};  /* current state of each stream   */
static int  stream        = 0;          /* stream index, 0 is the default */
static int  initialized   = 0;          /* test for stream initialization */

///////////////////////// for uniform random end   ///////////////////////// 


void* memmove0 ( void * dst, const void * src, int count )	//实现低地址和高地址之间的数据拷贝(lsw)
{
	void* ret = dst;
	if (dst <= src || (char *)dst >= ((char *)src + count)) {
		/*
		* Non-Overlapping Buffers
		* copy from lower addresses to higher addresses
		*/
		while (count--) {
				*(char *)dst = *(char *)src;
				dst = (char *)dst + 1;
				src = (char *)src + 1;
		}
	}
	else {
		/*
		* Overlapping Buffers
		* copy from higher addresses to lower addresses
		*/
		dst = (char *)dst + count - 1;
		src = (char *)src + count - 1;

		while (count--) {
				*(char *)dst = *(char *)src;
				dst = (char *)dst - 1;
				src = (char *)src - 1;
		}
	}
	return ret;
}

void shuffle_rand(void)								//产生一个0到0x7fff之间的随机整数(lsw))
{
	seed_ = ((seed_ * 214013L + 2531011L) >> 16) & 0x7fff;
}
int rand0(void)										//获取0到0x7fff之间的随机整数(lsw)
{	
	return UniformRandom((double)0, (double)32768);

}

int getRand0( void )								//获取0到0x7fff之间的随机整数(lsw)
{
	return seed_;
}

#define IA   16087L
#define IM   2147483647L
#define AM   ( 1.0 / (double)IM ) 
#define IQ   127773L
#define IR   2836L
#define MASK 123459876L

double ran0(void)									//产生0.0到1.0之间的随机小数(lsw)
{
    long k;
    double ans;
	
    seed_ ^= MASK;
    k = seed_/IQ;
    seed_ = IA*(seed_-k*IQ)-IR*k;
    if(seed_ < 0) seed_ += IM;
    ans = AM*seed_;
    seed_ ^= MASK;
    
    return ans;
}

#define NUMERICS_MAX_ERROR 5.0e-9
#define NTAB 32L
#define NDIV ( 1.0 + (double)(IM-1.0) / (double)NTAB )
#define RNMX ( 1.0 - NUMERICS_MAX_ERROR )

double ran1(void)									//产生0.0到1.0之间的随机小数(lsw)					
{
    int j;
    long k;
    static long iy=0;
    static long iv[NTAB];
    double temp;
    	
    if(seed_ <= 0 || !iy){
        if(-seed_ < 1){
            seed_ = 1;
        } else {
            seed_ = -seed_;
        }
        for(j = NTAB+7; j >= 0; j--){
            k = seed_ / IQ;
            seed_ = IA * (seed_ - k * IQ) - IR * k;
            if(seed_ < 0){
                seed_ += IM;
            }
            if(j < NTAB){
                iv[j] = seed_;
            }
        }
        iy = iv[0];
    }
    k = seed_ / IQ;
    seed_ = IA * (seed_ - k * IQ) - IR * k;
    if(seed_ < 0){
        seed_ += IM;
    }
    j = (int)(iy / NDIV);
    iy = iv[j];
    iv[j] = seed_;
    if((temp = AM * iy) > RNMX){
        return RNMX;
    } else {
        return temp;
    }
}

#define IA1   40014L
#define IA2   40692L
#define IM1   2147483563L
#define AM1   ( 1.0 / (double)IM1 )
#define IM2   2147483399L
#define IMM1  (IM1 - 1)
#define IQ1   53668L
#define IQ2   52774L
#define IR1   12211L
#define IR2   3791
#define NDIV1 (1.0+ (double)IMM1 / (double)NTAB )

double ran2(void)									//产生0.0到1.0之间的随机小数(lsw)
{
    int j;
    long k;
    static long idum2 = 123456789L;
    static long iy=0;
    static long iv[NTAB];
    double temp;
		
    if(seed_ <= 0){
        if(-seed_ < 1) seed_ = 1;
        else seed_ = -seed_;
        idum2=seed_;
        for(j=NTAB+7;j>=0;j--){
            k=seed_/IQ1;
            seed_ =IA1*(seed_-k*IQ1)-k*IR1;
            if(seed_ < 0) seed_ += IM1;
            if(j < NTAB) iv[j] = seed_;
        }
        iy=iv[0];
    }
    k=seed_/IQ1;
    seed_=IA1*(seed_-k*IQ1)-k*IR1;
    if(seed_ < 0) seed_ += IM1;
    k = idum2/IQ2;
    idum2=IA2*(idum2-k*IQ2)-k*IR2;
    if(idum2 < 0) idum2 += IM2;
    j = (int)(iy/NDIV1);
    iy=iv[j]-idum2;
    iv[j]=seed_;
    if(iy < 1)iy +=IMM1;
    if((temp=AM1*iy) > RNMX) return RNMX;
    else return temp;
}

#define MBIG  1000000000L
#define FAC   ( 1.0 / (double)MBIG )
#define MSEED 161803398L
#define MZ    0L

double ran3(void)									//产生0.0到1.0之间的随机小数(lsw)
{
    static int inext, inextp;
    static long ma[56];
    static int iff = 0;
    long mj, mk;
    int i, ii, k;    
	
    if(seed_ < 0 || iff == 0){
        iff = 1;
        mj = MSEED - (seed_ < 0 ? -seed_ : seed_);
        mj %= MBIG;
        ma[55] = mj;
        mk = 1;
        for(i = 1; i <= 54; i++){
            ii = (21 * i) % 55;
            ma[ii] = mk;
            mk = mj - mk;
            if(mk < MZ) mk += MBIG;
            mj = ma[ii];
        }
        for(k = 1; k <= 4; k++){
            for(i = 1; i <= 55; i++){
                ma[i] -= ma[1+(i+30) % 55];
                if(ma[i] < MZ) ma[i] += MBIG;
            }
        }
        inext = 0;
        inextp = 31;
        seed_ = 1;
    }
    if(++inext == 56) inext = 1;
    if(++inextp == 56) inextp = 1;
    mj = ma[inext] - ma[inextp];
    if(mj < MZ) mj += MBIG;
    ma[inext] = mj;
    
    return mj * FAC;
}

#define m16Long 65536L          /* 2^16 */
#define m16Mask 0xFFFF          /* mask for lower 16 bits */
#define m15Mask 0x7FFF          /* mask for lower 15 bits */
#define m31Mask 0x7FFFFFFF      /* mask for 31 bits */
#define m32Double  4294967295.0 /* 2^32-1 */

double ran4(void)									//产生0.0到1.0之间的随机小数(lsw)	
{
    static short m1[10];
    static short m2[10];
	static unsigned long seed;
	static short start = 1;

    unsigned long number, n1, n2;
    short n, *p;
    unsigned short sNumber;

    if(seed_ < 0 || start){
		seed = (unsigned long)seed_;
        sNumber = (unsigned short)(seed & m16Mask);
        number = seed & m31Mask;

        p = m1;
        for(n = 18; n-- ;){
            number = 30903 * sNumber + (number >> 16);
            *p++ = sNumber = (unsigned short)(number & m16Mask);
            if(n == 9){
                p = m2;
            }
        }
        m1[0] &= m15Mask;
        m2[0] &= m15Mask;
		start = 0;
    }

    memmove0(m1 + 2, m1 + 1, 8 * sizeof(short));
    memmove0(m2 + 2, m2 + 1, 8 * sizeof(short));

    n1 = m1[0];
    n2 = m2[0];

    n1 += 1941 * m1[2] + 1860 * m1[3] + 1812 * m1[4] + 1776 * m1[5]
        + 1492 * m1[6] + 1215 * m1[7] + 1066 * m1[8] + 12013 * m1[9];
    n2 += 1111 * m2[2] + 2222 * m2[3] + 3333 * m2[4] + 4444 * m2[5]
        + 5555 * m2[6] + 6666 * m2[7] + 7777 * m2[8] + 9272 * m2[9];

    m1[0] = (short)(n1 / m16Long);
    m2[0] = (short)(n2 / m16Long);
    m1[1] = (short)(m16Mask & n1);
    m2[1] = (short)(m16Mask & n2);

    seed = (((long)m1[1]) << 16) + (long)m2[1];
	seed_ = seed & m31Mask;

    return seed / m32Double;
}



////////////////////////////////////////////////////////



void Random(void)
{
	char number,cyold,cynew;
	U8 a,b,c,d;
//#ifdef _PC
	//int i;
	//for(i=0;i<4;++i)
	//{
	//	ranram[i] = rand0()%256;
	//}
	//return;
//#endif
	a = ranram[0];
	b = ranram[1];
	c = ranram[2];
	d = ranram[3];
	number=ranram[3];
	number>>=2;
	number^=ranram[2];
	number>>=4;
	number^=ranram[0];
	number>>=1;
	number^=ranram[0];
	number^=0x01;
	cyold=number&0x01;
	number>>=1;
	cynew=(ranram[0]&0x80)>>7;
	ranram[0]<<=1;
	ranram[0]|=cyold;
	cyold=cynew;
	cynew=(ranram[1]&0x80)>>7;
	ranram[1]<<=1;
	ranram[1]|=cyold;
	cyold=cynew;
	cynew=(ranram[2]&0x80)>>7;
	ranram[2]<<=1;
	ranram[2]|=cyold;
	cyold=cynew;
	ranram[3]<<=1;
	ranram[3]|=cyold;
	if( (ranram[0] == a) && (ranram[1] == b) && (ranram[2] == c) && (ranram[3] ==d) )
	{
		ranram[0] = 0xaa;
		ranram[1] = 0x55;
		ranram[2] = 0xaa;
		ranram[3] = 0x55;
	}
}


U16 rand1000()
{
	return UniformRandom((double)0, (double)1000);
}

U16 rand4096()
{
	return UniformRandom((double)0, (double)4096);
}



U16 rand10000()
{
	return UniformRandom((double)0, (double)10000);
}


U16 rand32768()
{
	return UniformRandom((double)0, (double)32768);
}


///////////////////////// for uniform random begin   ///////////////////////// 


///////////////////////// for uniform random begin   ///////////////////////// 

void srand0(S32 seed )					//将参数转换成long型（lsw）
{
	seed_ = (long)seed;
	seed2_ = (long)seed % MODULUS;
}


/* ----------------------------------------------------------------
* SPDCRandom returns a pseudo-random real number uniformly distributed 
* between 0.0 and 1.0. 
* ----------------------------------------------------------------
*/
double SPDCRandom(void)
{
	return ((double)SPDCRandomInt()/ MODULUS);
}


/* ---------------------------------------------------------------------
* Use this function to set the state of all the random number generator 
* streams by "planting" a sequence of states (seeds), one per stream, 
* with all states dictated by the state of the default stream. 
* The sequence of planted states is separated one from the next by 
* 8,367,782 calls to Random().
* ---------------------------------------------------------------------
*/
void PlantSeeds(long x)
{
	seed2_ = x % MODULUS;	/* set seed2_    */

}



/* ---------------------------------------------------------------
* Use this function to get the state of the current random number 
* generator stream.                                                   
* ---------------------------------------------------------------
*/
void GetSeed(long *x)

{
	*x = seed2_;
}


/* =========================================================== 
* Returns a uniformly distributed real number between a and b. 
* NOTE: use a < b
* ===========================================================
*/
U16 UniformRandom(double a, double b)
{ 
	double temp;
	double temp2;

	temp = SPDCRandom();
	temp2 = (a + (b - a) * temp);

	return (U16)temp2;

	//return (a + (b - a) * SPDCRandom());
}

U32 SPDCRandomInt(void)
{
	const long Q = MODULUS / MULTIPLIER;
	const long R = MODULUS % MULTIPLIER;
	long t;

	t = MULTIPLIER * (seed2_ % Q) - R * (seed2_ / Q);

	if (t > 0) 
	{
		seed2_ = t;
	}
	else 
	{
		seed2_ = t + MODULUS;
	}
	return seed2_;
}




#ifdef _PC

void TestRandom(void)
/* ------------------------------------------------------------------
 * Use this (optional) function to test for a correct implementation.
 * ------------------------------------------------------------------    
 */
{
  long   i;
  long   x;
  double u;
  char   ok = 0; 

  S32 temp;

  const long Q = MODULUS / A256;
  const long R = MODULUS % A256;

  PlantSeeds(1);                       /* and set the state to 1    */
  for(i = 0; i < 10000; i++)
  {
    u = SPDCRandom();
  }
  
  GetSeed(&x);                      /* get the new state value   */
  ok = (x == CHECK);                /* and check for correctness */

  //SelectStream(1);                  /* select stream 1                 */ 
  //PlantSeeds(1);                    /* set the state of all streams    */
  //GetSeed(&x);                      /* get the state of stream 1       */
  //ok = ok && (x == A256);           /* x should be the jump multiplier */ 

	 
	
   PlantSeeds(1); 
  seed2_ = A256 * (seed2_ % Q) - R * (seed2_ / Q);

  GetSeed(&x);                      /* get the state of stream 1       */
  ok = ok && (x == A256);           /* x should be the jump multiplier */    

  if (ok)
  {
    printf("\n The implementation of rngs.c is correct.\n\n");
  }
  else
  {
    printf("\n\a ERROR -- the implementation of rngs.c is not correct.\n\n");
  }
}

#endif

///////////////////////// for uniform random end   ///////////////////////// 