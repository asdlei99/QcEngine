#pragma once

#include "Res\QcResDesc.hpp"
class QcResDialogItemDesc : public QcResDesc
{
	DeclareRuntimeClass(QcResDialogItemDesc);
public:
	QcResDialogItemDesc();

	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	void LoadTexture(QcResManager* pResMgr);

	void SetAlign(QeAlign eAlign) {m_eAlign = eAlign;}
	void SetAlignPos(const QcPoint& vPos) {m_vPos = vPos;}
	void SetSize(f32 fWidth, f32 fHeigh) {m_size.X = fWidth; m_size.Y = fHeigh;}
	u32 GetID() const {return m_nID;}
	QeAlign GetAlign() const {return m_eAlign;}
	QcPoint GetAlignPos() const {return m_vPos;}
	f32 GetWidth() const {return m_size.X;}
	f32 GetHeight() const {return m_size.Y;}
	bool IsAcceptInput() {return m_bAcceptInput;}
	QcRuntimeClass* GetWidgetClass() const {return m_pWidgetClass;}
	QcResImageDesc* GetTipDesc() const {return m_pTip.Get();}
	QcResImageDesc* GetWidgetImage() const {return m_pImage.Get();}
	QcResDialogItemDescList& GetSubWidetDescList() {return m_arWidgetItemDesc;}
	const QcString& GetAction() const {return m_sAction;}
	f32 GetUpdateTime() const {return m_fUpdateTime;}
protected:
	virtual bool LoadParam(const QcXmlNodeIterator& node, QcRuntimeClass* pRunClass);
	virtual bool LoadSubItem(const QcXmlNodeIterator& node);
	QcRuntimeClass* TypeToRuntimeClass(const char* psType);
protected:
	QcString m_sAction;
	QeAlign m_eAlign;
	QcPoint m_vPos;
	QcPoint m_size;
	u32 m_nID;
	f32 m_fUpdateTime;
	bool m_bAcceptInput;
	QcRuntimeClass* m_pWidgetClass;
	QcResImageDescPtr m_pTip;
	QcResImageDescPtr m_pImage;
	QcResDialogItemDescList m_arWidgetItemDesc;
};