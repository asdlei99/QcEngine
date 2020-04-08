#pragma once
#include "Math\QcVector2d.hpp"

//! 2D line between two points with intersection methods.
template <class T>
class QcLine2d
{
	public:
		//! Default constructor for line going from (0,0) to (1,1).
		QcLine2d() : start(0,0), end(1,1) {}
		//! Constructor for line between the two points.
		QcLine2d(T xa, T ya, T xb, T yb) : start(xa, ya), end(xb, yb) {}
		//! Constructor for line between the two points given as vectors.
		QcLine2d(const QcVector2d<T>& start, const QcVector2d<T>& end) : start(start), end(end) {}
		//! Copy constructor.
		QcLine2d(const QcLine2d<T>& other) : start(other.start), end(other.end) {}

		// operators

		QcLine2d<T> operator+(const QcVector2d<T>& point) const { return QcLine2d<T>(start + point, end + point); }
		QcLine2d<T>& operator+=(const QcVector2d<T>& point) { start += point; end += point; return *this; }

		QcLine2d<T> operator-(const QcVector2d<T>& point) const { return QcLine2d<T>(start - point, end - point); }
		QcLine2d<T>& operator-=(const QcVector2d<T>& point) { start -= point; end -= point; return *this; }

		bool operator==(const QcLine2d<T>& other) const
		{ return (start==other.start && end==other.end) || (end==other.start && start==other.end);}
		bool operator!=(const QcLine2d<T>& other) const
		{ return !(start==other.start && end==other.end) || (end==other.start && start==other.end);}

		// functions
		//! Set this line to new line going through the two points.
		void setLine(const T& xa, const T& ya, const T& xb, const T& yb){start.set(xa, ya); end.set(xb, yb);}
		//! Set this line to new line going through the two points.
		void setLine(const QcVector2d<T>& nstart, const QcVector2d<T>& nend){start.set(nstart); end.set(nend);}
		//! Set this line to new line given as parameter.
		void setLine(const QcLine2d<T>& line){start.set(line.start); end.set(line.end);}

		//! Get length of line
		/** \return Length of the line. */
		f64 getLength() const { return start.getDistanceFrom(end); }

		//! Get squared length of the line
		/** \return Squared length of line. */
		T getLengthSQ() const { return start.getDistanceFromSQ(end); }

		//! Get middle of the line
		/** \return center of the line. */
		QcVector2d<T> getMiddle() const
		{
			return (start + end) * (T)0.5;
		}

		//! Get the vector of the line.
		/** \return The vector of the line. */
		QcVector2d<T> getVector() const { return QcVector2d<T>(end.X - start.X, end.Y - start.Y); }

		//! Tests if this line intersects with another line.
		/** \param l: Other line to test intersection with.
		\param out: If there is an intersection, the location of the
		intersection will be stored in this vector.
		\return True if there is an intersection, false if not. */
		bool intersectWith(const QcLine2d<T>& line, QcVector2d<T>& out) const
		{
			return intersectWith(line.start, line.end, out);
		}
		bool intersectWith(const QcVector2d<T>& start2, const QcVector2d<T>& end2, QcVector2d<T>& out) const
		{
			// Uses the method given at:
			// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/ 
			const f32 commonDenominator = (f32)((end2.Y - start2.Y)*(end.X - start.X) -
											(end2.X - start2.X)*(end.Y - start.Y));

			const f32 numeratorA = (f32)((end2.X - start2.X)*(start.Y - start2.Y) -
											(end2.Y - start2.Y)*(start.X -start2.X));

			const f32 numeratorB = (f32)((end.X - start.X)*(start.Y - start2.Y) -
											(end.Y - start.Y)*(start.X -start2.X)); 

			if(QcMath::equals(commonDenominator, 0.f))
			{ 
				// The lines are either coincident or parallel
				// if both numerators are 0, the lines are coincident
				if(QcMath::equals(numeratorA, 0.f) && QcMath::equals(numeratorB, 0.f))
				{
					// Try and find a common endpoint
					if(start2 == start || end2 == start)
						out = start;
					else if(end2 == end || start2 == end)
						out = end;
					// now check if the two segments are disjunct
					else if (start2.X>start.X && end2.X>start.X && start2.X>end.X && end2.X>end.X)
						return false;
					else if (start2.Y>start.Y && end2.Y>start.Y && start2.Y>end.Y && end2.Y>end.Y)
						return false;
					else if (start2.X<start.X && end2.X<start.X && start2.X<end.X && end2.X<end.X)
						return false;
					else if (start2.Y<start.Y && end2.Y<start.Y && start2.Y<end.Y && end2.Y<end.Y)
						return false;
					// else the lines are overlapping to some extent
					else
					{
						// find the points which are not contributing to the
						// common part
						QcVector2d<T> maxp;
						QcVector2d<T> minp;
						if ((start.X>start2.X && start.X>end2.X && start.X>end.X) || (start.Y>start2.Y && start.Y>end2.Y && start.Y>end.Y))
							maxp=start;
						else if ((end.X>start2.X && end.X>end2.X && end.X>start.X) || (end.Y>start2.Y && end.Y>end2.Y && end.Y>start.Y))
							maxp=end;
						else if ((start2.X>start.X && start2.X>end2.X && start2.X>end.X) || (start2.Y>start.Y && start2.Y>end2.Y && start2.Y>end.Y))
							maxp=start2;
						else
							maxp=end2;
						if (maxp != start && ((start.X<start2.X && start.X<end2.X && start.X<end.X) || (start.Y<start2.Y && start.Y<end2.Y && start.Y<end.Y)))
							minp=start;
						else if (maxp != end && ((end.X<start2.X && end.X<end2.X && end.X<start.X) || (end.Y<start2.Y && end.Y<end2.Y && end.Y<start.Y)))
							minp=end;
						else if (maxp != start2 && ((start2.X<start.X && start2.X<end2.X && start2.X<end.X) || (start2.Y<start.Y && start2.Y<end2.Y && start2.Y<end.Y)))
							minp=start2;
						else
							minp=end2;

						// one line is contained in the other. Pick the center
						// of the remaining points, which overlap for sure
						out = QcVector2d<T>();
						if (start != maxp && start != minp)
							out += start;
						if (end != maxp && end != minp)
							out += end;
						if (start2 != maxp && start2 != minp)
							out += start2;
						if (end2 != maxp && end2 != minp)
							out += end2;
						out.X = (T)(out.X*0.5f);
						out.Y = (T)(out.Y*0.5f);
					}

					return true; // coincident
				}

				return false; // parallel
			}

			// Get the point of intersection on this line, checking that
			// it is within the line segment.
			const f32 uA = numeratorA / commonDenominator;
			if(uA < 0.f || uA > 1.f)
				return false; // Outside the line segment

			const f32 uB = numeratorB / commonDenominator;
			if(uB < 0.f || uB > 1.f)
				return false; // Outside the line segment

			// Calculate the intersection point.
			out.X = (T)(start.X + uA * (end.X - start.X));
			out.Y = (T)(start.Y + uA * (end.Y - start.Y));
			return true; 
		}


