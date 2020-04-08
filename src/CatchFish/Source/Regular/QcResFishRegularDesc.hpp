#pragma once

#include "Res\QcResDesc.hpp"
class QcResPathDesc;
class QcResFishRegularDesc : public QcResDesc
{
	friend class QcRegularMgr;
	DeclareRuntimeClass(QcResFishRegularDesc);
public:
	struct QsRegularItem
	{
		operator const QcObjType&() const
		{
			return m_sFishName;
		}
		f32 m_fScale;
		QcObjType m_sFishName;
		QcPointList m_pointList;
	};
	QcResFishRegularDesc();
	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual const QcObjType& GetObjType() const {return m_objType; }
	static QcResFishRegularDesc* MakeSingleFishRegular(const QcObjType& objType);

	u32 GetLayer() const {return m_iLayer;};
	f32 GetWidth() const {return m_fWidth;}
	const QcList<QsRegularItem>& GetItemList() const { return m_arRegularItem;}
	QcList<QcResPathDesc*>& GetNormalWalkablePath() {return m_arWalkablePathList[0];}
	QcList<QcResPathDesc*>& GetSceneWalkablePath(u32 nSceneId) {return m_arWalkablePathList[nSceneId + 1];}
protected:
	void LoadRegularItem(const QcXmlNodeIterator& node);
protected:
	QcObjType m_objType;
	u32 m_iLayer;
	f32 m_fWidth;
	f32 m_fRandom;
	bool m_bOnlyInSceneRegular;
	QcRuntimeClass* m_pAnimator;			
	QcList<QsRegularItem> m_arRegularItem;
	QcList<QcResPathDesc*> m_arWalkablePathList[QmCountOf(QaSceneList) + 1];
};

typedef QcList<QcResFishRegularDesc::QsRegularItem> QsRegularItemList;
typedef QsRegularItemList::iterator QsRegularItemListIter;
typedef QsRegularItemList::const_iterator QsRegularItemListConstIter;