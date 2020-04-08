#include "pch.hpp"
#include "QcAnimatorMoveBoundBox.hpp"
#include "Scene\QcSceneNode.hpp"

ImplRuntimeClass2(QcAnimatorMoveBoundBox, 200);

enum
{
	eBoundCenter = 0,
	eBoundLeft = 1,
	eBoundBottom = 2,
	eBoundRight= 4,
	eBoundTop= 8,
};

void QcAnimatorMoveBoundBox::AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
    f32 fOldSpd = m_fSpeed;
	if (HandleSpeed(pSceneNode, dCurrentTime, fElapsedTime))
	{
		QcPoint vPos = pSceneNode->GetPosition();
		Walk(vPos, (m_fSpeed+fOldSpd)*0.5f * fElapsedTime);
		pSceneNode->SetPosition(vPos);
		pSceneNode->SetRotation(surroundRad(m_fRoration));
	}
}


void QcAnimatorMoveBoundBox::AddToSceneNode(QcSceneNode* pSceneNode)
{
	m_fRoration = pSceneNode->GetRotation();
	m_timeVector.rotateByRad(QcMath::surroundRad(m_fRoration));
}

void QcAnimatorMoveBoundBox::Walk(QcPoint& nextPt, f32 fDist)
{
	nextPt += m_timeVector * fDist;
	int iPos = m_rcBox.Position(nextPt);
	while (iPos > 0)
	{
		int iSide = 0;
		QcPoint symmetryPoint;	
		switch(iPos)
		{
		case 1:
			{
				symmetryPoint = m_rcBox.LeftTop();
				QcLine line(nextPt , nextPt + m_timeVector);
				f32 fValue = line.getPointOrientation(symmetryPoint);
				if (!QcMath::equals(fValue, 0.f))
				{
					iSide = fValue < 0.f ? eBoundTop : eBoundLeft;
				}
				break;
			}
		case 2:
			{
				iSide = eBoundLeft;
				break;
			}
		case 3:
			{
				symmetryPoint = m_rcBox.LeftBottom();
				QcLine line(nextPt , nextPt + m_timeVector);
				f32 fValue = line.getPointOrientation(symmetryPoint);
				if (!QcMath::equals(fValue, 0.f))
				{
					iSide = fValue < 0.f ? eBoundBottom : eBoundLeft;
				}
				break;
			}
		case 4:
			{
				iSide = eBoundTop;
				break;
			}
		case 5:
			{
				iSide = eBoundBottom;
				break;
			}
		case 6:
			{
				symmetryPoint = m_rcBox.RightTop();
				QcLine line(nextPt , nextPt + m_timeVector);
				f32 fValue = line.getPointOrientation(symmetryPoint);
				if (!QcMath::equals(fValue, 0.f))
				{
					iSide = fValue < 0.f ? eBoundRight : eBoundTop;
				}
				break;
			}
		case 7:
			{
				iSide = eBoundRight;
				break;
			}
		case 8:
			{
				symmetryPoint = m_rcBox.RightBottom();
				QcLine line(nextPt , nextPt + m_timeVector);
				f32 fValue = line.getPointOrientation(symmetryPoint);
				if (!QcMath::equals(fValue, 0.f))
				{
					iSide = fValue < 0.f ? eBoundRight : eBoundBottom;
				}
				break;
			}
		default:
			QmAssert(false);
			break;
		}
		switch(iSide)
		{
		//½Çµã·´µ¯
		case 0:
			{
				m_timeVector.X = -m_timeVector.X;
				m_timeVector.Y = -m_timeVector.Y;
				m_fRoration = QcMath::surroundRad2(m_fRoration + PI);
				nextPt = symmetryPoint + symmetryPoint - nextPt;
				break;
			}
		case eBoundLeft:
			{
				m_timeVector.X = 0 - m_timeVector.X;
				m_fRoration = (TWO_PI + PI) - m_fRoration;
				nextPt.X = m_rcBox.left() + m_rcBox.left() - nextPt.X;
				break;
			}
		case eBoundRight:
			{
				m_timeVector.X = 0 - m_timeVector.X;
				m_fRoration = (TWO_PI + PI) - m_fRoration;
				nextPt.X = m_rcBox.right() + m_rcBox.right() - nextPt.X;
				break;
			}
		case eBoundBottom:
			{
				m_timeVector.Y = 0 - m_timeVector.Y;
				m_fRoration = TWO_PI - m_fRoration;
				nextPt.Y = m_rcBox.bottom() + m_rcBox.bottom() - nextPt.Y;
				break;
			}
		case eBoundTop:
			{
				m_timeVector.Y = 0 - m_timeVector.Y;
				m_fRoration = TWO_PI - m_fRoration;
				nextPt.Y = m_rcBox.top() + m_rcBox.top() - nextPt.Y;
				break;
			}
		}
		iPos = m_rcBox.Position(nextPt);
	}
}