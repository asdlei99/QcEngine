#include "pch.hpp"
#include "Scene/QcVolCtrl.hpp"

ImplRuntimeClass(QcVolCtrl);

#define MAX_VOL_LEVEL 10

QcVolCtrl::QcVolCtrl() : m_MyDisplayTime(0)
{
	u32 vol = QmSysSetting->GetVolume();
	QmSysSetting->SetVolume(vol);
	vol >>= 16;
	m_VolumnLevel = f32(f32(vol)/0xffff * MAX_VOL_LEVEL);
	m_VolumnLevel = QcMath::clamp(m_VolumnLevel, 0.f, 10.f);
}

void QcVolCtrl::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcBaseClass::Init(desc,bLoadSubNode);
	QcResImageDesc* pDesc = QmResMgr->GetImageDesc(QmObjName(VolumnBlock));
	for (int i=0; i<MAX_VOL_LEVEL; ++i)
	{
		f32 height = pDesc->GetHeight();
		height *= 1 + (i / 12.f); 
		QcSceneNode* pNode  = pDesc->MakeInstance();
		pNode->SetPosition(QcPoint(-200 + i * 33.f,14));
		pNode->ScaleTo(pDesc->GetWidth()*1.3f, height);
		m_arPointList.PushBack(pNode);
	}
	m_pNumNode = QfMakeEmptyInstance<QcSceneAnimation>();
	//m_pNumNode->SetDigitalType(QmObjName(Volumn));
	QmSetDigitalType(m_pNumNode->GetDigitalNode(), Volumn);
	m_pNumNode->SetDigitalVisible(true);
	m_pNumNode->SetDigitalValue(s64(m_VolumnLevel * 2));
	m_pNumNode->SetPosition(QcPoint(145,4));
}

QcVolCtrl::~QcVolCtrl()
{
	
}

void QcVolCtrl::RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	for (u32 i=0; i<floorf(m_VolumnLevel); ++i)
	{
		m_arPointList[i]->Render(render, pWorldMatrix);
	}
	QcBaseClass::RenderSub(render, pWorldMatrix);

	m_pNumNode->Render(render, pWorldMatrix);
}

void QcVolCtrl::Animate(double dCurrentTime,f32 fElapsedTime)
{
	SetVisible(m_MyDisplayTime > 0.f);
	if (IsVisible())
	{
		m_MyDisplayTime -= fElapsedTime;

		for (u32 i=0; i<floorf(m_VolumnLevel); ++i)
		{
			m_arPointList[i]->Animate(dCurrentTime, fElapsedTime);
		}
		m_pNumNode->Animate(dCurrentTime, fElapsedTime);
	}
}

void QcVolCtrl::OnKeyDown(u32 key, const QsKeyInfo& info)
{
	if (key == eSmallKeyLeft)
	{
		if ( (info.fKeyDownTime<info.fGapTime || info.fKeyDownTime>=0.5f) && m_MyDisplayTime > 0 && m_VolumnLevel > 0.f)
			m_VolumnLevel -= 0.5f;
		if (m_VolumnLevel < 0.f)
		{
			m_VolumnLevel = 0.f;
		}
		m_MyDisplayTime = 3;

		m_pNumNode->SetDigitalValue(s64(m_VolumnLevel * 2));
		f32 fLv = m_VolumnLevel/f32(MAX_VOL_LEVEL);
		DWORD vol = DWORD(fLv * 0xffff);
		//waveOutSetVolume(0, (vol<<16)|vol);
		QmSysSetting->SetVolume((vol<<16)|vol);

		if(m_VolumnLevel == 0.f)
		{
			this->GetSubSceneNode(QmObjName(VolumnLogo))->SetColor(QcColorf(0.5f,0.5f,0.5f));
		}
		else
			this->GetSubSceneNode(QmObjName(VolumnLogo))->SetColor(QcColorf(1.f,1.f,1.f));
	}
	else if (key == eSmallKeyRight)
	{
		if ( (info.fKeyDownTime<info.fGapTime || info.fKeyDownTime>=0.5f) && m_MyDisplayTime > 0 && m_VolumnLevel < MAX_VOL_LEVEL)
			m_VolumnLevel += 0.5f;
		if (m_VolumnLevel > 10.f)
		{
			m_VolumnLevel = 10.f;
		}
		m_MyDisplayTime = 3;

		m_pNumNode->SetDigitalValue(s64(m_VolumnLevel * 2));
		f32 fLv = m_VolumnLevel/f32(MAX_VOL_LEVEL);
		DWORD vol = DWORD(fLv * 0xffff);
		//waveOutSetVolume(0, (vol<<16)|vol);
		QmSysSetting->SetVolume((vol<<16)|vol);

		if(m_VolumnLevel == 0.f)
		{
			this->GetSubSceneNode(QmObjName(VolumnLogo))->SetColor(QcColorf(0.5f,0.5f,0.5f));
		}
		else
			this->GetSubSceneNode(QmObjName(VolumnLogo))->SetColor(QcColorf(1.f,1.f,1.f));
	}
}