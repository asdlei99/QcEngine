#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcWidget.hpp"
#include "QcSettingMgr.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "Animator\QcAnimatorFlash.hpp"
#include "Res\QcResDescParam.hpp"
#include "Res\QcResManager.hpp"

ImplRuntimeClass(QcWidget);

QcWidget::QcWidget()
{
	m_iActiveItem = -1;
	m_pWidgetDesc = NULL;
	m_bFocus = false;
}

void QcWidget::InitWidget(QcResDialogItemDesc* pWidgetDesc, QcDialog* pDialog)
{
	if (pWidgetDesc->GetID() != 0)
		pDialog->RegisterWidget(pWidgetDesc->GetID(), this);

	if (pWidgetDesc->GetWidgetImage())
		Init(*(pWidgetDesc->GetWidgetImage()), false);
	else
		Init(*QmResMgr->GetEmptyImageDesc(), false);

	m_pWidgetDesc = pWidgetDesc;
	m_pDialog = pDialog;
	m_vAlignPos = pWidgetDesc->GetAlignPos();
	m_eAlign = pWidgetDesc->GetAlign();
	if (!pWidgetDesc->GetObjType().empty())
		m_objType = pWidgetDesc->GetObjType();

	QcResDialogItemDescList& subWidgetList = pWidgetDesc->GetSubWidetDescList();
	QcResDialogItemDescListIter iter(subWidgetList.begin());
	for (; iter != subWidgetList.end(); ++iter)
	{
		QcWidget* pWidget = (*iter)->GetWidgetClass()->CreateObject<QcWidget>();
		if (pWidget)
			pWidget->InitWidget((*iter), m_pDialog);
		AddSubSceneNode(pWidget);
		if (pWidget->IsAcceptInput())
			m_arItemList.PushBack(pWidget);
	}
	
	m_iActiveItem = -1;
	SetPosition(AdjustAlginPos(m_eAlign, m_vAlignPos, GetWidth(), GetHeight(), m_vPos));

	if (pWidgetDesc->GetAction().size())
	{
		FindWidgetCallBacker(pWidgetDesc->GetAction(), m_cb);
		QmAssert(m_cb);
	}
	ResetUpdateTime(false);
}

void QcWidget::ResetUpdateTime(bool bKill)
{
	if (bKill)
	{
		KillTimer(eWidgetUpdateTimer);
	}
	else
	{
		if (!QcMath::iszero(m_pWidgetDesc->GetUpdateTime()) && m_cb)
		{
			SetTimer(eWidgetUpdateTimer, m_pWidgetDesc->GetUpdateTime());
		}
	}
}

bool QcWidget::FindWidgetCallBacker(const QcString& sAction, QcWidgetCallBacker& cb)
{
	if (!m_pDialog->FindWidgetCallBacker(sAction, cb))
		cb = QmSettingMgr->FindCallBacker(sAction); 
	return cb;
}

void QcWidget::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eWidgetUpdateTimer && m_cb)
		Update(false);
	else
		QcSceneNode::OnTimer(nTimer, pContext);
}

bool QcWidget::IsAcceptInput()
{
	if (m_pWidgetDesc && m_pWidgetDesc->IsAcceptInput())
		return true;

	QcSceneNodeListIter subiter(m_arSubList.begin());
	for (; subiter != m_arSubList.end(); ++subiter)
	{
		QcWidget* pWidget = static_cast<QcWidget*>((*subiter).Get());
		if (pWidget && pWidget->IsAcceptInput())
		{
			return true;
		}
	}
	return false;
}

u32 QcWidget::GetID() const
{
	return m_pWidgetDesc->GetID();
}

void QcWidget::SetActive(bool bActive)
{
	QcWidgetPtrListIter iter(m_arItemList.begin());
	for (; iter != m_arItemList.end(); ++iter)
	{
		(*iter)->SetActive(bActive);
	}
}

void QcWidget::SetActiveItem(s32 iItem, bool bForward)
{
	if (iItem != -1 && iItem == m_iActiveItem)
	{
		GetInputWidget(m_iActiveItem)->ActiveAgain(bForward);
	}
	else
	{
		if (m_iActiveItem != -1)
		{
			s32 iActiveItem =  m_iActiveItem;
			m_iActiveItem = -1;

			QcWidget* pWidget = GetInputWidget(iActiveItem);
			pWidget->SetFocus(false);
			iActiveItem = pWidget->m_iActiveItem;
			while (iActiveItem != -1)
			{
				pWidget->m_iActiveItem = -1;
				pWidget = pWidget->GetInputWidget(iActiveItem);
				pWidget->SetFocus(false);
				iActiveItem = pWidget->m_iActiveItem;
			}
			pWidget->EndFlash();
		}
		m_iActiveItem = iItem;
		if (m_iActiveItem != -1)
		{
			QcWidget* pWidget = GetInputWidget(m_iActiveItem);
			while (pWidget->m_arItemList.size())
			{
				pWidget->m_iActiveItem = bForward ? 0 : pWidget->m_arItemList.size() - 1;
				pWidget = pWidget->GetInputWidget(pWidget->m_iActiveItem);
			}
			pWidget->SetFocus(true);
			if (IsMoreThanOneInputWidget())
			{
				pWidget->StartFlash();
			}
		}
	}
}

