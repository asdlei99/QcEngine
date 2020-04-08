#pragma once

#ifdef QmBoostOn
#include "boost\any.hpp"
#include "boost\function.hpp"
#include "boost\bind.hpp"
#endif

#include <string>
#include <vector>
#include <map>
#include <assert.h>

typedef signed char		s8;
typedef signed short	s16;
typedef signed int		s32;
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef unsigned long long	u64;
typedef signed long long s64;
typedef float			f32;
typedef double			f64;

typedef std::string	QcString;

#include "Common\QcArray.hpp"
typedef std::vector<QcString> QcStringList;
typedef QcStringList::iterator QcStringListIter;
typedef QcStringList::const_iterator QcStringListConstIter;

#include "Common\QcNoCaseCompare.hpp"
#ifdef QmBoostOn
typedef boost::any QcVariant;
typedef std::map<QcString, QcVariant, QcNoCaseCompare> QcVariantMap;
typedef QcVariantMap::iterator QcVariantMapIter;
typedef QcVariantMap::const_iterator QcVariantMapConstIter;
#endif

#pragma  pack(push,1)
union QuByte
{
	struct
	{
		u8 b0 : 1;
		u8 b1 : 1;
		u8 b2 : 1;
		u8 b3 : 1;
		u8 b4 : 1;
		u8 b5 : 1;
		u8 b6 : 1;
		u8 b7 : 1;
	};
	u8 byteValue;
};

union QuWord
{
	u16 wordValue;
	QuByte byte[2];
};

union QuUInt32
{
	u32 value32;
	QuByte byte[4];
	u16 word[2];
};
#pragma  pack(pop)