#include "pch.hpp"
#include "QcResPathDesc.hpp"
#include "Common\QcEncryptFileBuf.hpp"
#include "QcPathMgr.hpp"
#include "Animator\QcPathKeyPointHandler.hpp"

ImplRuntimeClass2(QcResPathDesc, 500);

QcSharedPointerList<QcResPathDesc> QcResPathDesc::m_arDynamicCreateList;

bool QcResPathDesc::MakeScreenIntersection(const QcPoint& startPoint, const QcVector& vec, QcPoint& IntersectionPt)
{
	const QcRect& bound = QmGlobalData.gScreenRect;
	if (bound.isPointInside(startPoint))
	{
		QcPoint farPoint(startPoint + vec * 10000.f);
		if (bound.IntersectWith(QcLine(startPoint, farPoint), &IntersectionPt))
		{
			return true;
		}
	}
	return false;
}

QcResPathDesc* QcResPathDesc::MakeLinePath(const QcPoint& startPoint, const QcPoint& endPoint)
{
	QcResPathDesc* pPath = new QcResPathDesc();
	pPath->m_iDynCreate = true;
	pPath->m_iPathType = 0;
	pPath->m_vBasePoint = startPoint;
	pPath->m_arSeqList.push_back(QsPathNodeInfo(startPoint, endPoint));
	pPath->m_fTotalLen = pPath->m_arSeqList.back().m_fLen;
	m_arDynamicCreateList.Insert(pPath);
	QmAssert(m_arDynamicCreateList.size() < 100);
	return pPath;
}

QcResPathDesc* QcResPathDesc::MakeRegularPath(f32 fObjWidth)
{
	QcPoint pt;
	f32 fInnerWidth = QmGlobalData.gScreenRect.getWidth() * 0.25f;
	f32 fInnerHeight = QmGlobalData.gScreenRect.getHeight() * 0.25f;
	pt.X = Randomf(-fInnerWidth * 0.5f, fInnerWidth * 0.5f);
	pt.Y = Randomf(-fInnerHeight * 0.5f, fInnerHeight * 0.5f);
	return MakeRegularPath(pt, Randomf(0, TWO_PI), fObjWidth);
}

QcResPathDesc* QcResPathDesc::MakeRegularPath(const QcPoint& pt, f32 fRoration, f32 fObjWidth)
{
	const QcRect& bound = QmGlobalData.gScreenRect;

	QcVector vec(1.0f, 0.f);
	vec.rotateByRad(fRoration);

	QcLine line(pt - vec * 100000.f, pt + vec * 100000.f);
	QcPoint point[2];
	QmAssert2(bound.IntersectWith(line, &point[0], &point[1]) == 2);
	return MakeLinePath(point[0] - fObjWidth * vec * 0.5f , point[1] + fObjWidth * vec * 0.5f);
}

void QcResPathDesc::MakeRandomSplinePoint(QcPointList& pointList, f32 fObjWidth)
{
	fObjWidth = QcMath::max_(fObjWidth, 150.f);
	QcRect boundRect = QmGlobalData.gScreenRect;
	boundRect.Expand(QcVector(fObjWidth * 0.5f, fObjWidth * 0.5f));

	QcPoint vCurrentPos;
	f32 fCurrentRotation = 0.f;
	QcVector currentVec(1.0f, 0.f);
	int iCorner = RandomInt(0, 4);
	RandomStartPos(vCurrentPos, fCurrentRotation, iCorner);
	fCurrentRotation = iCorner * HALF_PI + Randomf(HALF_PI * 0.4f, HALF_PI * 0.6f);
	currentVec.rotateByRad(fCurrentRotation);
	pointList.push_back(vCurrentPos - (currentVec * fObjWidth + 10.f) );
	pointList.push_back(vCurrentPos);

	fObjWidth = QcMath::min_(fObjWidth, 350.f);
	vCurrentPos += currentVec * fObjWidth;
	f32 fRotation = 0.f;
	for(;;)
	{
		pointList.push_back(vCurrentPos);
		if (fObjWidth * pointList.size() < boundRect.getWidth() * 3.f)
		{
			fRotation = RandomInt(10, 45) * (RandomInt(0,2) == 1 ? DEGTORAD : -DEGTORAD);
			currentVec.rotateByRad(fRotation);
		}
		vCurrentPos += currentVec * fObjWidth;
		if (!boundRect.isPointInside(vCurrentPos))
		{
			pointList.push_back(vCurrentPos);
			break;
		}
	}
}

