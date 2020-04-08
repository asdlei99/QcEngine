#pragma once

class QcAnimatorFlash;
class QcWidget : public QcSceneNode
{
	DeclareRuntimeClass(QcWidget);
public:
	QcWidget();

	virtual void InitWidget(QcResDialogItemDesc* pWidgetDesc, QcDialog* pDialog);
	virtual void OnTimer(u32 nTimer, void* pContext);
	virtual bool OnUpKey(const QsKeyInfo& info);
	virtual bool OnDownKey(const QsKeyInfo& info);
	virtual bool OnLeftKey(const QsKeyInfo& info);
	virtual bool OnRightKey(const QsKeyInfo& info);
	virtual void ActiveAgain(bool bForward){};
	virtual bool OnOkKey(const QsKeyInfo& info);
	virtual bool OnCancelKey(const QsKeyInfo& info);
	virtual bool IsAcceptInput();
	virtual void SetActive(bool bActive);
	virtual void SetFocus(bool bFocus){m_bFocus = bFocus;}
	virtual void StartFlash(f32 fFlashTime = 0.3f);
	virtual void EndFlash();
	virtual void Update(bool bUpdateData);
	virtual bool Update(BYTE* pValue,u32& nBit, bool bUpdateData){return false;}
	virtual bool Update(s32& iValue, bool bUpdateData){return false;}
	virtual bool Update(f64& fValue, bool bUpdateData){return false;}
	virtual void SetValue(const char* psKey, const QcVariant& value){}
	virtual void GetValue(const char* psKey, QcVariant& value){}

	void ResetUpdateTime(bool bKill = false);
	u32 GetID() const;
	QeAlign GetAlign() const {return m_eAlign;}
	const QcPoint& GetAlignPos() const {return m_vAlignPos;}
	QcResDialogItemDesc* GetWidgetDesc() {return m_pWidgetDesc;}
	void SetWidgetImage(const QcResImageDesc* pImage);
	bool FindWidgetCallBacker(const QcString& sAction, QcWidgetCallBacker& cb);
protected:
	bool IsMoreThanOneInputWidget();
	void SetActiveItem(s32 iItem, bool bForward = false);
	QcWidget* GetInputWidget(s32 iItem)
	{
		return m_arItemList[iItem];
	}
	QcWidgetCallBacker& GetCb() {return m_cb;}
protected:
	bool m_bFocus;
	QcWidgetCallBacker m_cb;
	QcSharedPtr<QcAnimatorFlash> m_pFlashAnimator;
	QcResDialogItemDescPtr m_pWidgetDesc;
	QeAlign m_eAlign;
	QcPoint m_vAlignPos;
	QcDialog* m_pDialog;
	s32 m_iActiveItem;
	QcWidgetPtrList m_arItemList;
};