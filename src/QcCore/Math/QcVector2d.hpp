#pragma once

#include "Math\QcDimension2d.hpp"
//! 2d vector template class with lots of operators and methods.
/** As of Irrlicht 1.6, this class supercedes position2d, which should
be considered deprecated. */
template <class T>
class QcVector2d
{
public:
	//! Default constructor (null vector)
	QcVector2d() : X(0), Y(0) {}
	//! Constructor with two different values
	QcVector2d(T nx, T ny) : X(nx), Y(ny) {}
	//! Constructor with the same value for both members
	explicit QcVector2d(T n) : X(n), Y(n) {}
	//! Copy constructor
	QcVector2d(const QcVector2d<T>& other) : X(other.X), Y(other.Y) {}

	QcVector2d(const QcDimension2d<T>& other) : X(other.Width), Y(other.Height) {}

	QcVector2d(const QcVector2d<T>& firstPt, const QcVector2d<T>& secondPt) : X(secondPt.X - firstPt.X), Y(secondPt.Y - firstPt.Y) {}

	// operators
	QcVector2d<T> operator-() const { return QcVector2d<T>(-X, -Y); }

	QcVector2d<T>& operator=(const QcVector2d<T>& other) { X = other.X; Y = other.Y; return *this; }

	QcVector2d<T>& operator=(const QcDimension2d<T>& other) { X = other.Width; Y = other.Height; return *this; }

	QcVector2d<T> operator+(const QcVector2d<T>& other) const { return QcVector2d<T>(X + other.X, Y + other.Y); }
	QcVector2d<T> operator+(const QcDimension2d<T>& other) const { return QcVector2d<T>(X + other.Width, Y + other.Height); }
	QcVector2d<T>& operator+=(const QcVector2d<T>& other) { X+=other.X; Y+=other.Y; return *this; }
	QcVector2d<T> operator+(const T v) const { return QcVector2d<T>(X + v, Y + v); }
	QcVector2d<T>& operator+=(const T v) { X+=v; Y+=v; return *this; }
	QcVector2d<T>& operator+=(const QcDimension2d<T>& other) { X += other.Width; Y += other.Height; return *this;  }

	QcVector2d<T> operator-(const QcVector2d<T>& other) const { return QcVector2d<T>(X - other.X, Y - other.Y); }
	QcVector2d<T> operator-(const QcDimension2d<T>& other) const { return QcVector2d<T>(X - other.Width, Y - other.Height); }
	QcVector2d<T>& operator-=(const QcVector2d<T>& other) { X-=other.X; Y-=other.Y; return *this; }
	QcVector2d<T> operator-(const T v) const { return QcVector2d<T>(X - v, Y - v); }
	QcVector2d<T>& operator-=(const T v) { X-=v; Y-=v; return *this; }
	QcVector2d<T>& operator-=(const QcDimension2d<T>& other) { X -= other.Width; Y -= other.Height; return *this;  }

	QcVector2d<T> operator*(const QcVector2d<T>& other) const { return QcVector2d<T>(X * other.X, Y * other.Y); }
	QcVector2d<T>& operator*=(const QcVector2d<T>& other) { X*=other.X; Y*=other.Y; return *this; }
	QcVector2d<T> operator*(const T v) const { return QcVector2d<T>(X * v, Y * v); }
	QcVector2d<T>& operator*=(const T v) { X*=v; Y*=v; return *this; }

	QcVector2d<T> operator/(const QcVector2d<T>& other) const { return QcVector2d<T>(X / other.X, Y / other.Y); }
	QcVector2d<T>& operator/=(const QcVector2d<T>& other) { X/=other.X; Y/=other.Y; return *this; }
	QcVector2d<T> operator/(const T v) const { return QcVector2d<T>(X / v, Y / v); }
	QcVector2d<T>& operator/=(const T v) { X/=v; Y/=v; return *this; }

	//! sort in order X, Y. Equality with rounding tolerance.
	bool operator<=(const QcVector2d<T>&other) const
	{
		return 	(X<other.X || QcMath::equals(X, other.X)) ||
			(QcMath::equals(X, other.X) && (Y<other.Y || QcMath::equals(Y, other.Y)));
	}

	//! sort in order X, Y. Equality with rounding tolerance.
	bool operator>=(const QcVector2d<T>&other) const
	{
		return 	(X>other.X || QcMath::equals(X, other.X)) ||
			(QcMath::equals(X, other.X) && (Y>other.Y || QcMath::equals(Y, other.Y)));
	}

	//! sort in order X, Y. Difference must be above rounding tolerance.
	bool operator<(const QcVector2d<T>&other) const
	{
		return 	(X<other.X && !QcMath::equals(X, other.X)) ||
			(QcMath::equals(X, other.X) && Y<other.Y && !QcMath::equals(Y, other.Y));
	}

	//! sort in order X, Y. Difference must be above rounding tolerance.
	bool operator>(const QcVector2d<T>&other) const
	{
		return 	(X>other.X && !QcMath::equals(X, other.X)) ||
			(QcMath::equals(X, other.X) && Y>other.Y && !QcMath::equals(Y, other.Y));
	}