void QcResPathDesc::BuildSplinePointList(const QcPointList& pointList, QcPointList& destPointList, f32 fDistance)
{
	QcPoint point;
	int nSize = pointList.size();
	for (int i=0; i<nSize - 1; ++i)
	{
		for (f32 u=0.f; u<1.f; u += 0.05f)
		{
			const QcPoint& p0 = pointList[clamp(i-1, nSize)];
			const QcPoint& p1 = pointList[clamp(i+0, nSize)];
			const QcPoint& p2 = pointList[clamp(i+1, nSize)];
			const QcPoint& p3 = pointList[clamp(i+2, nSize)];

			const f32 h1 = 2.0f * u * u * u - 3.0f * u * u + 1.0f;
			const f32 h2 = -2.0f * u * u * u + 3.0f * u * u;
			const f32 h3 = u * u * u - 2.0f * u * u + u;
			const f32 h4 = u * u * u - u * u;

			//tangents
			const f32 Tightness = 0.5f;
			const QcPoint t1 = (p2 - p0) * Tightness;
			const QcPoint t2 = (p3 - p1) * Tightness;

			point = p1 * h1 + p2 * h2 + t1 * h3 + t2 * h4;
			if (destPointList.size() == 0 || destPointList.back().getDistanceFromSQ(point) > fDistance * fDistance)
			{
				destPointList.push_back(point);
			}
		}	
	}
}

QcResPathDesc* QcResPathDesc::MakeSplinePath(const QcPointList& pointList, f32 fDistance)
{
	QcPointList destPointList;
	BuildSplinePointList(pointList, destPointList, fDistance);
	QcResPathDesc* pPath = new QcResPathDesc();
	pPath->m_arRelatedPath.push_back(pPath);
	pPath->m_iDynCreate = true;
	pPath->LoadPath(&destPointList[0], destPointList.size());
	m_arDynamicCreateList.push_back(QcResPathDescPtr(pPath));
	return pPath;
}

QcResPathDesc* QcResPathDesc::MakeSplinePath(f32 fObjWidth)
{
	QcPointList pointList;
	MakeRandomSplinePoint(pointList, fObjWidth);
	return MakeSplinePath(pointList, 1.5f);
}

void QcResPathDesc::RandomStartPos(QcPoint& pos, f32& fRotation, int iCorner)
{
	if (iCorner < 0 || iCorner >= 4)
		iCorner = RandomInt(0,4);
	f32 iLen = (f32)RandomInt(0, s32( (QmGlobalData.gScreenRect.getWidth() + QmGlobalData.gScreenRect.getHeight()) * 0.5f));

	fRotation = iCorner * HALF_PI + Randomf(0, HALF_PI);
	switch(iCorner)
	{
	case 0:
		{
			if (iLen <= QmGlobalData.gScreenRect.top())
			{
				pos.X = QmGlobalData.gScreenRect.left();
				pos.Y = -iLen;
			}
			else
			{
				pos.X = QmGlobalData.gScreenRect.left() + iLen - QmGlobalData.gScreenRect.top();
				pos.Y = QmGlobalData.gScreenRect.bottom();
			}
			break;
		}
	case 1:
		{
			if (iLen <= QmGlobalData.gScreenRect.right())
			{
				pos.X = iLen;
				pos.Y = QmGlobalData.gScreenRect.bottom();
			}
			else
			{
				pos.X = QmGlobalData.gScreenRect.right();
				pos.Y = QmGlobalData.gScreenRect.bottom() + iLen - QmGlobalData.gScreenRect.right();
			}
			break;
		}
	case 2:
		{
			if (iLen <= QmGlobalData.gScreenRect.top())
			{
				pos.X = QmGlobalData.gScreenRect.right();
				pos.Y = iLen;
			}
			else
			{
				pos.X = QmGlobalData.gScreenRect.right() - (iLen - QmGlobalData.gScreenRect.top());
				pos.Y = QmGlobalData.gScreenRect.top();
			}
			break;
		}
	case 3:
		{
			if (iLen <= QmGlobalData.gScreenRect.right())
			{
				pos.X = -iLen;
				pos.Y = QmGlobalData.gScreenRect.top();
			}
			else
			{
				pos.X = QmGlobalData.gScreenRect.left();
				pos.Y = QmGlobalData.gScreenRect.top() - (iLen - QmGlobalData.gScreenRect.right());
			}
			break;
		}
	}
}

