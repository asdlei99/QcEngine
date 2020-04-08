#include "pch.hpp"
#include "QcGunMgr.hpp"
#include "QcGun.hpp"
#include "QcPlayer.hpp"
#include "QcSceneDigital.hpp"
#include "QcSceneAnimation.hpp"
#include "Res\QcResGunDesc.hpp"
#include "Res\QcResPlayerDesc.hpp"
#include "Res\QcResManager.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "QcSceneAnimationMgr.hpp"

struct QcGunSorter
{
	bool operator ()(const QcGunPtr& first, const QcGunPtr& second) const
	{
		return first->GetGunDesc()->GetLevelMax() < second->GetGunDesc()->GetLevelMax();
	} 
};

QcGunMgr::QcGunMgr()
: m_iCurrentGun(-1)
, m_iCurrentGunLevel(1)
{

}

QcGunMgr::~QcGunMgr()
{

}

void QcGunMgr::Init(QcPlayer* pPlayer, const QcMathNodeInfo& gunPos)
{
	m_vOffsetTotal = QcPoint(0.f, 20.0f);
	m_pPlayer = pPlayer;
	const QcResPlayerDesc* playerDesc = static_cast<const QcResPlayerDesc*>(m_pPlayer->GetDesc());

	m_fOriental = gunPos.GetRotate();
	QcResManager* pResMgr = QmResMgr;

	for (int i=0; i<QmCountOf(QaGunList); ++i)
	{
		QcGun* pGun = pResMgr->MakeGameInstance(QaGunList[i])->As<QcGun*>();
		pGun->SetPosition(gunPos.GetPos() + m_vOffsetTotal);
		pGun->SetRotation(m_fOriental);
		m_arGun.PushBack(pGun);
	}
	m_pNormalFort = pResMgr->MakeGameInstance(playerDesc->GetFort());
	m_pSuperFort = pResMgr->MakeGameInstance(playerDesc->GetSuperFort());
	m_pGunLevel = pResMgr->MakeGameInstance(playerDesc->GetGunLevelFort())->As<QcSceneDigital*>();
	m_pGunLevel->SetDigitalVisible(true);
	m_pGunLevel->RemoveAnimator<QcAnimatorTexture>();
	m_pGunLevelBgList = m_pGunLevel->GetDesc()->GetStateParameter(eLiving)->m_pTextureList;

	m_pEnergyAnimation = pResMgr->MakeSceneNodeInstance<QcSceneAnimation>(QmObjType(EnergyGunEffect));

	m_pNormalFort->SetPosition(m_vOffsetTotal);
	m_pSuperFort->SetPosition(m_vOffsetTotal);
	m_pGunLevel->SetPosition(m_vOffsetTotal + QcPoint(0.f, 5.0f + 16.f));
	m_pGunLevel->GetDigitalNode().SetPosition(QcPoint(2.f, -5.0f));

	m_iMinLevel = 1;
	m_bSetSuperGun = false;

	std::sort(m_arGun.begin(), m_arGun.end(), QcGunSorter());
}

void QcGunMgr::Update(QcPlayerInfo* pInfo)
{
	u32 iMinLevel = (u32)QmSysSetting->GetMinGunLevel();
	u32 iMaxLevel = (u32)QmSysSetting->GetMaxGunLevel();
	u32 iGunLevel = (u32)pInfo->GetGunLevel();

	if (iGunLevel < iMinLevel)
	{
		iGunLevel = iMinLevel;
	}
	else if (iGunLevel > iMaxLevel)
	{
		iGunLevel = iMaxLevel;
	}
	pInfo->SetGunLevel((int)iGunLevel);
	SwitchGun(iGunLevel);
}

void QcGunMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
	if (m_arMoveNodeList.size())
	{
		std::vector<QcSceneNode*>::iterator iter(m_arMoveNodeList.begin());
		std::vector<QcSceneNode*>::iterator endIter(m_arMoveNodeList.end());
		for (; iter != endIter; ++iter)
		{
			(*iter)->Animate(dCurrentTime, fElapsedTime);
		}

		f32 fMoveLen = m_fMoveSpeed * fElapsedTime;
		m_vOffsetPoint += fMoveLen * m_vMoveVec;

		m_fMoveSpeed += m_fGravitySpeed * fElapsedTime;
		if (!m_bMoveDown && m_fMoveSpeed < 70.f)
		{
			if (GetCurrentGun()->IsSuperGun())
				m_fMoveSpeed = 70.f;
			else
				m_fMoveSpeed = 80.f;
		}
		m_fMoveLen -= fMoveLen;
		if (m_fMoveLen < 0.f)
		{
			if (m_bMoveDown)
				MakeMoveAnimation(m_iCurrentGun, false);
			else
			{
				if (GetCurrentGun()->IsSuperGun())
					m_pEnergyAnimation->SetState(eDying);
				m_arMoveNodeList.clear();
			}
		}
	}
	else
	{
		QcGun* pCurrentGun = GetCurrentGun();
		pCurrentGun->Animate(dCurrentTime, fElapsedTime);
		m_pGunLevel->Animate(dCurrentTime, fElapsedTime);
		if (pCurrentGun->IsSuperGun())
		{
			m_pEnergyAnimation->Animate(dCurrentTime, fElapsedTime);
			m_pSuperFort->Animate(dCurrentTime, fElapsedTime);
		}
		else
		{
			m_pNormalFort->Animate(dCurrentTime, fElapsedTime);
		}
	}
}

