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
	//游动范围
	QcRect m_rcBoundBox;
	f32 m_fStayTime;
	bool m_bFleeOn;
	//每秒获得的能量
	f32 m_fEnergyOneSecondGet;
	//阻力系数
	f32 m_fResistance; 

	//当前能量
	f32 m_fCurrentEnergy;
	//走完后转弯
	f32 m_fRorationGap;
	f32 m_fLeaveLen;
	f32 m_fRoration;
	QcVector m_timeVector;
};