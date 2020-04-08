#pragma once
#include <math.h>
#include <float.h>
#include <stdlib.h> // for abs() etc.
#include <limits.h> // For INT_MAX / UINT_MAX

#ifndef REALINLINE
#ifdef _MSC_VER
#define REALINLINE __forceinline
#else
#define REALINLINE inline
#endif
#endif

namespace QcMath
{
	//! Rounding error constant often used when comparing f32 values.
	const s32 ROUNDING_ERROR_Int32 = 0;
	const f32 ROUNDING_ERROR_f32 = 0.000001f;
	const f64 ROUNDING_ERROR_f64 = 0.00000001;

#ifdef PI // make sure we don't collide with a define
#undef PI
#endif
	//! Constant for PI.
	const f32 PI		= 3.14159265359f;

	const f32 TWO_PI	= PI * 2.0f;

	//! Constant for reciprocal of PI.
	const f32 RECIPROCAL_PI	= 1.0f/PI;

	//! Constant for half of PI.
	const f32 HALF_PI	= PI/2.0f;

#ifdef PI64 // make sure we don't collide with a define
#undef PI64
#endif
	//! Constant for 64bit PI.
	const f64 PI64		= 3.1415926535897932384626433832795028841971693993751;

	const f64 TWO_PI64	= PI64 * 2.0f;

	//! Constant for half of PI.
	const f64 HALF_PI64	= PI64/2.0f;

	const f32 gfMax		= 3.402823466e+30F;

	const f32 gfMin		= 1.175494351e-30F;

	//! Constant for 64bit reciprocal of PI.
	const f64 RECIPROCAL_PI64 = 1.0/PI64;

	//! 32bit Constant for converting from degrees to radians
	const f32 DEGTORAD = PI / 180.0f;

	//! 32bit constant for converting from radians to degrees (formally known as GRAD_PI)
	const f32 RADTODEG   = 180.0f / PI;

	//! 64bit constant for converting from degrees to radians (formally known as GRAD_PI2)
	const f64 DEGTORAD64 = PI64 / 180.0;

	//! 64bit constant for converting from radians to degrees
	const f64 RADTODEG64 = 180.0 / PI64;

	//! Utility function to convert a radian value to degrees
	/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
	\param radians	The radians value to convert to degrees.
	*/
	REALINLINE f32 radToDeg(f32 radians)
	{
		return RADTODEG * radians;
	}

	//! Utility function to convert a radian value to degrees
	/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
	\param radians	The radians value to convert to degrees.
	*/
	REALINLINE f64 radToDeg(f64 radians)
	{
		return RADTODEG64 * radians;
	}

	//! Utility function to convert a degrees value to radians
	/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
	\param degrees	The degrees value to convert to radians.
	*/
	REALINLINE f32 degToRad(f32 degrees)
	{
		return DEGTORAD * degrees;
	}

	//! Utility function to convert a degrees value to radians
	/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
	\param degrees	The degrees value to convert to radians.
	*/
	REALINLINE f64 degToRad(f64 degrees)
	{
		return DEGTORAD64 * degrees;
	}

	REALINLINE f32& surroundRad(f32& degrees)
	{
		while (degrees < 0)
			degrees += TWO_PI;
		while (degrees > TWO_PI)
			degrees -= TWO_PI;
		return degrees;
	}

	REALINLINE f32 surroundRad2(f32 degrees)
	{
		return surroundRad(degrees);
	}

	template<class T, class T1, class T2>
	REALINLINE T surround(T value, T1 tMin, T2 tMax)
	{
		T gap = tMax - tMin;
		while (value < (T)tMin)
			value += gap;
		while (value >= (T)tMax)
			value -= gap;
		assert(value >= (T)tMin && value < (T)tMax);
		return value;
	}
	template <class T, class T1>
	REALINLINE T surround(T idx, T1 size)
	{
		return surround(idx, 0, size);
	}

	//! returns minimum of two values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	REALINLINE const T& min_(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	//! returns minimum of three values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	REALINLINE const T& min_(const T& a, const T& b, const T& c)
	{
		return a < b ? min_(a, c) : min_(b, c);
	}

	//! returns maximum of two values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	REALINLINE const T& max_(const T& a, const T& b)
	{
		return a < b ? b : a;
	}

	//! returns maximum of three values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	REALINLINE const T& max_(const T& a, const T& b, const T& c)
	{
		return a < b ? max_(b, c) : max_(a, c);
	}

	//! returns abs of two values. Own implementation to get rid of STL (VS6 problems)
	template<class T>
	REALINLINE T abs_(const T& a)
	{
		return a < (T)0 ? -a : a;
	}

	//! returns linear interpolation of a and b with ratio t
	//! \return: a if t==0, b if t==1, and the linear interpolation else
	template<class T>
	REALINLINE T lerp(const T& a, const T& b, const f32 t)
	{
		return (T)(a*(1.f-t)) + (b*t);
	}

