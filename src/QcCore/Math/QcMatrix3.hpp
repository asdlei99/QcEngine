#pragma once

#include "Math\QcVector2d.hpp"

template <class T>
class QcMatrix3
{
public:

	//! Constructor Flags
	enum eConstructor
	{
		EM4CONST_NOTHING = 0,
		EM4CONST_COPY,
		EM4CONST_IDENTITY,
		EM4CONST_TRANSPOSED,
	};
	QcMatrix3(eConstructor constructor = EM4CONST_IDENTITY);
	//! Copy constructor
	/** \param other Other matrix to copy from
	\param constructor Choose the initialization style */
	QcMatrix3(const QcMatrix3<T>& other,eConstructor constructor = EM4CONST_COPY);

	inline T& operator()(const s32 row, const s32 col){ return m[row][col];}
	inline const T& operator()(const s32 row, const s32 col) const{ return m[row][col];}
	inline T& operator[](u32 index){ return M[index];}
	inline const T& operator[](u32 index) const { return M[index]; }
	inline const T* pointer() const { return M; }
	inline T* pointer() { return M;}

	inline QcMatrix3<T>& makeIdentity();
	inline QcMatrix3<T> getTransposed() const;
	inline void getTransposed(QcMatrix3<T>& dest) const;

	inline QcMatrix3<T>& Offset(T X, T Y, T Z = T(0)){ _31 += X; _32 += Y; return *this;}
	inline QcMatrix3<T>& setTranslation(T xTran, T yTran);
	inline QcMatrix3<T>& setRotationRadians(T fRotation);
	inline QcMatrix3<T>& setScale(const T& X, const T& Y);
	inline QcMatrix3<T>& MakeMatrix(T xTran, T yTran, T fRotation, T fScaleX = T(1), T fScaleY = T(1));
	inline QcMatrix3<T>& operator=(const QcMatrix3<T> &other);
	inline QcMatrix3<T> operator*(const QcMatrix3<T> &other) const;
	inline QcMatrix3<T>& operator*=(const QcMatrix3<T> &other);
	inline QcVector2d<T>& transformVect(QcVector2d<T>& out, const QcVector2d<T>& in) const;
private:
	union {
		struct {
			T        _11, _12, _13;
			T        _21, _22, _23;
			T        _31, _32, _33;
		};
		T m[3][3];
		T M[9];
	};
};

template <class T>
inline QcMatrix3<T>::QcMatrix3(eConstructor constructor)
{
	switch (constructor)
	{
	case EM4CONST_NOTHING:
	case EM4CONST_COPY:
		break;
	default:
		makeIdentity();
		break;
	}
}

// Copy constructor
template <class T>
inline QcMatrix3<T>::QcMatrix3(const QcMatrix3<T>& other, eConstructor constructor)
{
	switch (constructor)
	{
	case EM4CONST_IDENTITY:
		makeIdentity();
		break;
	case EM4CONST_NOTHING:
		break;
	case EM4CONST_COPY:
		*this = other;
		break;
	case EM4CONST_TRANSPOSED:
		other.getTransposed(*this);
		break;
	}
}

template <class T>
inline QcMatrix3<T>& QcMatrix3<T>::makeIdentity()
{
#if 1
	static const T identity[] = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
	};
	memcpy(M, identity, sizeof(identity));
#else
	memset(M, 0, 9*sizeof(T));
	M[0] = M[4] = M[8] = (T)1;
#endif
	return *this;
}

template <class T>
inline QcMatrix3<T> QcMatrix3<T>::getTransposed() const
{
	QcMatrix3<T> t( EM4CONST_NOTHING );
	getTransposed(t);
	return t;
}

template<class T>
inline void QcMatrix3<T>::getTransposed(QcMatrix3<T>& dest) const
{
	dest._11 = _11;
	dest._12 = _21;
	dest._13 = _31;

	dest._21 = _12;
	dest._22 = _22;
	dest._23 = _32;

	dest._31 = _13;
	dest._32 = _23;
	dest._33 = _33;
}

template<class T>
inline QcMatrix3<T>& QcMatrix3<T>::setTranslation(T xTran, T yTran)
{
	_31 = xTran;
	_32 = yTran;

	return *this;
}

template<class T>
inline QcMatrix3<T>& QcMatrix3<T>::setRotationRadians(T fRotation)
{
	const T fCos = cos(fRotation); 
	const T fSin = sin(fRotation);

	_11 = fCos;
	_12 = fSin;
	_21 = -fSin;
	_22 = fCos;

	return *this;
}

template<class T>
inline QcMatrix3<T>& QcMatrix3<T>::setScale(const T& X, const T& Y)
{
	_11 = X;
	_22 = Y;
	return *this;
}

template <class T>
inline QcVector2d<T>& QcMatrix3<T>::transformVect(QcVector2d<T>& out, const QcVector2d<T>& in) const
{
	out.X = in.X * _11 + in.Y * _21 + _31;
	out.Y = in.X * _12 + in.Y * _22 + _32;

	return out;
}

template<class T>
inline QcMatrix3<T>& QcMatrix3<T>::MakeMatrix(T xTran, T yTran, T fRotation, T fScaleX = T(1), T fScaleY = T(1))
{
	makeIdentity();
	setTranslation(xTran, yTran);
	if (!(QcMath::equals(fRotation, T(0)) ))
		setRotationRadians(fRotation);

	if (!QcMath::equals(fScaleX, T(1)))
	{
		_11 *= fScaleX;
		_12 *= fScaleX;
	}
	if (!QcMath::equals(fScaleY, T(1)))
	{
		_21 *= fScaleY;
		_22 *= fScaleY;
	}
	return *this;
}

template<class T>
inline QcMatrix3<T>& QcMatrix3<T>::operator=(const QcMatrix3<T> &other)
{
	if (this != &other)
		memcpy(M, other.M, sizeof(M));
	return *this;
}

template<class T>
inline QcMatrix3<T> QcMatrix3<T>::operator*(const QcMatrix3<T> &other) const
{
	QcMatrix3<T> ret(*this);
	ret *= other;
	return ret;
}

template<class T>
inline QcMatrix3<T>& QcMatrix3<T>::operator*=(const QcMatrix3<T> &other)
{
	T mat[9];

	const T *m1 = this->M, *m2 = other.M;

	mat[0] = m1[0] * m2[0] + m1[3] * m2[1] + m1[6] * m2[2];
	mat[1] = m1[1] * m2[0] + m1[4] * m2[1] + m1[7] * m2[2];
	mat[2] = m1[2] * m2[0] + m1[5] * m2[1] + m1[8] * m2[2];

	mat[3] = m1[0] * m2[3] + m1[3] * m2[4] + m1[6] * m2[5];
	mat[4] = m1[1] * m2[3] + m1[4] * m2[4] + m1[7] * m2[5];
	mat[5] = m1[2] * m2[3] + m1[5] * m2[4] + m1[8] * m2[5];

	mat[6] = m1[0] * m2[6] + m1[3] * m2[7] + m1[6] * m2[8];
	mat[7] = m1[1] * m2[6] + m1[4] * m2[7] + m1[7] * m2[8];
	mat[8] = m1[2] * m2[6] + m1[5] * m2[7] + m1[8] * m2[8];

	memcpy(M, mat, sizeof(M));

	return *this;
}

typedef QcMatrix3<f32> QcMatrix3f;