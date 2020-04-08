#include "pch.hpp"
#include "QcDigitalNode.hpp"
#include "Res\QcResDigitalDesc.hpp"
#include "Res\QcTexture.hpp"

ImplRuntimeClass(QcDigitalNode);

QcDigitalNode::QcDigitalNode()
: m_iFlashBit(-1)
, m_fTimeShow(0.f)
, m_fTimeHide(0.f)
, m_bShowNode(true)
, m_eAlign(eAlignCenter)
, m_eRadix(eRadix10)
, m_bBuf(false)
, m_iValue(0)
, m_nFillChars(0)
, m_bPosDirty(true)
, m_ffGapWidth(0.f)
{
}

void QcDigitalNode::Init(const QcResImageDesc& desc, bool bLoadSubNode)
{
	QcSceneNode::Init(desc, bLoadSubNode);
	SetDigitalType(GetObjType());
}

void QcDigitalNode::SetValue(BYTE* pBuf, u32 nSize) 
{ 
	QmAssert(nSize < QmMaxDigital);
	memcpy_s(m_sBuf, QmMaxDigital, pBuf, nSize);
	m_nFillChars = nSize;
	m_bBuf = true;
	m_bPosDirty = true;
}

void QcDigitalNode::SetFlashBit(s32 iBit)
{ 
	if (m_iFlashBit != -1 && iBit == -1)
	{
		KillTimer(eFlashTimer);
	}
	m_iFlashBit = iBit;
	if (iBit != -1)
	{
		SetTimer(eFlashTimer, m_fTimeHide);
		m_bShowNode = true;
	}
}

void QcDigitalNode::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eFlashTimer)
	{
		m_bShowNode = !m_bShowNode;
		if (m_bShowNode)
			SetTimer(eFlashTimer, m_fTimeShow);
		else
			SetTimer(eFlashTimer, m_fTimeHide);
	}
	else
	{
		QcSceneNode::OnTimer(nTimer, pContext);
	}
}

void QcDigitalNode::SetFlashTime(f32 fShowTime, f32 fHideTime)
{
	m_fTimeShow = fShowTime;
	m_fTimeHide = fHideTime;
}

void QcDigitalNode::RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	const QcResImageDesc* pDigitalDesc = GetDesc();
	if (pDigitalDesc)
	{
		if (m_pRenderState)
			m_pRenderState->BeforeRender(render);
		int iHideBit = m_bShowNode ? -1 : m_iFlashBit;
		QcMatrix subMat(QcMatrix::EM4CONST_NOTHING);
		const QcPointList& posList = GetDispPosList();
		int nCount = posList.size();
		for (int i=0; i<nCount; ++i)
		{
			if (iHideBit != i && m_arTextureList[i])
			{
				subMat.MakeMatrix(
					posList[i].X + m_arTextureList[i]->GetOffsetX(), 
					posList[i].Y - m_arTextureList[i]->GetOffsetY(),
					0.f,
					m_arTextureList[i]->GetWidth(), 
					m_arTextureList[i]->GetHeight());

				render.RenderTexture(m_arTextureList[i], (*pWorldMatrix) * subMat);
			}
		}
		if (m_pUnitNode)
		{
			m_pUnitNode->Render(render, pWorldMatrix);
		}
		if (m_pRenderState)
			m_pRenderState->EndRender(render);
	}
}

