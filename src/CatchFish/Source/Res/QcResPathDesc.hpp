#pragma once

#include "Res\QcResDesc.hpp"

struct QsPathNodeInfo
{
	QsPathNodeInfo(const QcPoint& startPt, const QcPoint& endPt) : m_vPos(startPt)
	{
		m_vec = endPt - startPt;
		m_fLen = m_vec.getLength();
		m_vec.normalize(m_fLen);
		m_fOriental = (f32)m_vec.getRadAngleTrig();
	}
	QcPoint m_vPos;
	QcVector m_vec;
	f32 m_fOriental;
	f32 m_fLen;
};

class QcResPathDesc : public QcResDesc
{
	friend class QcPathMgr;
	DeclareRuntimeClass(QcResPathDesc);
public:
	QcResPathDesc()
		:m_iUsable(0)
		,m_iPathType(0)
		,m_iDynCreate(true)
		, m_fTotalLen(0.f) {}

	static void RandomStartPos(QcPoint& pos, f32& fRotation, int iCorner = -1);
	static bool MakeScreenIntersection(const QcPoint& startPoint, const QcVector& vec, QcPoint& IntersectionPt);
	static QcResPathDesc* MakeLinePath(const QcPoint& startPoint, const QcPoint& endPoint);
	static QcResPathDesc* MakeRegularPath(const QcPoint& pt, f32 fRoration, f32 fObjWidth);
	static QcResPathDesc* MakeRegularPath(f32 fObjWidth);
	static QcResPathDesc* MakeGravityPath(QsPathKeyPointList& keyPointList, f32 fObjWidth, f32 fSpeedUp = 300.f, f32 fTimeUp = 1.f, f32 fSpeedDown = 100.f, f32 fTimeDown = 1.2f);
	static QcResPathDesc* MakeSplinePath(f32 fObjWidth);
	static QcResPathDesc* MakeSplinePath(const QcPointList& pointList, f32 fDistance);
	static void BuildSplinePointList(const QcPointList& pointList, QcPointList& destPointList, f32 fDistance);
	static void MakeRandomSplinePoint(QcPointList& pointList, f32 fObjWidth);

	virtual QcResPathDesc* Load(const QcXmlNodeIterator& node);
	virtual QcResPathDesc* Load(const QcObjType& objType, const QcString& file = QcString());
	virtual const QcObjType& GetObjType() const {return m_sObjType; };

	s32 GetPathType() const {return m_iPathType;}
	int IsUsable() const {return m_iUsable;}
	void SetUsable(bool bUsable);
	f32 GetTotalLen() const {return m_fTotalLen;}

	const QcList<QsPathNodeInfo>& GetSeqList() const {return m_arSeqList;}
	const QsPathNodeInfo& GetStartSeq() const {return m_arSeqList[0];}
	f32 GetStartRotation() const { return m_arSeqList[0].m_fOriental; };
	const QcPoint& GetBasePoint() const {return m_vBasePoint;}

	const QcList<QcResPathDesc*>& GetRelatedPath() const {return m_arRelatedPath;}
	QcList<QcResPathDesc*>& GetRelatedPath() {return m_arRelatedPath;}
protected:
	bool LoadPath();
	bool LoadPath(const QcPoint* pointList, int nCount);
private:
	bool m_iDynCreate;
	int m_iUsable;
	s32 m_iPathType;
	f32 m_fTotalLen;
	QcObjType m_sObjType;
	QcString m_sFileName;
	QcPoint m_vBasePoint;
	QcList<QsPathNodeInfo> m_arSeqList;
	QcList<QcResPathDesc*> m_arRelatedPath;
	static QcSharedPointerList<QcResPathDesc> m_arDynamicCreateList;
};