void QcGunMgr::RenderFort(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (m_arMoveNodeList.size())
	{
		QcMatrix worldMat(*pWorldMatrix);
		QcMatrix subMat(gsIdentityMatrix);
		subMat.setTranslation(m_vOffsetPoint.X, m_vOffsetPoint.Y);
		worldMat *= subMat;
		m_arMoveNodeList[0]->Render(render, &worldMat);
	}
	else
	{
		if (IsSuperGun())
			m_pSuperFort->Render(render, pWorldMatrix);
		else
			m_pNormalFort->Render(render, pWorldMatrix);
	}
}

void QcGunMgr::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (m_arMoveNodeList.size())
	{
		QcMatrix worldMat(*pWorldMatrix);
		QcMatrix subMat(gsIdentityMatrix);
		subMat.setTranslation(m_vOffsetPoint.X, m_vOffsetPoint.Y);
		worldMat *= subMat;

		std::vector<QcSceneNode*>::iterator iter(m_arMoveNodeList.begin());
		std::vector<QcSceneNode*>::iterator endIter(m_arMoveNodeList.end());
		++iter;	//skip Fort
		for (; iter != endIter; ++iter)
		{
			(*iter)->Render(render, &worldMat);
		}
	}
	else
	{
		QcGun* pCurrentGun = GetCurrentGun();
		pCurrentGun->Render(render, pWorldMatrix);
 		if (pCurrentGun->IsSuperGun())
 			m_pEnergyAnimation->Render(render, pWorldMatrix);
		m_pGunLevel->Render(render, pWorldMatrix);
	}
}

u32 QcGunMgr::AddGunLevel(u32 /*iAdd*/)
{
	if (m_iCurrentGunLevel == (u32)QmSysSetting->GetMaxGunLevel())
	{
		SwitchGun((u32)QmSysSetting->GetMinGunLevel());
	}
	else
	{
		m_iCurrentGunLevel += QmSysSetting->GetJiaPaoFuDu();
		SwitchGun(QcMath::min_(m_iCurrentGunLevel, (u32)QmSysSetting->GetMaxGunLevel()));
	}
	QmPlaySound(AddGunLevelSound);
	//QmSoundMgr->Play(QmSoundType(AddGunLevelSound));
	return m_iCurrentGunLevel;
}

void QcGunMgr::Fire(u32 iScore)
{
	//if (iScore < GetCurrentGun()->GetGunDesc()->GetLevelMin())
	//{
	//	UInt32 uTemp = iScore;
	//	//Int32 iGun = FindGun(uTemp);
 //       QmAssert(uTemp == iScore);
 //       //SwitchGun(iScore);
	//	//GetCurrentGun()->Fire(iScore, m_pPlayer);
	//}
	//else
		GetCurrentGun()->Fire(iScore, m_pPlayer);

}

void QcGunMgr::AdjustGunOriental(f32 fAddRotation)
{
	SetRotation(QcMath::clamp(GetRotation() + fAddRotation, -HALF_PI, HALF_PI));
}

void QcGunMgr::AdjustGunOriental(const QcPoint& vec)
{
	f32 fTargetRotaion = f32(vec.getRadAngleTrig() - m_pPlayer->GetRotation());
	if (fTargetRotaion < - HALF_PI)
		fTargetRotaion += TWO_PI;
	 fTargetRotaion -= HALF_PI;

	if (fTargetRotaion < HALF_PI && fTargetRotaion > -HALF_PI)
	{
		f32 fgap = fTargetRotaion - GetRotation();
		if (fgap > 0)
			AdjustGunOriental(QcMath::min_(QmSysSetting->GetGunRotateSpeed(), fgap));
		else
			AdjustGunOriental(-QcMath::min_(QmSysSetting->GetGunRotateSpeed(), -fgap));
	}
}

void QcGunMgr::AdjustGunOriental(const QcPoint& vec ,f32 fMinRotation)
{
	f32 fTargetRotaion = f32(vec.getRadAngleTrig() - m_pPlayer->GetRotation());
	if (fTargetRotaion < - HALF_PI)
		fTargetRotaion += TWO_PI;
	fTargetRotaion -= HALF_PI;

	if (fTargetRotaion < HALF_PI && fTargetRotaion > -HALF_PI)
	{
		f32 fgap = fTargetRotaion - GetRotation();
		if (fgap > 0)
			AdjustGunOriental(QcMath::min_(fMinRotation, fgap));
		else
			AdjustGunOriental(-QcMath::min_(fMinRotation, -fgap));
	}
}


