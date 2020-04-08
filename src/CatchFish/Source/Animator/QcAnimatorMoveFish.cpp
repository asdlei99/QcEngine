#include "pch.hpp"
#include "QcAnimatorMoveFish.hpp"

ImplRuntimeClass2(QcAnimatorMoveFish, 200);

QcAnimatorMoveFish::QcAnimatorMoveFish()
{
	
}

void QcAnimatorMoveFish::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	HandleSpeed(fElapsedTime);

	//能量, 速度计算
	m_fCurrentEnergy += m_fEnergyOneSecondGet * fElapsedTime;
	if (m_bFleeOn)
	{
		m_fSpeed += 200.f * fElapsedTime;
	}
	m_fCurrentEnergy -= m_fSpeed * m_fResistance * fElapsedTime;


	//方向，位置计算
	QcPoint pos = pSceneNode->GetPosition();
	Walk(pos, m_fSpeed * fElapsedTime);
	pSceneNode->SetPosition(pos);
	pSceneNode->SetRotation(QcMath::surroundRad(m_fRoration));
}

void QcAnimatorMoveFish::Walk(QcPoint& vPos, f32 fDist)
{
	vPos += m_timeVector  * fDist;
	m_fLeaveLen -= fDist;

	if (m_fLeaveLen < 0)
	{
		m_fLeaveLen = Randomf(200.f, 500.f);
		m_fRorationGap = 0;
	}
	else
	{
		m_fRoration += m_fRorationGap;
		m_timeVector.X = cos(m_fRoration);
		m_timeVector.Y = sin(m_fRoration);
	}
}

void QcAnimatorMoveFish::AddToSceneNode(QcSceneNode* pSceneNode)
{
	f32 fLen =  QcMath::max_(pSceneNode->GetWidth(), pSceneNode->GetHeight());

	m_rcBoundBox = QmGlobalData.gScreenRect;

	//计算出场位置
	QcRect posRect = m_rcBoundBox;
	posRect.Expand(QcVector(fLen, fLen));
	QcPoint pos;
	UInt16 iBorder = RandomInt(0, 4);
	switch (iBorder)
	{
	case 0:
	case 2:
		{
			pos.X = Randomf(posRect.left(), posRect.right());
			pos.Y = iBorder == 0 ? posRect.bottom() : posRect.top();
			break;
		}
	case 1:
	case 3:
		{
			pos.Y = Randomf(posRect.bottom(), posRect.top());
			pos.X = iBorder == 1 ? posRect.right() : posRect.left();
			break;
		}
	}
	pSceneNode->SetPosition(pos);
	//计算出场方向
	m_timeVector = -pos;
	m_fRoration = (f32)m_timeVector.getRadAngleTrig();
	pSceneNode->SetRotation(m_fRoration);
}