	bool operator==(const QcVector2d<T>& other) const { return equals(other); }
	bool operator!=(const QcVector2d<T>& other) const { return !equals(other); }

	// functions

	//! Checks if this vector equals the other one.
	/** Takes floating point rounding errors into account.
	\param other Vector to compare with.
	\return True if the two vector are (almost) equal, else false. */
	bool equals(const QcVector2d<T>& other) const
	{
		return QcMath::equals(X, other.X) && QcMath::equals(Y, other.Y);
	}

	QcVector2d<T>& set(T nx, T ny) {X=nx; Y=ny; return *this; }
	QcVector2d<T>& set(const QcVector2d<T>& p) { X=p.X; Y=p.Y; return *this; }

	//! Gets the length of the vector.
	/** \return The length of the vector. */
	inline T getLength() const { return squareroot( X*X + Y*Y ); }

	//! Get the squared length of this vector
	/** This is useful because it is much faster than getLength().
	\return The squared length of the vector. */
	inline T getLengthSQ() const { return X*X + Y*Y; }

	inline int lenCompare(T dist) const 
	{ 
		T lenSQ = getLengthSQ(); 
		T distSQ = dist * dist; 
		return QcMath::equals(lenSQ, distSQ) ?  (0) :  (lenSQ > distSQ  ?  1 : -1);
	}

	inline bool longer(T dist) const
	{
		if (dist > abs(X) || dist > abs(Y))
			return false;
		return getLengthSQ() > dist * dist;
	}

	inline bool smaller(T dist) const
	{
		if (dist < abs(X) || dist < abs(Y))
			return false;
		return getLengthSQ() < dist * dist;
	}


	//! Get the dot product of this vector with another.
	/** \param other Other vector to take dot product with.
	\return The dot product of the two vectors. */
	T dotProduct(const QcVector2d<T>& other) const
	{
		return X*other.X + Y*other.Y;
	}

	//! Gets distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Distance from other point. */
	T getDistanceFrom(const QcVector2d<T>& other) const
	{
		return QcVector2d<T>(X - other.X, Y - other.Y).getLength();
	}

	//! Returns squared distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Squared distance from other point. */
	T getDistanceFromSQ(const QcVector2d<T>& other) const
	{
		return (X - other.X) * (X - other.X) + (Y - other.Y) * (Y - other.Y);
		//return QcVector2d<T>(X - other.X, Y - other.Y).getLengthSQ();
	}

	//! rotates the point anticlockwise around a center by an amount of degrees.
	/** \param degrees Amount of degrees to rotate by, anticlockwise.
	\param center Rotation center.
	\return This vector after transformation. */
	QcVector2d<T>& rotateBy(f64 degrees)
	{
		return rotateByRad(degrees * DEGTORAD64);
	}
	QcVector2d<T>& rotateByRad(f64 rad)
	{
		return rotateBy(cos(rad), sin(rad));
	}
	QcVector2d<T>& rotateBy(const f64 cs, const f64 sn)
	{
		set((T)(X*cs - Y*sn), (T)(X*sn + Y*cs));
		return *this;
	}
	QcVector2d<T>& rotateBy(f64 degrees, const QcVector2d<T>& center)
	{
		return rotateByRad(degrees * DEGTORAD64, center);
	}
	QcVector2d<T>& rotateByRad(f64 rad, const QcVector2d<T>& center)
	{
		return rotateBy(cos(rad), sin(rad), center);
	}
	QcVector2d<T>& rotateBy(const f64 cs, const f64 sn, const QcVector2d<T>& center)
	{
		X -= center.X;
		Y -= center.Y;

		rotateBy(cs, sn);

		X += center.X;
		Y += center.Y;
		return *this;
	}

	//! Normalize the vector.
	/** The null vector is left untouched.
	\return Reference to this vector, after normalization. */
	QcVector2d<T>& normalize()
	{
		f32 length = (f32)(X*X + Y*Y);
		if ( length == 0 )
			return *this;
		length = reciprocal_squareroot ( length );
		X = (T)(X * length);
		Y = (T)(Y * length);
		return *this;
	}

	QcVector2d<T>& normalize(f32 length)
	{
		if ( length == 0 )
			return *this;
		length = 1.0f/length;
		X = (T)(X * length);
		Y = (T)(Y * length);
		return *this;
	}

	//! Calculates the angle of this vector in degrees in the trigonometric sense.
	/** 0 is to the right (3 o'clock), values increase counter-clockwise.
	This method has been suggested by Pr3t3nd3r.
	\return Returns a value between 0 and 360. */
	f64 getAngleTrig() const
	{
		if (Y == 0)
			return X < 0 ? 180 : 0;
		else
			if (X == 0)
				return Y < 0 ? 270 : 90;

		if ( Y > 0)
			if (X > 0)
				return atan((f64)Y/(f64)X) * RADTODEG64;
			else
				return 180.0-atan((f64)Y/-(f64)X) * RADTODEG64;
		else
			if (X > 0)
				return 360.0-atan(-(f64)Y/(f64)X) * RADTODEG64;
			else
				return 180.0+atan(-(f64)Y/-(f64)X) * RADTODEG64;
	}
	
