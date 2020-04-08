#pragma once

#include "Math\QcDimension2d.hpp"
#include "Math\QcVector2d.hpp"


template <class T>
class QcRect2d
{
public:
	QcRect2d() : _BottomLeftPt(0,0), _TopRightPt(0,0) {}

	QcRect2d(T x, T y, T x2, T y2)
		: _BottomLeftPt(x,y), _TopRightPt(x2,y2) {}

	QcRect2d(const QcVector2d<T>& bottomLeftPt, const QcVector2d<T>& topRightPt)
		: _BottomLeftPt(bottomLeftPt), _TopRightPt(topRightPt) {}

	QcRect2d(const QcVector2d<T>& center, T width, T height)
		: _BottomLeftPt(center.X - width * 0.5f, center.Y - height * 0.5f), _TopRightPt(center.X + width * 0.5f, center.Y + height * 0.5f) {}

	QcRect2d(T halfWidth, T halfHeight, const QcVector2d<T>& center)
		: _BottomLeftPt(center.X - halfWidth, center.Y - halfHeight), _TopRightPt(center.X + halfWidth, center.Y + halfHeight) {}

	template <class U>
	QcRect2d(const QcVector2d<T>& pos, const QcDimension2d<U>& size)
		: _BottomLeftPt(pos), _TopRightPt(pos.X + size.Width, pos.Y + size.Height) {}

	
	QcRect2d<T>& Expand(const QcVector2d<T>& size)
	{
		_BottomLeftPt -= size;
		_TopRightPt += size;
		return *this;
	}
	QcRect2d<T> operator+(const QcVector2d<T>& pos) const
	{
		QcRect2d<T> ret(*this);
		return ret+=pos;
	}

	QcRect2d<T>& operator+=(const QcVector2d<T>& pos)
	{
		_BottomLeftPt += pos;
		_TopRightPt += pos;
		return *this;
	}

	QcRect2d<T> operator-(const QcVector2d<T>& pos) const
	{
		QcRect2d<T> ret(*this);
		return ret-=pos;
	}

	QcRect2d<T>& operator-=(const QcVector2d<T>& pos)
	{
		_BottomLeftPt -= pos;
		_TopRightPt -= pos;
		return *this;
	}

	bool operator==(const QcRect2d<T>& other) const
	{
		return (_BottomLeftPt == other._BottomLeftPt &&
			_TopRightPt == other._TopRightPt);
	}

	//! inequality operator
	bool operator!=(const QcRect2d<T>& other) const
	{
		return (_BottomLeftPt != other._BottomLeftPt ||
			_TopRightPt != other._TopRightPt);
	}

	//! compares size of rectangles
	bool operator<(const QcRect2d<T>& other) const
	{
		return getArea() < other.getArea();
	}

	//! Returns size of rectangle
	T getArea() const
	{
		return getWidth() * getHeight();
	}

	//! Returns if a 2d point is within this rectangle.
	/** \param pos Position to test if it lies within this rectangle.
	\return True if the position is within the rectangle, false if not. */
	bool isPointInside(const QcVector2d<T>& pos) const
	{
		return (_BottomLeftPt.X <= pos.X &&
			_BottomLeftPt.Y <= pos.Y &&
			_TopRightPt.X >= pos.X &&
			_TopRightPt.Y >= pos.Y);
	}
	int IntersectWith(const QcLine2d<T>& line, QcVector2d<T>* pPos1 = NULL, QcVector2d<T>* pPos2 = NULL) const
	{
		const QcVector2d<T>& vLeftBottom = LeftBottom();
		QcVector2d<T> vRightBottom = RightBottom();
		const QcVector2d<T>& vRightTop = RightTop();
		QcVector2d<T> vLeftTop = LeftTop();

		if (pPos1 == NULL && pPos2 == NULL)
		{
			return (line.intersectWith(vLeftBottom, vRightBottom) 
				|| line.intersectWith(vRightBottom, vRightTop)
				|| line.intersectWith(vRightTop, vLeftTop)
				|| line.intersectWith(vLeftTop, vLeftBottom)) ? 1 : 0;
		}
		int iNum = 0;
		int nIntersectNum = pPos2 == NULL ? 1 : 2;
		if (line.intersectWith(vLeftBottom, vRightBottom, *pPos1))
		{
			++iNum;
		}
		if (iNum < nIntersectNum && line.intersectWith(vRightBottom, vRightTop, (iNum == 0 ? *pPos1 : *pPos2)))
		{
			++iNum;
		}
		if (iNum < nIntersectNum && line.intersectWith(vRightTop, vLeftTop, (iNum == 0 ? *pPos1 : *pPos2)))
		{
			++iNum;
		}
		if (iNum < nIntersectNum && line.intersectWith(vLeftTop, vLeftBottom, (iNum == 0 ? *pPos1 : *pPos2)))
		{
			++iNum;
		}
		if (iNum == 2)
		{
			if (QcVector2d<T>(line.start, line.end).dotProduct(QcVector2d<T>(*pPos1, *pPos2)) < 0.f)
			{
				QcMath::swap(*pPos1, *pPos2);
			}
		}
		return iNum;
	}
	int Position(const QcVector2d<T>& pos) const
	{
		if (pos.X < left())
		{
			if (pos.Y > top())
				return 1;
			else if (pos.Y <= top() && pos.Y >= bottom())
				return 2;
			else
				return 3;

		}
		else if (pos.X >= left() && pos.X <= right())
		{
			if (pos.Y > top())
				return 4;
			else if (pos.Y <= top() && pos.Y >= bottom())
				return 0;
			else
				return 5;
		}
		else
		{
			if (pos.Y > top())
				return 6;
			else if (pos.Y <= top() && pos.Y >= bottom())
				return 7;
			else
				return 8;
		}
		//return 0;
	}

