#include "pch.hpp"
#include "QcSceneNode.hpp"
#include "QiSceneNodeMonitor.hpp"
#include "RenderSystem\QcDeviceRender.hpp"
#include "Res\QcTexture.hpp"
#include "Res\QcResImageDesc.hpp"
#include "Res\QcResManager.hpp"
#include "Animator\QcAnimatorTexture.hpp"
#include "EffectRender\QcEffect.hpp"

ImplRuntimeClass2(QcSceneNode, 1000);

QcSceneNode::QcSceneNode()
: m_fWidth(0.f)
, m_fHeight(0.f)
, m_fDestWidth(0.f)
, m_matrix(gsIdentityMatrix)
, m_bMatrixDirty(true)
, m_pDesc(NULL)
, m_pEffectRender(NULL)
, m_eCurrentState(eReady)
, m_pParentNode(NULL)
, m_color(1.f, 1.f, 1.f, 1.f)
, m_pTexture(NULL)
, m_vPos(0.f, 0.f)
, m_fScale(1.f, 1.f)
, m_fRotate(0.f)
, m_bVisible(true)
, m_bIsLiving(true)
, m_fStartTime(0.f)
, m_bRenderJustSelf(false)
{
}

QcSceneNode::~QcSceneNode()
{
	
}

void QcSceneNode::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	m_pDesc = const_cast<QcResImageDesc*>(&desc);
	
	m_objType = desc.GetObjType();
	m_fWidth = desc.GetWidth();
	m_fHeight = desc.GetHeight();
	if (desc.GetEffectRender().size())
		m_pEffectRender = QmResMgr->MakeEffect(desc.GetEffectRender().c_str());

	SetScale(1.0f, eMaskXYZ);
	SetPosition(desc.GetPos());
	SetRotation(desc.GetRotation());
	SetState(eLiving, false);
	
	const QcList<QsStateParameter>& stateList = desc.GetStateParameterList();
	if (stateList.size())
	{
 		const QcTexturePtrList& textureList = stateList[0].m_pTextureList;
		SetCurrentTexture(textureList[0]);
 		if (textureList.size() > 1)
 		{
			m_arAnimatorList.reserve(4);
			AddAnimator(QmAnimatorCreator->MakeTextureAnimator(&stateList[0]));
 		}
	}
	if (bLoadSubNode)
	{
		QcResManager* pRes = QmResMgr;
		QcList<QcResImageDescPtr>& subDescList = m_pDesc->GetSubDescList();
		QcList<QcResImageDescPtr>::iterator iter(subDescList.begin());
		QcList<QcResImageDescPtr>::iterator endIter(subDescList.end());
		for (; iter != endIter; ++iter)
		{
			QcSceneNode* pSubObj = pRes->MakeGameInstance(*iter);
			QmAssert(pSubObj != NULL);
			AddSubSceneNode(pSubObj);
		}
	}
	UpdateFrameRegion(GetDesc()->GetRegion());
}

QiAnimator* QcSceneNode::MakeTextureAnimator(QeState eNowState)
{
	QiAnimator* pRet = NULL;
	const QsStateParameter* pStateParameter = NULL;
	if (m_pDesc && (pStateParameter = m_pDesc->GetStateParameter(eNowState)) != NULL)
	{
		pRet = QmAnimatorCreator->MakeTextureAnimator(pStateParameter);
	}
	return pRet;
}
void QcSceneNode::StateChange(QeState eRawState, QeState eNowState)
{
	int iPos = FindAnimator<QcAnimatorTexture>(0);
	const QsStateParameter* pStateParameter = NULL;
	if (m_pDesc && (pStateParameter = m_pDesc->GetStateParameter(eNowState)) != NULL)
	{
		if (iPos != -1)
			static_cast<QcAnimatorTexture*>(m_arAnimatorList[iPos].Get())->Reset(*pStateParameter);
		else
			AddAnimator(QmAnimatorCreator->MakeTextureAnimator(pStateParameter));
	}
	else
	{
		if (m_pParentNode == NULL)
			SetCurrentTexture(NULL);
		if (iPos != -1)
			m_arAnimatorList.EraseAt(iPos);
	}
	if (pStateParameter)
	{
		SetWidth(pStateParameter->m_fWidth);
		SetHeight(pStateParameter->m_fHeight);
	}
	if (m_arSubList.size())
	{
		QcSceneNodeListIter subiter(m_arSubList.begin());
		QcSceneNodeListIter endIter(m_arSubList.end());
		for (; subiter != endIter; ++subiter)
		{
			if ((*subiter).Get())
			{
				(*subiter)->StateChange(eRawState, eNowState);
			}
		}
	}
}

