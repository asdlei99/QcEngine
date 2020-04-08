#pragma once

#include "Animator\QcAnimatorSpeed.hpp"
class QcShelterHandle : public QcAnimatorSpeed
{
	QmDisableCopyAssignment(QcShelterHandle);
public:
	QcShelterHandle(const std::vector<f32>& boundInfo);
protected:
	virtual bool Animate(QcSceneNode* pSceneNode, f32& fSpeed, double dCurrentTime,f32 fElapsedTime);
protected:
	bool m_bEnterShelter;
	f32 m_fDestScale;
	f32 m_fScaleSpeed;
	f32 m_fDestSpeed;
	f32 m_fGravitySpeed;

	f32 m_fSpeedSave;
	f32 m_fCurrentLen;

	int m_iBoundIdx;
	const std::vector<f32>& m_boundInfo;
};