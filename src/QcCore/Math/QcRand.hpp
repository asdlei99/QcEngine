#pragma once

inline int RandomInt()
{
	return int(SPDCRandomInt());
}

//[begin, end)
template<typename T1, typename T2>
inline T1 RandomInt(T1 begin, T2 end)
{
	if (begin < (T1)end)
		return RandomInt()%(end-begin) + begin;
	else if (begin > (T1)end)
		return RandomInt()%(begin - end) + end;
	return begin;
}

//[begin, end]
template<typename T1, typename T2>
inline T1 RandomInt2(T1 begin, T2 end)
{
	if (begin < end)
		return RandomInt()%(end-begin + 1) + begin;
	else if (begin > end)
		return RandomInt()%(begin - end + 1) + end;
	return begin;
}

//0.0~1.0
inline f32 Randomf()
{
	return ((f32)SPDCRandom());
}

inline f32 Randomf(f32 a, f32 b)
{
	return Randomf() * (b - a) + a;
}