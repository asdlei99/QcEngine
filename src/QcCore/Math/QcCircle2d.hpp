#pragma once

template <class T>
class QcCircle2d
{
public:
	QcCircle2d() : m_radiu(0), m_vPos(0, 0){}

	QcCircle2d(T x, T y, T radiu) : m_radiu(radiu), m_vPos(x, y) {}

	QcCircle2d(const QcVector2d<T>& vPos, T radiu) : m_radiu(radiu), m_vPos(vPos) {}

	const QcVector2d<T>& GetPos() const 
	{
		return m_vPos;
	}
	void SetPos(const QcVector2d<T>& vPos)
	{
		m_vPos = vPos;
	}
	T GetRadiu() const
	{
		return m_radiu;
	}
	void SetRadiu(T radiu)
	{
		m_radiu = radiu;
	}
	bool Include(const QcVector2d<T>& vPos) const
	{
		return ((m_vPos.X - vPos.X) * (m_vPos.X - vPos.X) + (m_vPos.Y - vPos.Y) * (m_vPos.Y - vPos.Y)) < m_radiu * m_radiu;
	}
	
	//求线段与圆的交点，返回交点个数
	int GetIntersectPt(const QcLine2d<T>& seq, QcVector2d<T>& firstInsectPt, QcVector2d<T>& sndInsectPt) const
	{
		assert(false);
	}

	//求圆与圆的交点，返回交点个数
	int GetIntersectPt(const QcCircle2d& circle, QcVector2d<T>& firstInsectPt, QcVector2d<T>& sndInsectPt) const
	{
		assert(false);
	}
	
	/*求圆外点与圆的切点*/
	int GetTangentPt(const QcVector2d<T>& outPt, QcVector2d<T>& firstTangentPt, QcVector2d<T>& sndTangentPt) const
	{
		assert(false);
	}
public:
	QcVector2d<T> m_vPos;
	T m_radiu;
};

//! Typedef for a f32 3d plane.
typedef QcCircle2d<f32> QcCircle2df;

//! Typedef for an integer 3d plane.
typedef QcCircle2d<s32> QcCircle2di;