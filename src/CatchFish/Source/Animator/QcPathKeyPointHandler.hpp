#pragma once

typedef boost::function<void(u32)> QcPathKeyPointCb;

struct QsPathKeyPoint
{
	QsPathKeyPoint(f32 fLen, f32 fSpeed = 0.f, f32 fSpeedFactor = 0.f, f32 fTime = 0.f)
		: m_fLen(fLen), m_fSpeed(fSpeed), m_fSpeedFactor(fSpeedFactor), m_fTime(fTime), m_iParamCb(0)
	{}
	QsPathKeyPoint(f32 fLen, const QcPathKeyPointCb& cb, u32 iParam, f32 fSpeed = 0.f, f32 fSpeedFactor = 0.f, f32 fTime = 0.f)
		: m_fLen(fLen), m_fSpeed(fSpeed), m_fSpeedFactor(fSpeedFactor), m_fTime(fTime), m_cb(cb), m_iParamCb(iParam)
	{}

	bool operator<(const QsPathKeyPoint& right) const
	{
		return m_fLen < right.m_fLen;
	}

	f32 m_fLen;
	f32 m_fSpeed;
	f32 m_fSpeedFactor;
	f32 m_fTime;
	QcPathKeyPointCb m_cb;
	u32 m_iParamCb;
};
typedef std::vector<QsPathKeyPoint> QsPathKeyPointList;

#include "Animator\QcAnimatorSpeed.hpp"

class QcPathKeyPointHandler : public QcAnimatorSpeed
{
public:
	QcPathKeyPointHandler(const QsPathKeyPointList& keyPointList, bool bNeedSort = false);
	virtual bool Animate(QcSceneNode* pSceneNode, f32& fSpeed, double dCurrentTime,f32 fElapsedTime);
protected:
	virtual void OnCb(u32 iParamCb) {}
protected:
	f32 m_fCurrentLen;
	int m_index;
	f32 m_fFactor;
	f32 m_fTime;
	QsPathKeyPointList m_arKeyPointList;
};