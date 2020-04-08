#pragma once

#include "Animator/QcAnimatorMove.hpp"

class QcAnimatorMoveParapola : public QcAnimatorMove
{
	DeclareRuntimeClass(QcAnimatorMoveParapola);
public:
	explicit QcAnimatorMoveParapola();
	//���ݲ���ʱrotation�����Զ�����ҷ����ƫת����Ӧ�����ٶȺͼ��ٶȵķ����վ�������ȥ����;
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
	f32 m_speedx;				//x������ٶ�
	f32 m_speedy;				//y������ٶ�
	f32 m_aspeedx;				//x����ļ��ٶ�
	f32 m_aspeedy;				//y����ļ��ٶ�
	QcPoint m_postemp;			//�����ʼλ��
	QcPoint m_StartPos;
	f32 m_fparapolaTime;		//�����������˶���ʱ��
	f32	m_rotation;				//�Ƕ� ���ж���ҷ������ת
};