	//! clamps a value between low and high
	template <class T>
	REALINLINE const T clamp (const T& value, const T& low, const T& high)
	{
		return min_ (max_(value,low), high);
	}
	template <class T, class T1>
	REALINLINE const T clamp (const T& value, T1 size)
	{
		return clamp(value, (T)(0), (T)(size - 1));
	}


	//! swaps the content of the passed parameters
	// Note: We use the same trick as boost and use two template arguments to
	// avoid ambiguity when swapping objects of an Irrlicht type that has not
	// it's own swap overload. Otherwise we get conflicts with some compilers
	// in combination with stl.
	template <class T1, class T2>
	REALINLINE void swap(T1& a, T2& b)
	{
		T1 c(a);
		a = b;
		b = c;
	}

	//! returns if a equals b, taking possible rounding errors into account
	REALINLINE bool equals(const f64 a, const f64 b, const f64 tolerance = ROUNDING_ERROR_f64)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	//! returns if a equals b, taking possible rounding errors into account
	REALINLINE bool equals(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_f32)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	//! returns if a equals b, taking an explicit rounding tolerance into account
	REALINLINE bool equals(const s32 a, const s32 b, const s32 tolerance = ROUNDING_ERROR_Int32)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	//! returns if a equals b, taking an explicit rounding tolerance into account
	REALINLINE bool equals(const u32 a, const u32 b, const s32 tolerance = ROUNDING_ERROR_Int32)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}
	
	REALINLINE bool equals(const u64 a, const u64 b, const u64 tolerance = ROUNDING_ERROR_Int32)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	REALINLINE bool equals(const s64 a, const s64 b, const s64 tolerance = ROUNDING_ERROR_Int32)
	{
		return (a + tolerance >= b) && (a - tolerance <= b);
	}

	//! returns if a equals zero, taking rounding errors into account
	REALINLINE bool iszero(const f64 a, const f64 tolerance = ROUNDING_ERROR_f64)
	{
		return fabs(a) <= tolerance;
	}

	//! returns if a equals zero, taking rounding errors into account
	REALINLINE bool iszero(const f32 a, const f32 tolerance = ROUNDING_ERROR_f32)
	{
		return fabsf(a) <= tolerance;
	}

	//! returns if a equals not zero, taking rounding errors into account
	REALINLINE bool isnotzero(const f32 a, const f32 tolerance = ROUNDING_ERROR_f32)
	{
		return fabsf(a) > tolerance;
	}

	//! returns if a equals zero, taking rounding errors into account
	REALINLINE bool iszero(const s32 a, const s32 tolerance = 0)
	{
		return ( a & 0x7ffffff ) <= tolerance;
	}

	//! returns if a equals zero, taking rounding errors into account
	REALINLINE bool iszero(const u32 a, const u32 tolerance = 0)
	{
		return a <= tolerance;
	}

	inline s32 Int32_min(s32 a, s32 b)
	{
		const s32 mask = (a - b) >> 31;
		return (a & mask) | (b & ~mask);
	}

	inline s32 Int32_max(s32 a, s32 b)
	{
		const s32 mask = (a - b) >> 31;
		return (b & mask) | (a & ~mask);
	}

	inline s32 Int32_clamp (s32 value, s32 low, s32 high)
	{
		return Int32_min(Int32_max(value,low), high);
	}

	/*
		float IEEE-754 bit represenation

		0      0x00000000
		1.0    0x3f800000
		0.5    0x3f000000
		3      0x40400000
		+inf   0x7f800000
		-inf   0xff800000
		+NaN   0x7fc00000 or 0x7ff00000
		in general: number = (sign ? -1:1) * 2^(exponent) * 1.(mantissa bits)
	*/

	typedef union { u32 u; s32 s; f32 f; } inttofloat;

	#define F32_AS_Int32(f)		(*((s32 *) &(f)))
	#define F32_AS_UInt32(f)		(*((u32 *) &(f)))
	#define F32_AS_UInt32_POINTER(f)	( ((u32 *) &(f)))

	#define F32_VALUE_0		0x00000000
	#define F32_VALUE_1		0x3f800000
	#define F32_SIGN_BIT		0x80000000U
	#define F32_EXPON_MANTISSA	0x7FFFFFFFU

	#define	F32_LOWER_0(n)		((n) <  0.0f)
	#define	F32_LOWER_EQUAL_0(n)	((n) <= 0.0f)
	#define	F32_GREATER_0(n)	((n) >  0.0f)
	#define	F32_GREATER_EQUAL_0(n)	((n) >= 0.0f)
	#define	F32_EQUAL_1(n)		((n) == 1.0f)
	#define	F32_EQUAL_0(n)		((n) == 0.0f)
	#define	F32_A_GREATER_B(a,b)	((a) > (b))

	//! conditional set based on mask and arithmetic shift
	REALINLINE u32 if_c_a_else_b ( const s32 condition, const u32 a, const u32 b )
	{
		return ( ( -condition >> 31 ) & ( a ^ b ) ) ^ b;
	}

	//! conditional set based on mask and arithmetic shift
	REALINLINE u16 if_c_a_else_b ( const u8 condition, const u16 a, const u16 b )
	{
		return ( ( -condition >> 15 ) & ( a ^ b ) ) ^ b;
	}

	//! conditional set based on mask and arithmetic shift
	REALINLINE u32 if_c_a_else_0 ( const s32 condition, const u32 a )
	{
		return ( -condition >> 31 ) & a;
	}

	/*
		if (condition) state |= m; else state &= ~m;
	*/
	REALINLINE void setbit_cond ( u32 &state, s32 condition, u32 mask )
	{
		// 0, or any postive to mask
		//Int32 conmask = -condition >> 31;
		state ^= ( ( -condition >> 31 ) ^ state ) & mask;
	}

	REALINLINE f32 round_( f32 x )
	{
		return floorf( x + 0.5f );
	}

	// calculate: sqrt ( x )
	REALINLINE f32 squareroot(const f32 f)
	{
		return sqrtf(f);
	}

	// calculate: sqrt ( x )
	REALINLINE f64 squareroot(const f64 f)
	{
		return sqrt(f);
	}

	// calculate: sqrt ( x )
	REALINLINE s32 squareroot(const s32 f)
	{
		return static_cast<s32>(squareroot(static_cast<f32>(f)));
	}

	// calculate: 1 / sqrt ( x )
	REALINLINE f64 reciprocal_squareroot(const f64 x)
	{
		return 1.0 / sqrt(x);
	}

	// calculate: 1 / sqrtf ( x )
	REALINLINE f32 reciprocal_squareroot(const f32 f)
	{
		return 1.f / sqrtf(f);
	}

	// calculate: 1 / sqrtf( x )
	REALINLINE s32 reciprocal_squareroot(const s32 x)
	{
		return static_cast<s32>(reciprocal_squareroot(static_cast<f32>(x)));
	}

	// calculate: 1 / x
	REALINLINE f32 reciprocal( const f32 f )
	{
		return 1.f / f;
	}

	// calculate: 1 / x
	REALINLINE f64 reciprocal ( const f64 f )
	{
		return 1.0 / f;
	}

	// calculate: 1 / x, low precision allowed
	REALINLINE f32 reciprocal_approxim ( const f32 f )
	{
		return 1.f / f;
	}

	REALINLINE s32 floor32(f32 x)
	{
		return (s32) floorf ( x );
	}

	REALINLINE s32 ceil32( f32 x )
	{
		return (s32) ceilf ( x );
	}

	REALINLINE s32 round32(f32 x)
	{
		return (s32) round_(x);
	}

	inline f32 f32_max3(const f32 a, const f32 b, const f32 c)
	{
		return a > b ? (a > c ? a : c) : (b > c ? b : c);
	}

	inline f32 f32_min3(const f32 a, const f32 b, const f32 c)
	{
		return a < b ? (a < c ? a : c) : (b < c ? b : c);
	}

	inline f32 fract(f32 x)
	{
		return x - floorf(x);
	}

