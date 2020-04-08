#include "pch.hpp"
#include "QcDialog.hpp"
#include "QcWidget.hpp"
#include "QcResDialogDesc.hpp"

ImplRuntimeClass(QcDialog);

QcDialog::QcDialog()
{
	m_iActiveItem = -1;
}

void QcDialog::InitDialog(QcResDialogDesc* pDialogDesc, QcDialog* pParentDlg)
{
	m_pDlgDesc = pDialogDesc;
	m_pParentDlg = pParentDlg;

	Init(*pDialogDesc->GetBg(), false);

	if (pDialogDesc->GetSelectFlag())
	{
		m_pSelectFlagNode = QfMakeSceneNodeInstance<QcSceneNode>(pDialogDesc->GetSelectFlag());
		AddSubSceneNode(m_pSelectFlagNode);
	}
	if (pDialogDesc->GetSelectBg())
	{
		m_pSelectBgNode = QfMakeSceneNodeInstance<QcSceneNode>(pDialogDesc->GetSelectBg());
		AddSubSceneNode(m_pSelectBgNode);
	}

	QcResDialogItemDescList& staticList = pDialogDesc->GetStaticList();
	QcResDialogItemDescListIter iter(staticList.begin());
	for (; iter != staticList.end(); ++iter)
	{
		QcWidget* pWidget = (*iter)->GetWidgetClass()->CreateObject<QcWidget>();
		if (pWidget)
			pWidget->InitWidget((*iter), this);
		m_arWidgetAll.PushBack(pWidget);
		AddSubSceneNode(pWidget);
	}
	
	QcResDialogItemDescList& tipList = pDialogDesc->GetTipList();
	iter = tipList.begin();
	for (; iter != tipList.end(); ++iter)
	{
		QcWidget* pWidget = (*iter)->GetWidgetClass()->CreateObject<QcWidget>();
		if (pWidget)
			pWidget->InitWidget((*iter), this);
		m_arTipWidgetList.PushBack(pWidget);
		m_arWidgetAll.PushBack(pWidget);
		AddSubSceneNode(pWidget);
	}

	QcResDialogItemDescList& itemList = pDialogDesc->GetItemList();
	iter = itemList.begin();
	for (; iter != itemList.end(); ++iter)
	{
		QcWidget* pWidget = (*iter)->GetWidgetClass()->CreateObject<QcWidget>();
		if (pWidget)
			pWidget->InitWidget((*iter), this);
		if (pWidget->IsAcceptInput())
			m_arItemWidgetList.PushBack(pWidget);
		m_arWidgetAll.PushBack(pWidget);
		AddSubSceneNode(pWidget);
	}

	if (pDialogDesc->GetAction().size())
	{
		if (!FindWidgetCallBacker(pDialogDesc->GetAction(), m_cb))
			m_cb = QmSettingMgr->FindCallBacker(pDialogDesc->GetAction()); 
		m_cb(eDlgInit);
	}

	SetActiveItem(0, false);

	Enable(true);
}

void QcDialog::SetActive(bool bActive)
{
	if (m_pSelectFlagNode)
		m_pSelectFlagNode->SetVisible(bActive);
	if (m_pSelectBgNode)
		m_pSelectBgNode->SetVisible(bActive);
	if (bActive)
	{
		if (m_cb)
			m_cb(eDlgInit);

		for (int i=0; i<(int)m_arItemWidgetList.size(); ++i)
		{
			GetItem(i)->SetActive(false);
		}
		SetActiveItem(-1, true);
		m_iActiveItem = -1;
		SetActiveItem(0, true);
	}
}

bool QcDialog::FindWidgetCallBacker(const QcString& sAction, QcWidgetCallBacker& cb)
{
	if (m_pParentDlg)
		return m_pParentDlg->FindWidgetCallBacker(sAction, cb);
	return false;
}
void QcDialog::OnKeyDown(QeKey key, const QsKeyInfo& info)
{
	if (m_iActiveItem < 0 || !IsEnable())
		return;

	switch(key)
	{
	case eSmallKeyLeft:
		{
			if (!GetItem(m_iActiveItem)->OnLeftKey(info))
			{
				SetActiveItem(QcMath::surround(m_iActiveItem - 1, m_arItemWidgetList.size()), false);
			}
			break;
		}
	case eSmallKeyRight:
		{
			if (!GetItem(m_iActiveItem)->OnRightKey(info))
			{
				SetActiveItem(QcMath::surround(m_iActiveItem + 1, m_arItemWidgetList.size()), true);
			}
			break;
		}
	case eSmallKeyDown:
		{
			if (!GetItem(m_iActiveItem)->OnDownKey(info))
			{
				SetActiveItem(QcMath::surround(m_iActiveItem + 1, m_arItemWidgetList.size()), false);
			}
			break;
		}
	case eSmallKeyUp:
		{
			if (!GetItem(m_iActiveItem)->OnUpKey(info))
			{
				SetActiveItem(QcMath::surround(m_iActiveItem - 1, m_arItemWidgetList.size()), true);
			}
			break;
		}
	}
}