bool QcSceneNode::ToDie(bool bDieTimer) 
{
	RemoveMonitor(NULL);
	SetState(eDying);
	m_bIsLiving = false;

	//子对象不应该启动DyingTimer
	if (m_pParentNode == NULL && bDieTimer)
	{
		f32 fdyingTime = m_pDesc->GetDyingTime();
		if (fdyingTime > ROUNDING_ERROR_f32)
			SetTimer(eDyingTimer, fdyingTime, true);
		else
			ToDead();
	}
	return true;
}

void QcSceneNode::ToDead()
{
	RemoveMonitor(NULL);
	m_bIsLiving = false;
	QmSence->RecoveryNode(this);
}

void QcSceneNode::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eDyingTimer)
	{
		ToDead();
	}
}

void QcSceneNode::Animate(double dCurrentTime,f32 fElapsedTime)
{
	if (!IsReady())
	{
		m_fStartTime -= fElapsedTime;
		return;
	}
	
	{
		QcAnimatorPtrList::Tracker track(m_arAnimatorList);
		QcAnimatorPtrListIter iter(m_arAnimatorList.begin());
		QcAnimatorPtrListIter endIter(m_arAnimatorList.end());
		for (; iter != endIter; ++iter)
		{
			if ((*iter))
			{
				(*iter)->Animate(this, dCurrentTime, fElapsedTime);
			}
		}
	}
	if (m_pEffectRender)
		m_pEffectRender->OnAnimate(this, dCurrentTime, fElapsedTime);

	if (m_arSubList.size())
	{
		QcSceneNodePtrList::Tracker track(m_arSubList);
		QcSceneNodeListIter subiter(m_arSubList.begin());
		QcSceneNodeListIter endSubIter(m_arSubList.end());
		for (; subiter != endSubIter; ++subiter)
		{
			if ((*subiter).Get())
			{
				(*subiter)->Animate(dCurrentTime, fElapsedTime);
			}
		}
	}
}

void QcSceneNode::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (IsVisible() && IsReady())
	{
		QcMatrix worldMat(pWorldMatrix != NULL ? ((*pWorldMatrix) * GetMatrix()) : GetMatrix());

		if (m_pRenderState)
			m_pRenderState->BeforeRender(render);

		QmAutoRevertAlpha(render);
		render.SetMaterialColor(GetColor());
		if (GetCurrentTexture() && GetCurrentTransparency() > 0)
		{
			if (QcMath::equals(m_fDestWidth, 0.f) || QcMath::equals(m_fDestHeight, 0.f))
			{
				render.RenderTexture(GetCurrentTexture(), worldMat * GetCurrentTexture()->GetDispMat(), m_pEffectRender);
			}
			else
			{
				QcMatrix mat(QcMatrix::EM4CONST_NOTHING);
				mat.MakeMatrix(
					GetCurrentTexture()->GetOffsetX(), 
					0.f- GetCurrentTexture()->GetOffsetY(), 
					0.f, 
					m_fDestWidth, 
					m_fDestHeight);
				render.RenderTexture(GetCurrentTexture(), worldMat * mat, m_pEffectRender);
			}
		}

		if (m_pRenderState && m_pRenderState->GetRenderFlag() == QiRenderState::eOnlySelf)
			m_pRenderState->EndRender(render);
		if (!m_bRenderJustSelf)
			RenderSub(render, &worldMat);
		if (m_pRenderState && m_pRenderState->GetRenderFlag() == QiRenderState::eNormal)
			m_pRenderState->EndRender(render);
	}
}