	f64 getRadAngleTrig() const
	{
		if (Y == 0)
			return X < 0 ? PI64 : 0.f;
		else
			if (X == 0)
				return Y < 0 ? PI64 + HALF_PI64 : HALF_PI64;

		if ( Y > 0)
			if (X > 0)
				return atan((f64)Y/(f64)X);
			else
				return PI64-atan((f64)Y/-(f64)X);
		else
			if (X > 0)
				return TWO_PI64-atan(-(f64)Y/(f64)X);
			else
				return PI64+atan(-(f64)Y/-(f64)X);
		//return 0.f;
	}


	//! Calculates the angle of this vector in degrees in the counter trigonometric sense.
	/** 0 is to the right (3 o'clock), values increase clockwise.
	\return Returns a value between 0 and 360. */
	inline f64 getAngle() const
	{
		if (Y == 0) // corrected thanks to a suggestion by Jox
			return X < 0 ? 180 : 0;
		else if (X == 0)
			return Y < 0 ? 90 : 270;

		// don't use getLength here to avoid precision loss with Int32 vectors
		// avoid floating-point trouble as sqrt(y*y) is occasionally larger than y, so clamp
		const f64 tmp = clamp(Y / sqrt((f64)(X*X + Y*Y)), -1.0, 1.0);
		const f64 angle = atan(squareroot(1 - tmp*tmp) / tmp) * RADTODEG64;

		if (X>0 && Y>0)
			return angle + 270;
		else
			if (X>0 && Y<0)
				return angle + 90;
			else
				if (X<0 && Y<0)
					return 90 - angle;
				else
					if (X<0 && Y>0)
						return 270 - angle;

		return angle;
	}

	//! Calculates the angle between this vector and another one in degree.
	/** \param b Other vector to test with.
	\return Returns a value between 0 and 90. */
	inline f64 getAngleWith(const QcVector2d<T>& b) const
	{
		f64 tmp = X*b.X + Y*b.Y;

		if (tmp == 0.0)
			return 90.0;

		tmp = tmp / squareroot((f64)((X*X + Y*Y) * (b.X*b.X + b.Y*b.Y)));
		if (tmp < 0.0)
			tmp = -tmp;
		if ( tmp > 1.0 ) //   avoid floating-point trouble
			tmp = 1.0;

		return atan(sqrt(1 - tmp*tmp) / tmp) * RADTODEG64;
	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line.
	\param begin Beginning vector to compare between.
	\param end Ending vector to compare between.
	\return True if this vector is between begin and end, false if not. */
	bool isBetweenPoints(const QcVector2d<T>& begin, const QcVector2d<T>& end) const
	{
		if (begin.X != end.X)
		{
			return ((begin.X <= X && X <= end.X) ||
				(begin.X >= X && X >= end.X));
		}
		else
		{
			return ((begin.Y <= Y && Y <= end.Y) ||
				(begin.Y >= Y && Y >= end.Y));
		}
	}

	//! Creates an interpolated vector between this vector and another vector.
	/** \param other The other vector to interpolate with.
	\param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	\return An interpolated vector.  This vector is not modified. */
	QcVector2d<T> getInterpolated(const QcVector2d<T>& other, f64 d) const
	{
		f64 inv = 1.0f - d;
		return QcVector2d<T>((T)(other.X*inv + X*d), (T)(other.Y*inv + Y*d));
	}

	//! Creates a quadratically interpolated vector between this and two other vectors.
	/** \param v2 Second vector to interpolate with.
	\param v3 Third vector to interpolate with (maximum at 1.0f)
	\param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
	Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
	\return An interpolated vector. This vector is not modified. */
	QcVector2d<T> getInterpolated_quadratic(const QcVector2d<T>& v2, const QcVector2d<T>& v3, f64 d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const f64 inv = 1.0f - d;
		const f64 mul0 = inv * inv;
		const f64 mul1 = 2.0f * d * inv;
		const f64 mul2 = d * d;

		return QcVector2d<T> ( (T)(X * mul0 + v2.X * mul1 + v3.X * mul2),
			(T)(Y * mul0 + v2.Y * mul1 + v3.Y * mul2));
	}

	//! Sets this vector to the linearly interpolated vector between a and b.
	/** \param a first vector to interpolate with, maximum at 1.0f
	\param b second vector to interpolate with, maximum at 0.0f
	\param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	*/
	QcVector2d<T>& interpolate(const QcVector2d<T>& a, const QcVector2d<T>& b, f64 d)
	{
		X = (T)((f64)b.X + ( ( a.X - b.X ) * d ));
		Y = (T)((f64)b.Y + ( ( a.Y - b.Y ) * d ));
		return *this;
	}

	//! X coordinate of vector.
	T X;

	//! Y coordinate of vector.
	T Y;
};

//! Typedef for f32 2d vector.
typedef QcVector2d<f32> QcVector2df;

//! Typedef for integer 2d vector.
typedef QcVector2d<s32> QcVector2di;

template<class S, class T>
QcVector2d<T> operator*(const S scalar, const QcVector2d<T>& vector) { return vector*scalar; }