#ifdef QmFastMathTableOn
#include "Math\QcMathTable.hpp"
#endif

} // end namespace QcMath


#define _IRR_DEBUG_BREAK_IF(c) QmAssert((!(c)))

using namespace QcMath;
#include "Math\rand.h"
#include "Math\QcRand.hpp"
#include "Math\QcDimension2d.hpp"
#include "Math\QcVector2d.hpp"
#include "Math\QcLine2d.hpp"
#include "Math\QcRect2d.hpp"
#include "Math\QcCircle2d.hpp"
#include "Math\QcPolygon2d.hpp"
#pragma warning(push)
#pragma warning(disable:4201)
#include "Math\QcMatrix3.hpp"
#include "Math\QcMatrix4.hpp"
#pragma warning(pop)
#include "Math\QcMathNodeInfo.hpp"

typedef QcDimension2d<f32> QcSize;
typedef QcVector2d<f32> QcVector;
typedef QcVector2d<f32> QcPoint;
typedef QcLine2d<f32> QcLine;
typedef QcRect2d<f32> QcRect;
typedef QcCircle2d<f32> QcCircle;
typedef QcPolygon2d<f32> QcPolygon;
typedef QcMatrix3f QcMatrix;

typedef std::vector<QcPoint> QcPointList;
typedef QcPointList::iterator QcPointListIter;
typedef QcPointList::const_iterator QcPointListConstIter;

const QcMatrix gsIdentityMatrix(QcMatrix::EM4CONST_IDENTITY);

QmStrToValueTemplateFun(QcPoint)
{
	f32 ar[2];
	QmAssert2(StrToValueList(psStr, ar, ',') == 2);
	return QcPoint(ar[0], ar[1]); 
}
QmStrToValueTemplateFun(QcSize)
{
	f32 ar[2];
	QmAssert2(StrToValueList(psStr, ar, ',') == 2);
	return QcSize(ar[0], ar[1]);
}