void QcSceneNode::RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (m_arSubList.size())
	{
		QcSceneNodeListIter subiter(m_arSubList.begin());
		QcSceneNodeListIter subEndIter(m_arSubList.end());
		for (; subiter != subEndIter; ++subiter)
		{
			if (*subiter)
			{
				(*subiter)->Render(render, pWorldMatrix);
			}
		}
	}
}

void QcSceneNode::AddAnimator(QiAnimator* pAnimator)
{
	if (pAnimator)
	{
		pAnimator->AddToSceneNode(this);
		m_arAnimatorList.Insert(pAnimator);
	}
}
void QcSceneNode::AddAnimator2(QiAnimator* pAnimator)
{
	s32 iPos = m_arAnimatorList.Find(pAnimator);
	if (iPos == -1)
	{
		AddAnimator(pAnimator);
	}
}

void QcSceneNode::FinishAnimator(const QiAnimator* pAnimator)
{
	RemoveAnimator(pAnimator);
}
void QcSceneNode::RemoveAnimator(const QiAnimator* pAnimator)
{
	if (pAnimator == NULL)
		m_arAnimatorList.Clear();
	else
		m_arAnimatorList.Erase(pAnimator);
}


void QcSceneNode::AddMonitor(const QiSceneNodeMonitor* pMonitor)
{
	m_arMonitorList.Insert(const_cast<QiSceneNodeMonitor*>(pMonitor));
	const_cast<QiSceneNodeMonitor*>(pMonitor)->OnAddNodeMonitor(this);
}

void QcSceneNode::RemoveMonitor(const QiSceneNodeMonitor* pMonitor)
{
	if (pMonitor == NULL)
	{
		if (m_arMonitorList.size())
		{
			QcList<QiSceneNodeMonitor*>::iterator iter(m_arMonitorList.begin());
			QcList<QiSceneNodeMonitor*>::iterator endIter(m_arMonitorList.end());
			for (; iter != endIter; ++iter)
			{
				if (*iter)
				{
					(*iter)->OnRemoveNodeMonitor(this);
				}
			}
			m_arMonitorList.Clear();
		}
	}
	else
	{
		m_arMonitorList.Erase(pMonitor);
	}
}

void QcSceneNode::AddSubSceneNode(const QcSceneNode* pSceneNode)
{
	pSceneNode->m_pParentNode = this;
	m_arSubList.PushBack(pSceneNode);
}
void QcSceneNode::RemoveSubSceneNode(const QcSceneNode* pSceneNode)
{
	if (pSceneNode == NULL)
	{
		QcSceneNodeListIter iter(m_arSubList.begin());
		QcSceneNodeListIter endIter(m_arSubList.end());
		for (; iter != endIter; ++iter)
		{
			(*iter)->m_pParentNode = NULL;
		}
		m_arSubList.Clear();
	}
	else
	{
		pSceneNode->m_pParentNode = NULL;
		m_arSubList.Erase(pSceneNode);
	}
}
void QcSceneNode::RemoveSubSceneNodeAt(int index)
{
	if (index >=0 && index < (int)m_arSubList.size() && m_arSubList[index])
	{
		m_arSubList[index]->m_pParentNode = NULL;
		m_arSubList[index] = NULL;
	}
}
QcSceneNode* QcSceneNode::GetSubSceneNode(int index)
{
	if (index >=0 && index < (int)m_arSubList.size())
		return m_arSubList[index];
	return NULL;
}
QcSceneNode* QcSceneNode::GetSubSceneNode(const QcObjType& name)
{
	QcSceneNodeListIter iter(m_arSubList.begin());
	QcSceneNodeListIter endIter(m_arSubList.end());
	for (; iter != endIter; ++iter)
	{
		if (*iter && (*iter)->GetObjType() == name)
		{
			return *iter;
		}
	}
	return NULL;
}

