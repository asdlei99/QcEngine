#pragma once

#include "Setting\QcSetting.hpp"

class QcSettingMgr : public QcObjectEx
{
	typedef QcSharedPtr<QcResDialogDesc> QcResDialogDescPtr; 
	typedef std::map<QcObjType, QcResDialogDescPtr, QcNoCaseCompare> QcResDialogDescPtrMap;
	typedef QcResDialogDescPtrMap::iterator QcResDialogDescPtrMapIter;
public:
	QcSettingMgr(QcResManager* pResMgr);
	virtual ~QcSettingMgr();

	void Init(QcResManager* pResMgr);
	virtual void OnEvent(const QsEvent& evt);
	void Register(const char* psName, const QcWidgetCallBacker& cb);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render, int iRenderScreenCount);
	
	QcWidgetCallBacker& FindCallBacker(const char* psName)
	{
		return FindCallBacker(QcString(psName));
	}
	QcWidgetCallBacker& FindCallBacker(const QcString& sName);

	void EnterSetting();
	void ExitSetting();

	void OnKeyDown(u32 eKey, const QsKeyInfo& info);
	void OnKeyUp(u32 eKey, const QsKeyInfo& info);
	void LoadDesc(QcResDialogDesc* pDesc);
	void ToShowDialog(const QcObjType& objType, bool bHideActive = true);
	void HideDialog(const QcObjType& objType);
	QcDialog* GetActiveDialog() 
	{
		if (m_dialogList.size())
			return m_dialogList.back();
		return NULL;
	}
	QcResDialogDesc* FindDialogDesc(const QcObjType& objType)
	{
		QcResDialogDescPtrMapIter iter = m_dialogMap.find(objType);
		if (iter != m_dialogMap.end())
			return iter->second.Get();
		return NULL;
	}
	int FindDialog(const QcObjType& objType);
	QcDialog* FindDialogEx(const QcObjType& objType)
	{
		return m_dialogList[FindDialog(objType)];
	}
	QcDialogPtrList& GetDialogList() { return m_dialogList; }
	void ShowDialog(const QcObjType& objType, bool bHideActive = true);
protected:
	QcSettingExchanger* GetSettingExchanger(){ return m_pSettingExchanger;}
protected:
	QcResManager* m_pResMgr;
	QcSharedPtr<QcSettingExchanger> m_pSettingExchanger;
	QcResDialogDescPtrMap m_dialogMap;
	std::map<QcString, QcWidgetCallBacker> m_cbFunMap;
	QcDialogPtrList m_dialogList;
	QcObjType m_toDisplayDlg;
};