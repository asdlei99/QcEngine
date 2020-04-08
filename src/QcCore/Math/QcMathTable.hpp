#pragma once


template<size_t nTableSize>
class QcFastMathTable
{
public:
	QcFastMathTable()
	{
		m_TrigTableFactor = nTableSize/TWO_PI;
		f32 angle;
		for (int i=0; i<nTableSize; ++i)
		{
			angle = TWO_PI * i/nTableSize;
			m_arSinTable[i] = sin(angle);
			m_arTanTable[i] = tan(angle);
		}
	}
	f32 Sin(f32 fValue) const
	{
		int idx;
		if (fValue >= 0)
		{
			idx = int(fValue * m_TrigTableFactor) % nTableSize;
		}
		else
		{
			idx = nTableSize - (int(-fValue * m_TrigTableFactor) % nTableSize) - 1;
		}
		return m_arSinTable[idx];
	}
	f32 Cos(f32 fValue) const
	{
		return Sin(fValue + HALF_PI);
	}
	f32 Tan(f32 fValue) const
	{
		int idx = int(fValue *= m_TrigTableFactor) % nTableSize;
		return m_arTanTable[idx];
	}
private:
	f32 m_arSinTable[nTableSize];
	f32 m_arTanTable[nTableSize];
	f32 m_TrigTableFactor;
};
const QcFastMathTable<4096> gsFastMathTable;
template<typename T>
REALINLINE T Cos(T fValue)
{
	return (T)gsFastMathTable.Cos(f32(fValue));
}
template<typename T>
REALINLINE T Sin(T fValue)
{
	return (T)gsFastMathTable.Sin(f32(fValue));
}
template<typename T>
REALINLINE T Tan(T fValue)
{
	return (T)gsFastMathTable.Tan(f32(fValue));
}

#define cos Cos
#define sin Sin
#define tan Tan