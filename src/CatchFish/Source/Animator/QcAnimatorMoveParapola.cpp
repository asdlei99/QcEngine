#include "Pch.hpp"
#include "Animator/QcAnimatorMoveParapola.hpp"
#include "Scene/QcSceneNode.hpp"

ImplRuntimeClass2(QcAnimatorMoveParapola, 10);

QcAnimatorMoveParapola::QcAnimatorMoveParapola()
{

}

void QcAnimatorMoveParapola::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	m_fparapolaTime -= fElapsedTime;
	if (m_fparapolaTime > 0.f)
	{
		m_speedx += m_aspeedx * fElapsedTime;
		m_speedy += m_aspeedy * fElapsedTime;

		m_postemp.X  += m_speedx * fElapsedTime;
		m_postemp.Y += m_speedy * fElapsedTime;
		QcPoint temp_pos = m_postemp;
		temp_pos.rotateByRad(m_rotation,m_StartPos);
		pSceneNode->SetPosition(temp_pos);
	}
	else
		m_bFinished = true;
}

void QcAnimatorMoveParapola::AddToSceneNode(QcSceneNode* pSceneNode)
{

}