#pragma once

//! Specifies a 2 dimensional size.
template <class T>
class QcDimension2d
{
public:
	//! Default constructor for empty dimension
	QcDimension2d() : Width(0), Height(0) {}
	//! Constructor with width and height
	QcDimension2d(const T& width, const T& height)
		: Width(width), Height(height) {}

	//! Use this constructor only where you are sure that the conversion is valid.
	template <class U>
	explicit QcDimension2d(const QcDimension2d<U>& other) :
	Width((T)other.Width), Height((T)other.Height) {}

	template <class U>
	QcDimension2d<T>& operator=(const QcDimension2d<U>& other)
	{ 
		Width = (T) other.Width;
		Height = (T) other.Height;
		return *this;
	}


	//! Equality operator
	bool operator==(const QcDimension2d<T>& other) const
	{
		return QcMath::equals(Width, other.Width) &&
			QcMath::equals(Height, other.Height);
	}

	//! Inequality operator
	bool operator!=(const QcDimension2d<T>& other) const
	{
		return ! (*this == other);
	}

	//! Set to new values
	QcDimension2d<T>& set(const T& width, const T& height)
	{
		Width = width;
		Height = height;
		return *this;
	}

	//! Divide width and height by scalar
	QcDimension2d<T>& operator/=(const T& scale)
	{
		Width /= scale;
		Height /= scale;
		return *this;
	}

	//! Divide width and height by scalar
	QcDimension2d<T> operator/(const T& scale) const
	{
		return QcDimension2d<T>(Width/scale, Height/scale);
	}

	//! Multiply width and height by scalar
	QcDimension2d<T>& operator*=(const T& scale)
	{
		Width *= scale;
		Height *= scale;
		return *this;
	}

	//! Multiply width and height by scalar
	QcDimension2d<T> operator*(const T& scale) const
	{
		return QcDimension2d<T>(Width*scale, Height*scale);
	}

	//! Add another dimension to this one.
	QcDimension2d<T>& operator+=(const QcDimension2d<T>& other)
	{
		Width += other.Width;
		Height += other.Height;
		return *this;
	}

	//! Subtract a dimension from this one
	QcDimension2d<T>& operator-=(const QcDimension2d<T>& other)
	{
		Width -= other.Width;
		Height -= other.Height;
		return *this;
	}


	//! Add two dimensions
	QcDimension2d<T> operator+(const QcDimension2d<T>& other) const
	{
		return QcDimension2d<T>(Width+other.Width, Height+other.Height);
	}

	//! Get area
	T getArea() const
	{
		return Width*Height;
	}

	//! Get the optimal size according to some properties
	/** This is a function often used for texture dimension
	calculations. The function returns the next larger or
	smaller dimension which is a power-of-two dimension
	(2^n,2^m) and/or square (Width=Height).
	\param requirePowerOfTwo Forces the result to use only
	powers of two as values.
	\param requireSquare Makes width==height in the result
	\param larger Choose whether the result is larger or
	smaller than the current dimension. If one dimension
	need not be changed it is kept with any value of larger.
	\param maxValue Maximum texturesize. if value > 0 size is
	clamped to maxValue
	\return The optimal dimension under the given
	constraints. */
	QcDimension2d<T> getOptimalSize(
		bool requirePowerOfTwo=true,
		bool requireSquare=false,
		bool larger=true,
		u32 maxValue = 0) const
	{
		u32 i=1;
		u32 j=1;
		if (requirePowerOfTwo)
		{
			while (i<(u32)Width)
				i<<=1;
			if (!larger && i!=1 && i!=(u32)Width)
				i>>=1;
			while (j<(u32)Height)
				j<<=1;
			if (!larger && j!=1 && j!=(u32)Height)
				j>>=1;
		}
		else
		{
			i=(u32)Width;
			j=(u32)Height;
		}

		if (requireSquare)
		{
			if ((larger && (i>j)) || (!larger && (i<j)))
				j=i;
			else
				i=j;
		}

		if ( maxValue > 0 && i > maxValue)
			i = maxValue;

		if ( maxValue > 0 && j > maxValue)
			j = maxValue;

		return QcDimension2d<T>((T)i,(T)j);
	}

	//! Get the interpolated dimension
	/** \param other Other dimension to interpolate with.
	\param d Value between 0.0f and 1.0f.
	\return Interpolated dimension. */
	QcDimension2d<T> getInterpolated(const QcDimension2d<T>& other, f32 d) const
	{
		f32 inv = (1.0f - d);
		return QcDimension2d<T>( (T)(other.Width*inv + Width*d), (T)(other.Height*inv + Height*d));
	}


	//! Width of the dimension.
	T Width;
	//! Height of the dimension.
	T Height;
};

//! Typedef for an f32 dimension.
typedef QcDimension2d<f32> QcDimension2df;
//! Typedef for an unsigned integer dimension.
typedef QcDimension2d<u32> QcDimension2du;

//! Typedef for an integer dimension.
/** There are few cases where negative dimensions make sense. Please consider using
dimension2du instead. */
typedef QcDimension2d<s32> QcDimension2di;