void QcDigitalNode::UpdateSize()
{
	if (m_bPosDirty)
	{
		m_arPosList.clear();
		m_arTextureList.clear();

		QcResDigitalDesc* pDigitalDesc = const_cast<QcResImageDesc*>(GetDesc())->As<QcResDigitalDesc*>();
		if (pDigitalDesc)
		{
			char nBuf[QmMaxDigital] = {0};
			int nLen = 0;
			int nAddZero = 0;
			if (m_bBuf)
			{
				memcpy_s(nBuf, QmMaxDigital, m_sBuf, QmMaxDigital);
				nLen = m_nFillChars;
			}
			else
			{
				_i64toa_s(m_iValue, nBuf, sizeof(nBuf), m_eRadix);
				nLen = strlen(nBuf);
				if (m_nFillChars > 0)
					nAddZero = m_nFillChars - nLen;
			}

			int nCount = nLen + nAddZero;
			f32 fGap = pDigitalDesc->GetWidth() + m_ffGapWidth;
			SetWidth(fGap * nCount);
			SetHeight(pDigitalDesc->GetHeight());

			QcPoint offsetPt;
			switch(m_eAlign)
			{
			case eAlignLeftTop:
				{
					offsetPt.X = fGap * 0.5f;
					offsetPt.Y = -pDigitalDesc->GetHeight() * 0.5f; 
					break;
				}
			case eAlignLeftCenter:
				{
					offsetPt.X = fGap * 0.5f;
					break;
				}
			case eAlignLeftButtom:
				{
					offsetPt.X = fGap * 0.5f;
					offsetPt.Y = pDigitalDesc->GetHeight() * 0.5f; 
					break;
				}
			case eAlignCenterTop:
				{
					offsetPt.X = -(nCount - 1) * fGap * 0.5f;
					offsetPt.Y = -pDigitalDesc->GetHeight() * 0.5f; 
					break;
				}
			case eAlignCenter:
				{
					offsetPt.X = -(nCount - 1) * fGap * 0.5f;
					break;
				}
			case eAlignCenterBottom:
				{
					offsetPt.X = -(nCount - 1) * fGap * 0.5f;
					offsetPt.Y = pDigitalDesc->GetHeight() * 0.5f; 
					break;
				}
			case eAlignRightTop:
				{
					offsetPt.X = -nCount * fGap + 0.5f * fGap;
					offsetPt.Y = -pDigitalDesc->GetHeight() * 0.5f; 
					break;
				}
			case eAlignRightCenter:
				{
					offsetPt.X = -nCount * fGap + 0.5f * fGap;
					break;
				}
			case eAlignRightBottom:
				{
					offsetPt.X = -nCount * fGap + 0.5f * fGap;
					offsetPt.Y = pDigitalDesc->GetHeight() * 0.5f; 
					break;
				}
			}
			QcPoint pt =  offsetPt;
			if (m_pUnitNode)
			{
				pt.X -= m_pUnitNode->GetWidth() * 0.5f;
			}
			const QcTexture* texture = pDigitalDesc->GetTexture(0);
			for (int i=0; i<nAddZero; ++i)
			{
				m_arPosList.push_back(pt);
				m_arTextureList.PushBack(texture);
				pt.X += fGap;
			}
			for (int i=0; i<nLen; ++i)
			{
				m_arPosList.push_back(pt);
				m_arTextureList.PushBack(pDigitalDesc->GetTexture(nBuf[i]));
				pt.X += fGap;
			}
			if (m_pUnitNode)
			{
				pt.X += (m_pUnitNode->GetWidth() - fGap) * 0.5f;
                m_pUnitNode->SetPosition(pt);
			}
		}
		m_bPosDirty = false;
	}
}

QcPointList& QcDigitalNode::GetDispPosList()
{
	UpdateSize();
	return m_arPosList;
}

void QcDigitalNode::SetDigitalType(const QcObjType& objType)
{
	const QcResImageDesc* pImageDesc = QmResMgr->GetImageDesc(objType);
	if (pImageDesc && pImageDesc->Is<const QcResDigitalDesc*>())
	{
		SetDigitalType(static_cast<const QcResDigitalDesc*>(pImageDesc));
	}
}

void QcDigitalNode::SetDigitalType(const QcResDigitalDesc* pDigitalDesc)
{
	m_pDesc = const_cast<QcResDigitalDesc*>(pDigitalDesc);
	m_objType = pDigitalDesc->GetObjType();
	m_bPosDirty = true;
}

void QcDigitalNode::SetUnitType(const QcObjType& objType)
{
	m_pUnitNode = NULL;
	const QcResImageDesc* pImageDesc = QmResMgr->GetImageDesc(objType);
	if (pImageDesc)
	{
		m_pUnitNode = const_cast<QcResImageDesc*>(pImageDesc)->MakeInstance();
		m_bPosDirty = true;
	}
}