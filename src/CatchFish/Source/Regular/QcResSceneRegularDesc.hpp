#pragma once

#include "Res\QcResDesc.hpp"

class QcResPathDesc;
class QcResFishRegularDesc;

class QcResSceneRegularDesc : public QcResDesc
{
	friend class QcRegularMgr;
	DeclareRuntimeClass(QcResSceneRegularDesc);
public:
	struct QsItemInfo
	{
		int m_id;
		QcSharedPtr<QcResFishRegularDesc> m_pRegularDesc;
		QcResPathDescPtr m_pPathDesc;
		u32 m_nCount;
		f32 m_fGapWidth;

		f32 m_fSpeed;
		f32 m_fStartTime;
		f32 m_fLivingTime;
		int m_iStartWaitItem;
		int m_iEndWaitItem;
	};
	QcResSceneRegularDesc();

	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual const QcObjType& GetObjType() const {return m_objType; }

	f32 GetLivingTime() const {return m_fLivingTime;}
	QcRuntimeClass* GetMakerClass() {return m_pRunClass;}
	QcList<QsItemInfo>& GetItemList() {return m_arRegularList; }
	QsItemInfo& GetItem(int item) {return m_arRegularList[item];}
protected:
	int FindItem(int id);
protected:
	f32 m_fLivingTime;
	QcObjType m_objType;
	QcRuntimeClass* m_pRunClass;
	QcList<QsItemInfo> m_arRegularList;
};

typedef QcList<QcResSceneRegularDesc::QsItemInfo> QcResSceneRegularDescList;
typedef QcResSceneRegularDescList::iterator QcResSceneRegularDescListIter;