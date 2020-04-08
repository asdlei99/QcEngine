#pragma once

#include "Animator\QcAnimatorMove.hpp"
#include "Animator\QiAnimator.hpp"

class QcAnimatorCyclone : public QcAnimatorMove
{
public:
	explicit QcAnimatorCyclone();

	void Reset(const QcPoint& center, f32 radSpeed, f32 fRadiuSpeed, f32 fRad = 0.f, f32 fTime = 0.f, f32 fMinRadiu = 10.f, f32 fMaxRadiu = 0.f, bool bAnticlockwise  = true, bool bSelfRotate = true, bool bOutScreenDied = false)
	{
		QcAnimatorMove::Reset();
		m_ptCenter = center;
		m_fSpeed = abs(radSpeed);
		m_fRadiuSpeed = fRadiuSpeed;
		m_fTime = abs(fTime);
		m_fRad = fRad;

		m_fMinRadiu = abs(fMinRadiu);
		if (fMaxRadiu < ROUNDING_ERROR_f32)
		{
			m_fMaxRadiu = QmGlobalData.gScreenRect.top();
		}
		else
		{
			m_fMaxRadiu = abs(fMaxRadiu);
		}

		m_bSelfRotate = bSelfRotate;
		m_bAnticlockwise = bAnticlockwise;
		
		m_fTurnRad = 0.f;
		m_fTurnRadiu = 0.f;
		m_bOutScreenDied = bOutScreenDied;
	}
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
private:
	QcPoint m_ptCenter;		
	f32 m_fRad;				//��ת�ķ���, С�ڵ���0��ʾһֱת
	f32 m_fTurnRad;			//��ǰ��ת����
	f32 m_fTime;		
	f32 m_fTurnRadiu;
	f32 m_fRadiuSpeed;		//�����ٶȣ�С��0���ڣ�����0������
	f32 m_fMinRadiu;
	f32 m_fMaxRadiu;
	bool m_bSelfRotate;		//�Ƿ���ת
	bool m_bAnticlockwise;	//��ת�ķ���
	bool m_bOutScreenDied;
	

};