QcResPathDesc* QcResPathDesc::MakeGravityPath(QsPathKeyPointList& keyPointList, f32 fObjWidth, f32 fSpeedUp, f32 fTimeUp, f32 fSpeedDown, f32 fTimeDown)
{
	QsPathKeyPoint keyPoint(0.f);
	f32 fCurrentLen = keyPoint.m_fLen;

	fObjWidth += 10.f;

	QcRect boundRect = QmGlobalData.gScreenRect;
	boundRect.Expand(QcVector(fObjWidth * 0.5f, fObjWidth * 0.5f));

	QcPoint vCurrentPos;
	QcPoint vNextPos;
	f32 fCurrentRotation;
	QcVector currentVec(1.0f, 0.f);
	int iCorner = RandomInt(0,4);
	RandomStartPos(vCurrentPos, fCurrentRotation, iCorner);
	fCurrentRotation = iCorner * HALF_PI + Randomf(HALF_PI * 0.4f, HALF_PI * 0.6f);
	currentVec.rotateByRad(fCurrentRotation);
	vCurrentPos -= currentVec * fObjWidth;		//反向延长

	QcResPathDesc* pPath = new QcResPathDesc();
	pPath->m_iDynCreate = true;
	pPath->m_iPathType = 0;
	pPath->m_vBasePoint = vCurrentPos;
	m_arDynamicCreateList.Insert(pPath);
	QmAssert(m_arDynamicCreateList.size() < 100);

	//vt + 0.5 * a * t * t
	f32 fSpeedupLen =  fSpeedUp * fTimeUp;
	f32 fSpeedDownLen = fSpeedDown * fTimeDown * 0.2f;
	f32 fAroundLen = fSpeedDown * fTimeDown * 0.8f;
	//f32 fPlayingLen = (fSpeedDown  * 2.f) * fPlayingTime;

	int iCount = 0;
	QcList<QsPathNodeInfo>& pathNodeList = pPath->m_arSeqList;

	vNextPos = vCurrentPos + currentVec * (fSpeedDownLen + fAroundLen);
	pathNodeList.push_back(QsPathNodeInfo(vCurrentPos, vNextPos));
	vCurrentPos = vNextPos;
	fCurrentLen += (fSpeedDownLen + fAroundLen);
	for (;;)
	{
		bool bPlaying = false;//fPlayingTime > 0.0f && Randomf() < 0.2f;

		//加速关键点
		keyPoint.m_fLen = fCurrentLen;
		keyPoint.m_fSpeed = fSpeedUp;
		keyPoint.m_fSpeedFactor = 0.f;
		keyPoint.m_fTime = 0.f;
		keyPoint.m_iParamCb = 0;
		keyPointList.push_back(keyPoint);
		//加速段
		vNextPos = vCurrentPos + currentVec * fSpeedupLen;
		pathNodeList.push_back(QsPathNodeInfo(vCurrentPos, vNextPos));
		vCurrentPos = vNextPos;
		fCurrentLen += fSpeedupLen;
		
		//关键点
		keyPoint.m_fLen = fCurrentLen;
		keyPoint.m_fSpeed = fSpeedDown;
		keyPoint.m_fSpeedFactor = 0.f;
		keyPoint.m_fTime = 0.f;
		keyPoint.m_iParamCb = 1;
		keyPointList.push_back(keyPoint);
		//减速段
		vNextPos = vCurrentPos + currentVec * fSpeedDownLen;
		pathNodeList.push_back(QsPathNodeInfo(vCurrentPos, vNextPos));
		vCurrentPos = vNextPos;
		fCurrentLen += fSpeedDownLen;

		if (iCount >= 3 && !boundRect.isPointInside(vCurrentPos))
		{
			pPath->m_fTotalLen = fCurrentLen;
			break;
		}

		//匀速
		keyPoint.m_fLen = fCurrentLen;
		keyPoint.m_fSpeed = fSpeedDown;
		keyPoint.m_fSpeedFactor = 0.f;
		keyPoint.m_fTime = 0.f;
		keyPoint.m_iParamCb = bPlaying ? 1 : 0;
		keyPointList.push_back(keyPoint);

		f32 fTurnRotation = 0.f;
		int iTurnCount = 1;
		f32 fGapRotation = 0.f;
		f32 fCs = 1.f;
		f32 fSn = 0.f;
		if (iCount < 10)
		{
			fTurnRotation = RandomInt(45, 75) * ( RandomInt(0, 2) == 1 ?  DEGTORAD : -DEGTORAD);
			iTurnCount = (int)QcMath::abs_(fTurnRotation/0.05f);
			fGapRotation = fTurnRotation/iTurnCount;

			fCs = cos(fGapRotation);
			fSn = sin(fGapRotation);
		}
		f32 fTurnLen = fAroundLen/iTurnCount;
		for (int i=0; i<iTurnCount; ++i)
		{
			currentVec.rotateBy(fCs, fSn);

			vNextPos = vCurrentPos + currentVec * fTurnLen;
			pathNodeList.push_back(QsPathNodeInfo(vCurrentPos, vNextPos));
			vCurrentPos = vNextPos;
		}
		fCurrentLen += fAroundLen;

		//if (bPlaying)
		//{
		//	vNextPos = vCurrentPos + currentVec * fPlayingLen;
		//	pathNodeList.push_back(QsPathNodeInfo(vCurrentPos, vNextPos));
		//	vCurrentPos = vNextPos;
		//	//加速关键点
		//	keyPoint.m_fLen = fCurrentLen;
		//	keyPoint.m_fSpeed = (fSpeedDown  * 2.f) ;
		//	keyPoint.m_fSpeedFactor = 0.f;
		//	keyPoint.m_fTime = 0.f;
		//	keyPoint.m_iParamCb = 2;
		//	keyPointList.push_back(keyPoint);
		//	fCurrentLen += fPlayingLen;
		//}

		++iCount;
	}
	return pPath;
}