	inline T left() const{ return _BottomLeftPt.X;}
	inline T bottom() const {return _BottomLeftPt.Y; }
	inline T right() const {return _TopRightPt.X; }
	inline T top() const {return _TopRightPt.Y;}
	inline const QcVector2d<T> LeftTop() const {return QcVector2d<T>(left(), top()); }
	inline const QcVector2d<T>& LeftBottom() const {return _BottomLeftPt; }
	inline const QcVector2d<T> RightBottom() const {return QcVector2d<T>(right(), bottom()); }
	inline const QcVector2d<T>& RightTop() const {return _TopRightPt; }

	bool intersect(const QcRect2d<T>& other) const
	{
		return left() < other.right() && other.left() < right() &&  bottom() < other.top() && other.bottom() < other.top();
	}

	void unionRect(const QcRect2d<T>& other)
	{
		_BottomLeftPt.X = min(other.left(), left());
		_BottomLeftPt.Y = min(other.bottom(), bottom());
		_TopRightPt.X = max(other.right(), right());
		_TopRightPt.Y = max(other.top(), top());
	}

	//! Clips this rectangle with another one.
	/** \param other Rectangle to clip with */
	void clipAgainst(const QcRect2d<T>& other)
	{
		_BottomLeftPt.X = max(other.left(), left());
		_BottomLeftPt.Y = max(other.bottom(), bottom());
		_TopRightPt.X = min(other.right(), right());
		_TopRightPt.Y = min(other.top(), top());
	}

	//! Get width of rectangle.
	T getWidth() const
	{
		return _TopRightPt.X - _BottomLeftPt.X;
	}

	//! Get height of rectangle.
	T getHeight() const
	{
		return _TopRightPt.Y - _BottomLeftPt.Y;
	}

	//! If the lower right corner of the rect is smaller then the upper left, the points are swapped.
	void repair()
	{
		if (_TopRightPt.X < _BottomLeftPt.X)
		{
			T t = _TopRightPt.X;
			_TopRightPt.X = _BottomLeftPt.X;
			_BottomLeftPt.X = t;
		}

		if (_TopRightPt.Y < _BottomLeftPt.Y)
		{
			T t = _TopRightPt.Y;
			_TopRightPt.Y = _BottomLeftPt.Y;
			_BottomLeftPt.Y = t;
		}
	}

	//! Returns if the rect is valid to draw.
	/** It would be invalid if the UpperLeftCorner is lower or more
	right than the LowerRightCorner. */
	bool isValid() const
	{
		return ((_TopRightPt.X >= _BottomLeftPt.X) &&
			(_TopRightPt.Y >= _BottomLeftPt.Y));
	}

	//! Get the center of the rectangle
	QcVector2d<T> getCenter() const
	{
		return QcVector2d<T>(
			(_BottomLeftPt.X + _TopRightPt.X) / 2,
			(_BottomLeftPt.Y + _TopRightPt.Y) / 2);
	}

	//! Get the dimensions of the rectangle
	QcVector2d<T> getSize() const
	{
		return QcVector2d<T>(getWidth(), getHeight());
	}

	//! Adds a point to the rectangle
	/** Causes the rectangle to grow bigger if point is outside of
	the box
	\param p Point to add to the box. */
	void addInternalPoint(const QcVector2d<T>& p)
	{
		addInternalPoint(p.X, p.Y);
	}

	//! Adds a point to the bounding rectangle
	/** Causes the rectangle to grow bigger if point is outside of
	the box
	\param x X-Coordinate of the point to add to this box.
	\param y Y-Coordinate of the point to add to this box. */
	void addInternalPoint(T x, T y)
	{
		if (x>_TopRightPt.X)
			_TopRightPt.X = x;
		if (y>_TopRightPt.Y)
			_TopRightPt.Y = y;

		if (x<_BottomLeftPt.X)
			_BottomLeftPt.X = x;
		if (y<_BottomLeftPt.Y)
			_BottomLeftPt.Y = y;
	}

	//! Upper left corner
	QcVector2d<T> _BottomLeftPt;
	//! Lower right corner
	QcVector2d<T> _TopRightPt;
};

typedef QcRect2d<f32> QcRect2df;
typedef QcRect2d<s32> QcRect2di;