s32 QcGunMgr::FindGun(u32& iLevel)
{
	s32 iGun = 0;
	QcList<QcGunPtr>::iterator iter(m_arGun.begin());
	QcList<QcGunPtr>::iterator endIter(m_arGun.end());
	for (; iter != endIter; ++iter)
	{
		if (iLevel <= (*iter)->GetGunDesc()->GetLevelMax())
		{
			iGun = iter - m_arGun.begin();
			break;
		}
	}
	if (iter == endIter)
	{
		iGun = 0;
		iLevel = m_iMinLevel;
	}
	if (m_bSetSuperGun)
		++iGun;
	return iGun;
}	

void QcGunMgr::SwitchGun(u32 iLevel, bool switchToSuperGun)
{
	s32 iLastGun = m_iCurrentGun;
	m_iCurrentGunLevel = iLevel;
	m_iCurrentGun = FindGun(m_iCurrentGunLevel);
	m_pGunLevel->SetDigitalValue(iLevel);
	s32 iCurrentTexTure = GetCurrentGun()->GetGunDesc()->GetTextureFortIndex();
	if (m_iCurrentGunLevel > 9 && m_iCurrentGunLevel < 100)
	{
		iCurrentTexTure = 7;
	}
	else if(m_iCurrentGunLevel > 999 && m_iCurrentGunLevel < 10000)
	{
		iCurrentTexTure = 19;
	}
	else if (m_iCurrentGunLevel == 10000)
	{
		iCurrentTexTure = 25;
	}
	m_pGunLevel->SetCurrentTexture(m_pGunLevelBgList[iCurrentTexTure]);	

	SetRotation(m_fOriental);
	//»»ÅÚÌØÐ§
	if (iLastGun != m_iCurrentGun && iLastGun != -1)
	{
		QcPoint temp_pos = m_pPlayer->GetPosition();
		f32 rotation = m_pPlayer->GetRotation();
		temp_pos.X += 40 * cos(rotation + HALF_PI);
		temp_pos.Y += 40 * sin(rotation + HALF_PI);
		QmSceneMgr->GetAnimationMgr()->AddParticleMP(eAnimationLayerGunEffect, "changegun",temp_pos);
		if (!switchToSuperGun)
			QmPlaySound(SwitchGunSound);
			//QmSoundMgr->Play(QmSoundType(SwitchGunSound));
	}
	
	//end
 	if (switchToSuperGun)
	{
 		MakeMoveAnimation(iLastGun == -1 ? m_iCurrentGun : iLastGun,  iLastGun != -1);
		QmPlaySound(SuperGunShowSound);
		//QmSoundMgr->Play(QmSoundType(SuperGunShowSound));
	}
}

void QcGunMgr::MakeMoveAnimation(s32 iGun, bool bMoveDown)
{
	const QcGun* pPrevGun = GetGun(iGun);

	if (bMoveDown)
	{
		m_vOffsetPoint.X = 0.f;
		m_vOffsetPoint.Y = 0.f;
	}
	else
	{
		m_vOffsetPoint.X = 0.f;
		m_vOffsetPoint.Y = - (pPrevGun->GetHeight() + m_vOffsetTotal.Y);
	}
	m_bMoveDown = bMoveDown;
	m_fMoveLen = pPrevGun->GetHeight() + m_vOffsetTotal.Y;
	m_vMoveVec.X = 0.f;
	if (bMoveDown)
	{
		m_vMoveVec.Y = -1.0f;
		m_fMoveSpeed = 600.f;
		m_fGravitySpeed = 0.f;
	}
	else
	{	
		if (GetCurrentGun()->IsSuperGun())
		{
			m_vMoveVec.Y = 1.0f;
			m_fMoveSpeed = 600.f;
			m_fGravitySpeed = -1300.f;
		}
		else
		{
			m_vMoveVec.Y = 1.0f;
			m_fMoveSpeed = 500.f;
			m_fGravitySpeed = -600.f;
		}
	}

	m_arMoveNodeList.clear();
	m_arMoveNodeList.push_back(pPrevGun->IsSuperGun() ? m_pSuperFort : m_pNormalFort);
	m_arMoveNodeList.push_back(const_cast<QcGun*>(pPrevGun));
	if (pPrevGun->IsSuperGun())
	{
		m_arMoveNodeList.push_back(m_pEnergyAnimation);
		m_pEnergyAnimation->SetState(eLiving);
	}
	m_arMoveNodeList.push_back(m_pGunLevel);
}

void QcGunMgr::SetRotation(f32 foriental)
{
	m_fOriental = foriental;
	GetCurrentGun()->SetRotation(m_fOriental);
}

void QcGunMgr::FreeFire( u32 iScore )
{
	GetCurrentGun()->FreeFire(iScore, m_pPlayer);
}
