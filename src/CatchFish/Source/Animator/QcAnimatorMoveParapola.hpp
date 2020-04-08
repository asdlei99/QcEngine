#pragma once

#include "Animator/QcAnimatorMove.hpp"

class QcAnimatorMoveParapola : public QcAnimatorMove
{
	DeclareRuntimeClass(QcAnimatorMoveParapola);
public:
	explicit QcAnimatorMoveParapola();
	//传递参数时rotation会做自动向玩家方向的偏转，相应设置速度和加速度的方向按照绝对坐标去设置;
	void Reset(const QcPoint& start, f32 speedx, f32 speedy, f32 aspeedx, f32 aspeedy, f32 timeForWay, f32 rotation)
	{
		QcAnimatorMove::Reset();
		m_speedx = speedx;
		m_speedy = speedy;
		m_aspeedx = aspeedx;
		m_aspeedy = aspeedy;
		m_fparapolaTime = timeForWay;
		m_postemp = start;
		m_StartPos = start;
		m_rotation = rotation;
	}
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
	virtual void AddToSceneNode(QcSceneNode* pSceneNode);
private:
	f32 m_speedx;				//x方向的速度
	f32 m_speedy;				//y方向的速度
	f32 m_aspeedx;				//x方向的加速度
	f32 m_aspeedy;				//y方向的加速度
	QcPoint m_postemp;			//保存初始位置
	QcPoint m_StartPos;
	f32 m_fparapolaTime;		//进行抛物线运动的时间
	f32	m_rotation;				//角度 进行对玩家方向的旋转
};