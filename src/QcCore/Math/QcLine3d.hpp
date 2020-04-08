#pragma once

#include "Math\QcVector3d.hpp"

//! 3D line between two points with intersection methods.
template <class T>
class QcLine3d
{
public:

	//! Default constructor
	/** line from (0,0,0) to (1,1,1) */
	QcLine3d() : start(0,0,0), end(1,1,1) {}
	//! Constructor with two points
	QcLine3d(T xa, T ya, T za, T xb, T yb, T zb) : start(xa, ya, za), end(xb, yb, zb) {}
	//! Constructor with two points as vectors
	QcLine3d(const QcVector3d<T>& start, const QcVector3d<T>& end) : start(start), end(end) {}

	// operators

	QcLine3d<T> operator+(const QcVector3d<T>& point) const { return QcLine3d<T>(start + point, end + point); }
	QcLine3d<T>& operator+=(const QcVector3d<T>& point) { start += point; end += point; return *this; }

	QcLine3d<T> operator-(const QcVector3d<T>& point) const { return QcLine3d<T>(start - point, end - point); }
	QcLine3d<T>& operator-=(const QcVector3d<T>& point) { start -= point; end -= point; return *this; }

	bool operator==(const QcLine3d<T>& other) const
	{ return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
	bool operator!=(const QcLine3d<T>& other) const
	{ return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}

	// functions
	//! Set this line to a new line going through the two points.
	void setLine(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb)
	{start.set(xa, ya, za); end.set(xb, yb, zb);}
	//! Set this line to a new line going through the two points.
	void setLine(const QcVector3d<T>& nstart, const QcVector3d<T>& nend)
	{start.set(nstart); end.set(nend);}
	//! Set this line to new line given as parameter.
	void setLine(const QcLine3d<T>& line)
	{start.set(line.start); end.set(line.end);}

	//! Get length of line
	/** \return Length of line. */
	T getLength() const { return start.getDistanceFrom(end); }

	//! Get squared length of line
	/** \return Squared length of line. */
	T getLengthSQ() const { return start.getDistanceFromSQ(end); }

	//! Get middle of line
	/** \return Center of line. */
	QcVector3d<T> getMiddle() const
	{
		return (start + end) * (T)0.5;
	}

	//! Get vector of line
	/** \return vector of line. */
	QcVector3d<T> getVector() const
	{
		return end - start;
	}

	//! Check if the given point is between start and end of the line.
	/** Assumes that the point is already somewhere on the line.
	\param point The point to test.
	\return True if point is on the line between start and end, else false.
	*/
	bool isPointBetweenStartAndEnd(const QcVector3d<T>& point) const
	{
		return point.isBetweenPoints(start, end);
	}

	//! Get the closest point on this line to a point
	/** \param point The point to compare to.
	\return The nearest point which is part of the line. */
	QcVector3d<T> getClosestPoint(const QcVector3d<T>& point) const
	{
		QcVector3d<T> c = point - start;
		QcVector3d<T> v = end - start;
		T d = (T)v.getLength();
		v /= d;
		T t = v.dotProduct(c);

		if (t < (T)0.0)
			return start;
		if (t > d)
			return end;

		v *= t;
		return start + v;
	}

	//! Check if the line intersects with a shpere
	/** \param sorigin: Origin of the shpere.
	\param sradius: Radius of the sphere.
	\param outdistance: The distance to the first intersection point.
	\return True if there is an intersection.
	If there is one, the distance to the first intersection point
	is stored in outdistance. */
	bool getIntersectionWithSphere(QcVector3d<T> sorigin, T sradius, f64& outdistance) const
	{
		const QcVector3d<T> q = sorigin - start;
		T c = q.getLength();
		T v = q.dotProduct(getVector().normalize());
		T d = sradius * sradius - (c*c - v*v);

		if (d < 0.0)
			return false;

		outdistance = v - squareroot ( d );
		return true;
	}

	// member variables

	//! Start point of line
	QcVector3d<T> start;
	//! End point of line
	QcVector3d<T> end;
};

//! Typedef for an f32 line.
typedef QcLine3d<f32> QcLine3df;
//! Typedef for an integer line.
typedef QcLine3d<s32> QcLine3di;
