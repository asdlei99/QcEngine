#pragma once

#include "Animator\QcAnimatorMove.hpp"

class QcAnimatorMoveCircle : public QcAnimatorMove
{
public:
	explicit QcAnimatorMoveCircle();

	void Reset(const QcPoint& center, f32 radSpeed, bool bAnticlockwise  = true, bool bSelfRotate = true, f32 fRad = 0.f)
	{
		QcAnimatorMove::Reset();
		m_ptCenter = center;
		m_fSpeed = abs(radSpeed);
		m_fRad = fRad;
		m_bSelfRotate = bSelfRotate;
		m_bAnticlockwise = bAnticlockwise;
		m_fTurnRad = 0.f;
	}
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
private:
	QcPoint m_ptCenter;		
	f32 m_fRad;				//��ת�ķ���, С�ڵ���0��ʾһֱת
	f32 m_fTurnRad;
	bool m_bSelfRotate;		//�Ƿ���ת
	bool m_bAnticlockwise;	//��ת�ķ���
};