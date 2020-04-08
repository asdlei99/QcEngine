#pragma once

class QcMathNodeInfo
{
public:
	QcMathNodeInfo()
		: m_fRotate(0.f),m_fScaleX(1.f), m_fScaleY(1.f)
	{

	}
	QcMathNodeInfo(const QcVector2d<f32>& point, f32 fRotate = 0.f, f32 fScaleX = 1.0f, f32 fScaleY = 1.0f)
		: m_vPos(point), m_fRotate(fRotate), m_fScaleX(fScaleX), m_fScaleY(fScaleY)
	{}

	const QcVector2d<f32>& GetPos() const{ return m_vPos;}
	void SetPos(const QcVector2d<f32>& pos){ m_vPos = pos;}

	f32 GetRotate() const {return m_fRotate;}
	void SetRotate(f32 fRotate) { m_fRotate = fRotate; }

	f32 GetScaleX() const { return m_fScaleX;}
	f32 GetScaleY() const { return m_fScaleY;}
	void SetScaleX(f32 val) { m_fScaleX = val;}
	void SetScaleY(f32 val) { m_fScaleY = val;}
	void SetScale(f32 val) { m_fScaleY = m_fScaleX = val;}
private:
	QcVector2d<f32> m_vPos;
	f32 m_fRotate;
	f32 m_fScaleX;
	f32 m_fScaleY;
};