QcResPathDesc* QcResPathDesc::Load(const QcXmlNodeIterator& node)
{
	m_sObjType = node.Attribute("name");
	m_sFileName = node.SubValue("fileName");
	if (m_sFileName.size() == 0)
		m_sFileName = m_sObjType + ".path";

	QcStringList pathList;
	QfSplitString(node.Attribute("RelatedPath"), pathList, ';');
	QcStringList::iterator iter(pathList.begin());
	QcStringList::iterator endIter(pathList.end());
	for (; iter != endIter; ++iter)
	{
		QcResPathDescPtr pPathDesc(new QcResPathDesc());
		pPathDesc = pPathDesc->Load(*iter);
		m_arRelatedPath.PushBack(pPathDesc.Get());
	}
	m_arRelatedPath.push_back(this);
	return QmResMgr->GetPathMgr()->LoadPathDesc(this);
}

QcResPathDesc* QcResPathDesc::Load(const QcObjType& objType, const QcString& file)
{
	m_sObjType = objType;
	m_sFileName = file;
	if (m_sFileName.size() == 0)
		m_sFileName = m_sObjType + ".path";

	return QmResMgr->GetPathMgr()->LoadPathDesc(this);
}

void QcResPathDesc::SetUsable(bool bUsable) 
{
	bUsable ? ++m_iUsable : --m_iUsable; 
	if (m_iUsable == 0 && m_iDynCreate)
	{
		QfErase(m_arDynamicCreateList, (const QcResPathDesc*)(this) );
	}
}

bool QcResPathDesc::LoadPath()
{
	QcEncryptFileBuf byteArray;
	if (byteArray.Load(m_sFileName.c_str()))
	{
		m_iDynCreate = false;

		s32 nPoint = byteArray.size()/sizeof(QcPoint);
		return LoadPath((const QcPoint*)byteArray.pointer(), nPoint);
	}
	else
	{
		QmAssert(false);
		QcString errorStr("could not find ");
		errorStr += m_sFileName.c_str();
		QmLogError(errorStr.c_str());
	}
	return false;
}

bool QcResPathDesc::LoadPath(const QcPoint* pointList, int nCount)
{
	if (nCount <= 0)
		return false;

	QcPoint lastPt = pointList[0];
	m_vBasePoint = pointList[0];
	
	++pointList;
	for (s32 i=1; i<nCount; ++i)
	{
		if (QcMath::abs_(pointList->X - lastPt.X) > 1.5 || QcMath::abs_(pointList->Y - lastPt.Y) > 1.5)
		{
			m_arSeqList.push_back(QsPathNodeInfo(lastPt, *pointList));
			m_fTotalLen += m_arSeqList.back().m_fLen;
			lastPt = *pointList;
		}
		++pointList;
	}
	return m_arSeqList.size() >= 1;
}