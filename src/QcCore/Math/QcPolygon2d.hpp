#pragma once
#include "Math\QcVector2d.hpp"
#include "Math\QcLine2d.hpp"

template <class T>
class QcPolygon2d
{
public:
	QcPolygon2d() {}
	QcPolygon2d(const QcVector2d<T>* pPtList, int nCount) 
	{
		assign(pPtList, nCount);
	}
	QcPolygon2d(const std::vector<QcVector2d<T>>& pointList) : m_pointList(pointList)
	{}
	inline void assign(const std::vector<QcVector2d<T>>& pointList)
	{
		m_pointList = pointList;
	}
	inline void assign(const QcVector2d<T>* pPtList, int nCount)
	{
		m_pointList.assign(pPtList, pPtList + nCount);
	}
	inline size_t Size() const
	{
		return m_pointList.size();
	}
	inline const QcVector2d<T>& operator[](int index) const
	{
		return m_pointList[index];
	}
	inline QcVector2d<T>& operator[](int index)
	{
		return m_pointList[index];
	}

	int intersectWith(const QcLine2d<T>& seq, QcVector2d<T>& firstPt, QcVector2d<T>* sccondPt = NULL) const
	{
		int iCount = 0;
		int nSize = m_pointList.size();
		if (nSize > 0)
		{
			QcVector2d<T> point;
			int i=0;
			for (i=0; i<nSize - 1; ++i)
			{
				if (seq.intersectWith(m_pointList[i], m_pointList[i+1], point))
				{
					++iCount;
					if (iCount == 1)
					{
						firstPt = point;
						if (sccondPt == NULL)
							break;
					}
					else
					{
						*sccondPt = point;
						break;
					}
				}
			}
			if ((iCount == 0 || (iCount == 1 &&  sccondPt != NULL)) && seq.intersectWith(m_pointList[i], m_pointList[0], point))
			{
				++iCount;
				if (iCount == 1)
					firstPt = point;
				else
					*sccondPt = point;
			}
		}
		return iCount;
	}

	bool intersectWith(const QcLine2d<T>& seq) const
	{
		int nSize = m_pointList.size();
		if (nSize <= 0)
			return false;

		const QcPointList& pointList = m_pointList;
		QcPointListConstIter preIter(pointList.begin());
		QcPointListConstIter endIter(pointList.end());
		QcPointListConstIter iter = preIter;
		++iter;
		QcLine2d<T> line;
		for (; iter != endIter; ++iter)
		{
			line.start = *preIter;
			line.end = *iter;
			if (seq.intersectWith(line))
				return true;
			preIter = iter;
		}
		line.start = *preIter;
		line.end = pointList.front();
		return seq.intersectWith(line);
	}

	bool intersectWith(const QcCircle2d<T>& circle) const
	{
		//是否有交点落入圆内
		const QcPointList& pointList = m_pointList;
		QcPointListConstIter iter(pointList.begin());
		QcPointListConstIter endIter(pointList.end());
		for (; iter != endIter; ++iter)
		{
			if (circle.Include(*iter))
				return true;
		}

		if (isPointInside(circle.m_vPos))
			return true;

		//圆心点到各边的垂直交点是否落入圆内
		QcPointListConstIter preIter(pointList.begin());
		QcPointListConstIter preEndIter(pointList.end());
		iter = preIter;
		++iter;
		QcLine2d<T> line;
		for (; iter != preEndIter; ++iter)
		{
			line.start = *preIter;
			line.end = *iter;
			if (circle.Include(line.getClosestPoint(circle.GetPos())))
				return true;
			preIter = iter;
		}
		line.start = *preIter;
		line.end = pointList.front();
		return circle.Include(line.getClosestPoint(circle.GetPos()));
	}

	bool intersectWith(const QcPolygon2d<T>& polygon) const
	{
		int nSize = polygon.m_pointList.size();
		if (nSize <= 0 || m_pointList.size() < 0)
			return false;

		const QcPointList& pointList = polygon.m_pointList;
		QcPointListConstIter preIter(pointList.begin());
		QcPointListConstIter endIter(pointList.end());
		QcPointListConstIter iter = preIter;
		++iter;
		QcLine2d<T> line;
		for (; iter != endIter; ++iter)
		{
			line.start = *preIter;
			line.end = *iter;
			if (intersectWith(line))
				return true;
			preIter = iter;
		}
		line.start = *preIter;
		line.end = pointList.front();
		return intersectWith(line);
	}
	
	bool isPointInside(const QcVector2d<T>& pt) const
	{
		int nSize = m_pointList.size();
		if (nSize <= 0)
			return false;

		int iNum = 0;
		QcLine2d<T> seq(pt, QcVector2d<T>(gfMax, pt.Y));
		int i=0;
		for (i=0; i<nSize - 1; ++i)
		{
			if (seq.intersectWith(m_pointList[i], m_pointList[i+1]))
				++iNum;
		}
		if (seq.intersectWith(m_pointList[i], m_pointList[0]))
			++iNum;
		return iNum % 2 != 0; 
	}

	inline void Clear()
	{
		m_pointList.clear();
	}

	inline const std::vector<QcVector2d<T>>& getPointList() const
	{
		return m_pointList;
	}

	QcRect2d<T> GetBox() const
	{
		QcRect2d<T> box;
		int nSize = m_pointList.size();
		if (nSize > 0)
		{
			box += m_pointList[0];
			for (int i=0; i<nSize; ++i)
			{
				box.addInternalPoint(m_pointList[i]);
			}
		}
		return box;
	}

	template<class _Fn1>
	inline int ForEachLine(_Fn1 _Func)
	{
		int iSuccee = -1;
		int nSize = m_pointList.size();
		if (nSize)
		{
			iSuccee = 0;
			for (int i=0; i<nSize - 1; ++i)
			{
				if (!_Func(QcLine2d<T>(m_pointList[i], m_pointList[i+1])))
					break;
				else
					++iSuccee;
			}
			if (i < nSize - 1)
			{
				if (_Func(m_pointList[i], m_pointList[0]))
					++iSuccee;
			}	
		}
		return iSuccee;
	}
private:
	std::vector<QcVector2d<T>> m_pointList;
};

typedef QcPolygon2d<f32> QcPolygon2df;
typedef QcPolygon2d<s32> QcPolygon2di;