		bool intersectWith(const QcLine2d<T>& line) const
		{
			return intersectWith(line.start, line.end);
		}
		bool intersectWith(const QcVector2d<T>& start2, const QcVector2d<T>& end2) const
		{
			// Uses the method given at:
			// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/ 
			const f32 commonDenominator = (f32)((end2.Y - start2.Y)*(end.X - start.X) -
				(end2.X - start2.X)*(end.Y - start.Y));

			const f32 numeratorA = (f32)((end2.X - start2.X)*(start.Y - start2.Y) -
				(end2.Y - start2.Y)*(start.X -start2.X));

			const f32 numeratorB = (f32)((end.X - start.X)*(start.Y - start2.Y) -
				(end.Y - start.Y)*(start.X -start2.X)); 

			if(QcMath::equals(commonDenominator, 0.f))
			{ 
				// The lines are either coincident or parallel
				// if both numerators are 0, the lines are coincident
				if(QcMath::equals(numeratorA, 0.f) && QcMath::equals(numeratorB, 0.f))
				{
					if (start2.X>start.X && end2.X>start.X && start2.X>end.X && end2.X>end.X)
						return false;
					else if (start2.Y>start.Y && end2.Y>start.Y && start2.Y>end.Y && end2.Y>end.Y)
						return false;
					else if (start2.X<start.X && end2.X<start.X && start2.X<end.X && end2.X<end.X)
						return false;
					else if (start2.Y<start.Y && end2.Y<start.Y && start2.Y<end.Y && end2.Y<end.Y)
						return false;
					return true; // coincident
				}
				return false; // parallel
			}

			// Get the point of intersection on this line, checking that
			// it is within the line segment.
			const f32 uA = numeratorA / commonDenominator;
			if(uA < 0.f || uA > 1.f)
				return false; // Outside the line segment

			const f32 uB = numeratorB / commonDenominator;
			if(uB < 0.f || uB > 1.f)
				return false; // Outside the line segment

			return true; 
		}

		//! Get unit vector of the line.
		/** \return Unit vector of this line. */
		QcVector2d<T> getUnitVector() const
		{
			T len = (T)(1.0 / getLength());
			return QcVector2d<T>((end.X - start.X) * len, (end.Y - start.Y) * len);
		}

		//! Get angle between this line and given line.
		/** \param l Other line for test.
		\return Angle in degrees. */
		f64 getAngleWith(const QcLine2d<T>& l) const
		{
			QcVector2d<T> vect = getVector();
			QcVector2d<T> vect2 = l.getVector();
			return vect.getAngleWith(vect2);
		}

		//! Tells us if the given point lies to the left, right, or on the line.
		/** \return 0 if the point is on the line
		<0 if to the left, or >0 if to the right. */
		T getPointOrientation(const QcVector2d<T>& point) const
		{
			return ( (end.X - start.X) * (point.Y - start.Y) -
					(point.X - start.X) * (end.Y - start.Y) );
		}

		//! Check if the given point is a member of the line
		/** \return True if point is between start and end, else false. */
		bool isPointOnLine(const QcVector2d<T>& point) const
		{
			T d = getPointOrientation(point);
			return (d == 0 && point.isBetweenPoints(start, end));
		}

		//! Check if the given point is between start and end of the line.
		/** Assumes that the point is already somewhere on the line. */
		bool isPointBetweenStartAndEnd(const QcVector2d<T>& point) const
		{
			return point.isBetweenPoints(start, end);
		}

		//! Get the closest point on this line to a point
		QcVector2d<T> getClosestPoint(const QcVector2d<T>& point) const
		{
			QcVector2d<T> c = point - start;
			QcVector2d<T> v = end - start;
			T d = (T)v.getLength();
			v /= d;
			T t = v.dotProduct(c);

			if (t < (T)0.0) return start;
			if (t > d) return end;

			v *= t;
			return start + v;
		}

		//! Start point of the line.
		QcVector2d<T> start;
		//! End point of the line.
		QcVector2d<T> end;
};

//! Typedef for an f32 line.
typedef QcLine2d<f32> QcLine2df;
//! Typedef for an integer line.
typedef QcLine2d<s32> QcLine2di;

