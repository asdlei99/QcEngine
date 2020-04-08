#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcSettingMgr.hpp"
#include "QcSettingExchanger.hpp"

QcSettingMgr::QcSettingMgr(QcResManager* pResMgr)
: m_pResMgr(pResMgr), m_pSettingExchanger(NULL)
{
	Init(m_pResMgr);
}

QcSettingMgr::~QcSettingMgr()
{
}

void QcSettingMgr::Init(QcResManager* pResMgr)
{
	m_pResMgr = pResMgr;
}

void QcSettingMgr::OnEvent(const QsEvent& evt)
{
	if (evt.m_iEvt == eEvtShowDialog)
	{
		ShowDialog(m_toDisplayDlg, evt.m_iParam == 1);
	}
}

void QcSettingMgr::Register(const char* psName, const QcWidgetCallBacker& cb)
{
	m_cbFunMap[psName] = cb;
}

QcWidgetCallBacker& QcSettingMgr::FindCallBacker(const QcString& sName)
{
	std::map<QcString, QcWidgetCallBacker>::iterator iter(m_cbFunMap.find(sName));
	if (iter != m_cbFunMap.end())
		return iter->second;
	static QcWidgetCallBacker gsCb;
	return gsCb;
}

void QcSettingMgr::EnterSetting()
{
	if (GetSettingExchanger() == NULL)
		m_pSettingExchanger = new QcSettingExchanger();
	m_pSettingExchanger->Init(this, QmSysSetting);

	if (QmSysSetting->GetDaMaFlag() != eDaMaNone)
		ShowDialog(QmObjType(Account));
	else 
		ShowDialog(QmObjType(SettingMain));
}
void QcSettingMgr::ExitSetting()
{
	m_toDisplayDlg = ID_NULL;
	m_dialogList.Clear();
	m_cbFunMap.clear();
	m_dialogMap.clear();
	m_pSettingExchanger = NULL;
}

void QcSettingMgr::OnKeyDown(u32 key, const QsKeyInfo& info)
{
	if (GetActiveDialog() && key >= eSmallKeyUp && key < eKeyTotal)
		GetActiveDialog()->OnKeyDown((QeKey)key, info);
}
void QcSettingMgr::OnKeyUp(u32 key, const QsKeyInfo& info)
{
	if (GetActiveDialog() && key >= eSmallKeyUp && key < eKeyTotal)
		GetActiveDialog()->OnKeyUp((QeKey)key, info);
}

void QcSettingMgr::LoadDesc(QcResDialogDesc* pDesc)
{
	m_dialogMap[pDesc->GetObjType()] = pDesc;
	pDesc->LoadTexture(m_pResMgr);
}

void QcSettingMgr::ToShowDialog(const QcObjType& objType, bool bHideActive)
{
	PostEvent(eEvtShowDialog, bHideActive ? 1 : 0);
	m_toDisplayDlg = objType;
}

void QcSettingMgr::ShowDialog(const QcObjType& objType, bool bHideActive)
{
	if (GetActiveDialog())
	{
		GetActiveDialog()->SetActive(false);
		if (bHideActive)
			GetActiveDialog()->SetVisible(false);
	}
	int iPos = FindDialog(objType);
	if (iPos != -1)
	{
		if (iPos != (int)(m_dialogList.size() - 1))
			QcMath::swap(m_dialogList.back(), m_dialogList[iPos]);
	}
	else
	{
		QcResDialogDesc* pDlgDesc = FindDialogDesc(objType);
		QcDialog* pDialog = pDlgDesc->MakeInstance();
		m_dialogList.PushBack(pDialog);
		pDialog->InitDialog(pDlgDesc);
	}
	GetActiveDialog()->SetVisible(true);
	GetActiveDialog()->Update(false);
	GetActiveDialog()->SetActive(true);
}

void QcSettingMgr::HideDialog(const QcObjType& objType)
{
	int iPos = FindDialog(objType);
	if (iPos != -1)
	{
		m_dialogList[iPos]->SetVisible(false);
		m_dialogList[iPos]->SetActive(false);
	}
}

int QcSettingMgr::FindDialog(const QcObjType& objType)
{
	QcDialogPtrListIter iter(m_dialogList.begin());
	for (; iter !=m_dialogList.end(); ++iter)
	{
		if ((*iter)->GetDlgDesc()->GetObjType() == objType)
			return iter-m_dialogList.begin();
	}
	return -1;
}

void QcSettingMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcDialogPtrList::iterator iter(m_dialogList.begin());
	for (; iter != m_dialogList.end(); ++iter)
	{
		if ((*iter)->IsVisible())
			(*iter)->Animate(dCurrentTime, fElapsedTime);
	}
}
void QcSettingMgr::Render(QcDeviceRender& render, int iRenderScreenCount)
{
	for (int i=0; i<iRenderScreenCount; ++i)
	{
		if (iRenderScreenCount != 1)
			render.SwitchToSubScreen(i);
		QcDialogPtrList::iterator iter(m_dialogList.begin());
		for (; iter != m_dialogList.end(); ++iter)
		{
			if ((*iter)->IsVisible())
				(*iter)->Render(render);
		}
	}
	if (iRenderScreenCount != 1)
		render.SwitchToSubScreen(-1);
}

