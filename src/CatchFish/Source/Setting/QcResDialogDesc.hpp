#pragma once

#include "Res\QcResDesc.hpp"

class QcResDialogDesc : public QcResDesc
{
	DeclareRuntimeClass(QcResDialogDesc);
public:
	QcResDialogDesc();
	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual const QcObjType& GetObjType() const {return m_objType; };

	void LoadTexture(QcResManager* pResMgr);

	const QcString& GetAction() const {return m_sAction;}
	const QcPoint& GetSize() const {return m_fSize;}
	const QcResImageDesc* GetBg() {return m_oBgImage;}
	const QcResImageDesc* GetSelectFlag() {return m_oSelectFlagImage;}
	const QcResImageDesc* GetSelectBg() {return m_oSelectBgImage;}
	QcResDialogItemDescList& GetItemList() {return m_arItemList;}
	QcResDialogItemDescList& GetTipList() {return m_arTipList;}
	QcResDialogItemDescList& GetStaticList() {return m_arStaticList;}
	template<class T>
	T* MakeInstance(void* /*pUserContext*/ = NULL)
	{
		return MakeInstance()->As<T*>();
	}
	QcDialog* MakeInstance();
protected:
	QcResDialogItemDesc* LoadItemDesc(const QcXmlNodeIterator& node);
	void LoadStaticList(const QcXmlNodeIterator& node);
	void LoadTipAreaList(const QcXmlNodeIterator& node);
	void LoadItemList(const QcXmlNodeIterator& node);
	void LoadType(const QcXmlNodeIterator& node, const char* psType, QcResDialogItemDescList& itemList);
protected:
	QcString m_sAction;
	QcObjType m_objType;
	QcPoint m_fSize;
	QcRuntimeClass* m_pDialogClass;
	QcResImageDescPtr m_oBgImage;
	QcResImageDescPtr m_oSelectFlagImage;
	QcResImageDescPtr m_oSelectBgImage;
	QcResDialogItemDescList m_arStaticList;
	QcResDialogItemDescList m_arTipList;
	QcResDialogItemDescList m_arItemList;
};