void QcDialog::OnKeyUp(QeKey key, const QsKeyInfo& info)
{
	if (m_iActiveItem < 0 || !IsEnable())
		return;

	switch(key)
	{
	case eSmallKeyCancel:
		{
			if (!GetItem(m_iActiveItem)->OnCancelKey(info) && m_cb)
			{
				m_cb(eDlgCancel);
			}
			break;
		}
	case eSmallKeyOk:
		{
			if (!GetItem(m_iActiveItem)->OnOkKey(info) && m_cb)
			{
				m_cb(eDlgOK);
			}
			break;
		}
	}
}

void QcDialog::SetActiveItem(s32 iNewItem, bool bForward)
{
	if (iNewItem != -1 && iNewItem == m_iActiveItem)
	{
		GetItem(m_iActiveItem)->ActiveAgain(bForward);
	}
	else if (m_iActiveItem != -1)
	{
		GetItem(m_iActiveItem)->SetActive(false);

		if (GetItem(m_iActiveItem)->GetDesc()->GetStateParameter(eLiving) != NULL)
			GetItem(m_iActiveItem)->SetState(eLiving);
	}

	if (iNewItem >=0 && iNewItem < (s32)m_arItemWidgetList.size())
	{
		m_iActiveItem = iNewItem;
		QcWidget* pWidget = GetItem(m_iActiveItem);
		pWidget->SetActive(true);
		pWidget->ActiveAgain(bForward);
		if (pWidget->GetDesc()->GetStateParameter(eDying) != NULL)
			pWidget->SetState(eDying);

		SetTip(pWidget->GetWidgetDesc()->GetTipDesc());

		QcPoint pos = pWidget->GetPosition();
		QcPoint leftCenterPos;
		pos.X -= pWidget->GetWidth() * 0.5f;
		leftCenterPos = pos;
		pos.X += pWidget->GetWidgetDesc()->GetWidth() * 0.5f;
		if (m_pSelectBgNode)
		{
			m_pSelectBgNode->SetPosition(pos);
			m_pSelectBgNode->ScaleTo(pWidget->GetWidgetDesc()->GetWidth(), pWidget->GetWidgetDesc()->GetHeight());
		}

		pos.X = leftCenterPos.X - 40.f;
		if (m_pSelectFlagNode)
			m_pSelectFlagNode->SetPosition(pos);
	}
}

void QcDialog::Update(bool bUpdate)
{
	QcWidgetPtrListIter iter(m_arWidgetAll.begin());
	for (; iter != m_arWidgetAll.end(); ++iter)
	{
		(*iter)->Update(bUpdate);
	}
}

void QcDialog::Update(u32 id, bool bUpdateData)
{
	QcWidget* pWidget = GetWidget(id);
	if (pWidget)
		pWidget->Update(bUpdateData);
}

void QcDialog::SetValue(u32 nID, const char* psKey, const QcVariant& value)
{
	QcWidget* pWidget = GetWidget(nID);
	if (pWidget)
		return pWidget->SetValue(psKey, value);
}
void QcDialog::GetValue(u32 nID, const char* psKey, QcVariant& value)
{
	QcWidget* pWidget = GetWidget(nID);
	if (pWidget)
		return pWidget->GetValue(psKey, value);
}

void QcDialog::WidgetVisible(u32 nID, bool bVisible)
{
	QcWidget* pWidget = GetWidget(nID);
	if (pWidget)
		pWidget->SetVisible(bVisible);
}

void QcDialog::SetTip(const QcResImageDesc* pDesc, int iTip)
{
	if (iTip >=0 && iTip < (int)m_arTipWidgetList.size())
	{
		m_arTipWidgetList[iTip]->RemoveSubSceneNode(NULL);
		if (pDesc)
		{
			QcWidget* pWidget = new QcWidget();
			QcResDialogItemDescPtr pWidgetDesc(new QcResDialogItemDesc());
			pWidget->InitWidget(pWidgetDesc, this);
			pWidget->SetWidgetImage(pDesc);
			m_arTipWidgetList[iTip]->AddSubSceneNode(pWidget);
		}
	}
}