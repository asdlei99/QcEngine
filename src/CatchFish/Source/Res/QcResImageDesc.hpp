#pragma once

#include "Res\QcResDesc.hpp"

class QcRegion;
struct QsResElementInfo;
class QcResImageDesc : public QcResDesc
{
	DeclareRuntimeClass(QcResImageDesc);
public:
	QcResImageDesc();
	virtual ~QcResImageDesc();
	virtual bool LoadDesc(const char* psObjType, const char* psPrefix = NULL, const char* psFileExt = NULL);
	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual bool LoadAttibute(const QcXmlNodeIterator& node);
	virtual bool LoadSubDesc(const QcXmlNodeIterator& node);
	virtual void LoadStateDesc(const QcXmlNodeIterator& node);
	virtual QcRegionAutoPtr LoadRegion(const QcXmlNodeIterator& node);
	virtual QcRegionAutoPtr LoadRegion2(const QcXmlNodeIterator& node);
	virtual QsFrameRegionList LoadFrameRegionList(const QcXmlNodeIterator& node);
	
	virtual void PostLoad(QcResManager* pResMgr);
	virtual const QcRegion* GetRegion() const {return m_pSourceRegion.Get(); }

	virtual f32 GetDyingTime() const{ return 0.f;}
	virtual f32 GetSpeed() const {return 0.f;}
	virtual const QcObjType& GetObjType() const {return m_objType; }

	void SetPrefix(const char* psPrefix){ m_sPrefix = psPrefix;}
	void SetObjType(const char* psType){ m_objType = psType;}
	void SetExt(const char* psExt) {m_sFileExt = psExt;}
	const QcString& GetPrefix() const { return m_sPrefix; }
	const QcString& GetExt() const { return m_sFileExt; }
	
	const QcPoint& GetPos() const {return m_vPos; }
	f32 GetRotation() const {return m_fRotation; }
	f32 GetWidth() const {return m_fWidth;}
	f32 GetHeight() const {return m_fHeight; }
	f32 GetMaxLen() const {return QcMath::max_(m_fWidth, m_fHeight); }
	s32 IsForceInVideo() const {return m_iForceTexInVideoMem;}
	const QcString& GetEffectRender() const {return m_sEffectRender;}

	const QcList<QsStateParameter>& GetStateParameterList() const { return m_arState; }
	QcList<QsStateParameter>& GetStateParameterList() {return m_arState;}
	const QsStateParameter* GetStateParameter(QeState eState = eLiving) const;
	int GetStateImageList(QeState eState, QcStringList& imageList) const;
	int GetStateImageList(const QsStateParameter& state, QcStringList& imageList) const;

	template<class T>
	T* MakeInstance(bool bLoadSubNode = true) const
	{
		return MakeInstance(bLoadSubNode)->As<T*>();
	}
	QcSceneNode* MakeInstance(bool bLoadSubNode = true) const;
	QcList<QcResImageDescPtr>& GetSubDescList() {return m_arSubList;}
protected:
	bool AddState(QeState eState, f32 fTime, bool bCycle, int iPerFrame = 0);
protected:
	QcRuntimeClass* m_pRuntimeClass;
	QcObjType m_objType;
	QcString m_sPrefix;
	QcString m_sFileExt;
	QcString m_sEffectRender;
	QcPoint	m_vPos;
	f32 m_fWidth;
	f32 m_fHeight;
	f32 m_fRotation;
	s32 m_iForceTexInVideoMem;
	QcList<QsStateParameter> m_arState;
	QcList<QcResImageDescPtr> m_arSubList;
	QcRegionAutoPtr m_pSourceRegion;
};