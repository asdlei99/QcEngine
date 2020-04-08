#pragma once
#include "Setting\QcWidget.hpp"
class QcDialog : public QcSceneNode
{
	DeclareRuntimeClass(QcDialog);
	typedef std::map<u32, QcWidgetPtr> QcWidgetPtrMap;
public:
	QcDialog();
	virtual void InitDialog(QcResDialogDesc* pDialogDesc, QcDialog* pParentDlg = NULL);
	virtual void Update(bool bUpdate);
	virtual bool FindWidgetCallBacker(const QcString& sAction, QcWidgetCallBacker& cb);

	virtual void Update(u32 id, bool bUpdateData);
	bool SetValue(u32 id, BYTE* pValue,u32 nBit)
	{
		QcWidget* pWidget = GetWidget(id);
		if (pWidget)
			return pWidget->Update(pValue, nBit, false);
		return false;
	}
	bool GetValue(u32 id, BYTE* pValue,u32& nBit)
	{
		QcWidget* pWidget = GetWidget(id);
		if (pWidget)
			return pWidget->Update(pValue, nBit, true);
		return false;
	}
	template<typename T>
	bool SetValue(u32 id, T value)
	{
		s32 iValue = value;
		QcWidget* pWidget = GetWidget(id);
		if (pWidget)
			return pWidget->Update(iValue, false);
		return false;
	}
	template<typename T>
	bool GetValue(u32 id, T& value)
	{
		s32 iValue = 0;
		bool bRet = false;
		QcWidget* pWidget = GetWidget(id);
		if (pWidget)
			bRet = pWidget->Update(iValue, true);
		value = T(iValue);
		return bRet;
	}
	template<>
	bool SetValue(u32 id, f64 value)
	{
		QcWidget* pWidget = GetWidget(id);
		if (pWidget)
			return pWidget->Update(value, false);
		return false;
	}
	template<>
	bool GetValue(u32 id, f64& value)
	{
		bool bRet = false;
		QcWidget* pWidget = GetWidget(id);
		if (pWidget)
			bRet = pWidget->Update(value, true);
		return bRet;
	}

	void SetValue(u32 id, const char* psKey, const QcVariant& value);
	void GetValue(u32 id, const char* psKey, QcVariant& value);
	virtual void WidgetVisible(u32 id, bool bVisible);
	
	virtual void SetActive(bool bActive);
	virtual void OnKeyDown(QeKey key, const QsKeyInfo& info);
	virtual void OnKeyUp(QeKey key, const QsKeyInfo& info);

	void Enable(bool bEnable){ m_bEnable = bEnable;}
	bool IsEnable() {return m_bEnable;}
	void SetTip(const QcResImageDesc* pDesc, int iTip = 0);
	const QcResDialogDesc* GetDlgDesc() const{ return m_pDlgDesc;}
	s32 GetInputItemCount() const {return m_arItemWidgetList.size(); }

	void RegisterWidget(u32 nID, QcWidget* pWidget)
	{
		QcWidgetPtrMap::iterator iter(m_mapWidget.find(nID));
		QmAssert(iter == m_mapWidget.end());

		m_mapWidget.insert(make_pair(nID, QcWidgetPtr(pWidget)));
	}
	QcWidget* GetWidget(u32 nID)
	{
		QcWidgetPtrMap::iterator iter(m_mapWidget.find(nID));
		QmAssert(iter != m_mapWidget.end());
		if (iter != m_mapWidget.end())
			return iter->second;
		return NULL;
	}
protected:
	void SetActiveItem(s32 iNewItem, bool bForward);
	QcWidget* GetItem(s32 iActive)
	{
		return m_arItemWidgetList[iActive];
	}
protected:
	QcWidgetCallBacker m_cb;
	QcDialog* m_pParentDlg;
	QcResDialogDesc* m_pDlgDesc;
	QcSceneNodePtr m_pSelectFlagNode;
	QcSceneNodePtr m_pSelectBgNode;
	QcWidgetPtrList m_arItemWidgetList;
	QcWidgetPtrList m_arTipWidgetList;
	QcWidgetPtrList m_arWidgetAll;
	QcWidgetPtrMap m_mapWidget;
	s32 m_iActiveItem;
	bool m_bEnable;
};