void QcSceneNode::UpdateRegion(QeMathNodeInfoMask eMask)
{
	m_bMatrixDirty = true;
	m_iIntersectScreenFlag = -1;

	QcRegion* pRegion = m_pRegion.Get();
	if (pRegion)
	{
		pRegion->SetDirty(true);
	}

	if (eMask & eMaskScale)
	{
		m_fHalfMaxLen = QcMath::max_(m_fWidth * m_fScale.X * 0.5f, m_fHeight * m_fScale.Y * 0.5f);
	}
}

void QcSceneNode::CalIntersectScreenFlag()
{
	m_iIntersectScreenFlag = 0;

	QcRect rc(QmGlobalData.gScreenRect);
	rc.Expand(QcVector(m_fHalfMaxLen, m_fHalfMaxLen));
	if (rc.isPointInside(GetPosition()))
	{
#if 1
		m_iIntersectScreenFlag = 1;
#else
		QcPoint points[4];
		points[0].X = m_fWidth * 0.5f;
		points[0].Y = m_fHeight * 0.5f;

		points[1].X = m_fWidth * 0.5f;
		points[1].Y = -m_fHeight * 0.5f;

		points[2].X = -points[0].X;
		points[2].Y = -points[0].Y;

		points[3].X = -points[1].X;
		points[3].Y = -points[1].Y;

		f32 fCs = cos(GetRotation());
		f32 fSn = sin(GetRotation());

		for (int i = 0; i< 4; i++)
		{
			points[i].X *= GetScaleX();
			points[i].Y *= GetScaleY();
			points[i].rotateBy(fCs,fSn);
			points[i] += GetPosition();

			if (QmGlobalData.gScreenRect.isPointInside(points[i]))
			{
				m_iIntersectScreenFlag = 1;
				break;
			}
		}
#endif
	}
}

void QcSceneNode::UpdateFrameRegion(const QcRegion* pRegion)
{
	if (m_pSourceRegion != pRegion)
	{
		m_pRegion = NULL;
		m_pSourceRegion = pRegion;
		if (NULL == m_pSourceRegion && GetDesc()->GetRegion())
			m_pSourceRegion = GetDesc()->GetRegion();

		if (m_pSourceRegion)
			m_pRegion = m_pSourceRegion->Clone();
	}
}

bool QcSceneNode::Intersect(QcSceneNode* pSceneNode)
{
	QcVector vec(pSceneNode->GetPosition(), GetPosition());
	if (vec.smaller(pSceneNode->m_fHalfMaxLen + m_fHalfMaxLen))
	{
		const QcRegionAutoPtr& pSourceRegion = GetRegion();
		const QcRegionAutoPtr& pDestRegion = pSceneNode->GetRegion();
		if (pSourceRegion && pDestRegion)
		{	
  			if (pDestRegion->IsDirty())
			{
				pDestRegion->Update(pSceneNode->GetPosition(), pSceneNode->GetRotation(),pSceneNode->m_fScale.X, pSceneNode->m_fScale.Y, pSceneNode->m_pSourceRegion);
			}
			if (pSourceRegion->IsDirty())
			{
				pSourceRegion->Update(GetPosition(), GetRotation(),m_fScale.X, m_fScale.Y, m_pSourceRegion);
			}
			return pSourceRegion->Intersect(pDestRegion.Get());
		}
	}
	return false;
}

bool QcSceneNode::Intersect(const QcRegion* pRegion)
{
	if (GetRegion()->IsDirty())
		GetRegion()->Update(GetPosition(), GetRotation(),m_fScale.X, m_fScale.Y, m_pSourceRegion);

	return GetRegion()->Intersect((QcRegion*)pRegion);
}