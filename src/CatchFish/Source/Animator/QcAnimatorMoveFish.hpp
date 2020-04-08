#pragma once

#include "Animator\QcAnimatorMove.hpp"

class QcAnimatorMoveFish : public QcAnimatorMove
{
	DeclareRuntimeClass(QcAnimatorMoveFish);
public:
	QcAnimatorMoveFish();

protected:
	bool IsCanFlee() {return m_fCurrentEnergy > 10.f; }
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
	virtual void AddToSceneNode(QcSceneNode* pSceneNode);
	void Walk(QcPoint& vPos, f32 fDist);
protected:
	//�ζ���Χ
	QcRect m_rcBoundBox;
	f32 m_fStayTime;
	bool m_bFleeOn;
	//ÿ���õ�����
	f32 m_fEnergyOneSecondGet;
	//����ϵ��
	f32 m_fResistance; 

	//��ǰ����
	f32 m_fCurrentEnergy;
	//�����ת��
	f32 m_fRorationGap;
	f32 m_fLeaveLen;
	f32 m_fRoration;
	QcVector m_timeVector;
};