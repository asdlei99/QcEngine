#pragma once

//#define QmRegionBoxOn

class QcRegion : public QcObject
{
public:
	enum EType
	{
		eNone,
		eCircle,
		eMutilCircle,
		ePolygon,
		eMulti,
	};
	explicit QcRegion() 
		: m_bDirty(true)
	{}
	virtual ~QcRegion()
	{

	}

	static EType Type(const char* psStr)
	{
		EType eRet = eNone;
		if (QfStrNoCaseCmp(psStr, "Circle") == 0)
			eRet = eCircle;
		else if (QfStrNoCaseCmp(psStr, "MutilCircle") == 0)
			eRet = eMutilCircle;
		else if (QfStrNoCaseCmp(psStr, "Polygon") == 0)
			eRet = ePolygon;
		else if (QfStrNoCaseCmp(psStr, "Multi") == 0)
			eRet = eMulti;

		return eRet;
	}
	virtual void Update(const QcPoint& pt, f32 rotate, f32 scaleX, f32 scaleY, QcRegion* region) = 0;

	inline bool IsDirty() const {return m_bDirty;}
	inline void SetDirty(bool bDirty) { m_bDirty = bDirty;}
#ifdef QmRegionBoxOn
	inline const QcRect& GetBox() 
	{ 
		return m_rcBox; 
	}
	virtual bool Intersect(QcRegion* pRegion){ return GetBox().intersect(pRegion->GetBox());}
#else
	virtual bool Intersect(QcRegion* pRegion){ return true;}
#endif
	virtual bool Intersect(const QcPoint& vPos, f32 fRadius) = 0;
	virtual bool Intersect(const QcPolygon& polygon) = 0;
	virtual bool Include(const QcPoint& vPos) = 0;
	virtual QcRegion* Clone() const = 0;
protected:
#ifdef QmRegionBoxOn
	QcRect m_rcBox;
#endif
	bool m_bDirty;
};