bool QcWidget::IsMoreThanOneInputWidget()
{
	bool bFlash = m_arItemList.size() > 1;
	QcWidget* pWidget = dynamic_cast<QcWidget*>(m_pParentNode);
	while (!bFlash && pWidget)
	{
		bFlash = pWidget->m_arItemList.size() > 1;
		pWidget = dynamic_cast<QcWidget*>(pWidget->m_pParentNode);
	}
	return bFlash;
}

void QcWidget::StartFlash(f32 fFlashTime)
{
	if (m_pFlashAnimator)
	{
		m_pFlashAnimator->Reset(fFlashTime, fFlashTime);
	}
	else
	{
		m_pFlashAnimator = (QcAnimatorFlash*)QmAnimatorCreator->MakeFlashAnimator(fFlashTime, fFlashTime);
		AddAnimator(m_pFlashAnimator);
	}
}

void QcWidget::EndFlash()
{
	if (m_pFlashAnimator)
	{
		m_pFlashAnimator->Suspend(true);
		SetVisible(true);
	}
}

void QcWidget::Update(bool bUpdateData)
{
	QcSceneNodeListIter iter(m_arSubList.begin());
	for (; iter != m_arSubList.end(); ++iter)
	{
		QcWidget* pWidget = dynamic_cast<QcWidget*>((*iter).Get());
		if (pWidget)
		{
			pWidget->Update(bUpdateData);
		}
	}
}

bool QcWidget::OnUpKey(const QsKeyInfo& info)
{
	if (m_iActiveItem != -1)
	{
		if (GetInputWidget(m_iActiveItem)->OnUpKey(info))
			return true;

		if (m_arItemList.size() > 1)
		{
			SetActiveItem(QcMath::surround(m_iActiveItem - 1, m_arItemList.size()), false);
			return true;
		}
		SetActiveItem(-1);
	}
	return false;
}

bool QcWidget::OnDownKey(const QsKeyInfo& info)
{
	if (m_iActiveItem != -1)
	{
		if (GetInputWidget(m_iActiveItem)->OnDownKey(info))
			return true;

		if (m_arItemList.size() > 1)
		{
			SetActiveItem(QcMath::surround(m_iActiveItem + 1, m_arItemList.size()), true);
			return true;
		}
		SetActiveItem(-1);
	}
	return false;
}

bool QcWidget::OnLeftKey(const QsKeyInfo& info)
{
	if (m_iActiveItem != -1)
	{
		if (GetInputWidget(m_iActiveItem)->OnLeftKey(info))
			return true;

		if (m_arItemList.size() > 1)
		{
			SetActiveItem(QcMath::surround(m_iActiveItem + 1, m_arItemList.size()), false);
			return true;
		}
	}
	else if (m_arItemList.size())
	{
		SetActiveItem(0, false);
		if (m_arItemList.size() == 1)
			GetInputWidget(m_iActiveItem)->OnLeftKey(info);
		return true;
	}
	return false;
}

bool QcWidget::OnRightKey(const QsKeyInfo& info)
{
	if (m_iActiveItem != -1)
	{
		if (GetInputWidget(m_iActiveItem)->OnRightKey(info))
			return true;

		if (m_arItemList.size() > 1)
		{
			SetActiveItem(QcMath::surround(m_iActiveItem + 1, m_arItemList.size()), true);
			return true;
		}
	}
	else if (m_arItemList.size())
	{
		SetActiveItem(m_arItemList.size() - 1, true);
		if (m_arItemList.size() == 1)
			GetInputWidget(m_iActiveItem)->OnRightKey(info);
		return true;;
	}
	return false;
}

bool QcWidget::OnOkKey(const QsKeyInfo& info)
{
	if (m_iActiveItem != -1)
	{
		if (GetInputWidget(m_iActiveItem)->OnOkKey(info))
			return true;
		SetActiveItem(-1);
	}
	return false;
}

bool QcWidget::OnCancelKey(const QsKeyInfo& info)
{
	if (m_iActiveItem != -1)
	{
		if (GetInputWidget(m_iActiveItem)->OnCancelKey(info))
			return true;
		SetActiveItem(-1);
	}
	return false;
}

void QcWidget::SetWidgetImage(const QcResImageDesc* pImage)
{
	m_pDesc = const_cast<QcResImageDesc*>(pImage);
	SetCurrentTexture(NULL);
	const QsStateParameter* pStateParam = pImage->GetStateParameter(eLiving);
	if (pStateParam)
	{
		QcAnimatorTexture* pTexture = FindAnimator<QcAnimatorTexture>();
		if (pTexture)
		{
			pTexture->Reset(*pStateParam);
		}
		else
		{
			if (pStateParam->m_pTextureList.size())
			{
				if (pStateParam->m_pTextureList.size() == 1)
					SetCurrentTexture(pStateParam->m_pTextureList[0]);
				else
					AddAnimator(QmAnimatorCreator->MakeTextureAnimator(pStateParam));
			}
		}
	}
	ScaleTo(m_pDesc->GetWidth(), m_pDesc->GetHeight());
	SetPosition(AdjustAlginPos(m_eAlign, m_vAlignPos, m_pDesc->GetWidth(), m_pDesc->GetHeight(), m_vPos));
}