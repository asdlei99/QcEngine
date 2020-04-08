#pragma once

#include "RenderSystem\QiRenderState.hpp"
#include "RenderSystem\QcRenderStateHelper.hpp"

const QsRenderState gNetRenderState[] = { {D3DRS_DESTBLEND, D3DBLEND_ONE, 0} };
const QsTexState gNumTexState[] = { {0, D3DTSS_COLOROP, D3DTOP_MODULATE2X, 0} };

#define QmMakeRenderState(state) new QcRenderStateHelper(state, QmCountOf(state))
#define QmMakeTexState(state) QmMakeRenderState(state)
#define QmMakeTexRenderState(renderState, texState) new QcRenderStateHelper(renderState, QmCountOf(renderState), texState, QmCountOf(texState))

class QcRenderStateHelper : public QiRenderState
{
public:
	QcRenderStateHelper(const QsRenderState* pRenderStates, u32 nRenderState)
		: m_arRenderState(pRenderStates, nRenderState)
		, m_arTexState(NULL, 0)
	{
	}
	QcRenderStateHelper(const QsTexState* pTexStates, u32 nTexState)
		: m_arRenderState(NULL, 0)
		, m_arTexState(pTexStates, nTexState)
	{
	}
	QcRenderStateHelper(const QsRenderState* pRenderStates, u32 nRenderState, const QsTexState* pTexStates, u32 nTexState)
		: m_arRenderState(pRenderStates, nRenderState)
		, m_arTexState(pTexStates, nTexState)
	{
	}
	virtual void BeforeRender(QcDeviceRender& render)
	{
		int nSize = m_arRenderState.size();
		for (int i=0; i<nSize; ++i)
		{
			m_arRenderState[i].iBackupValue = render.GetRenderState(m_arRenderState[i].type);
			if (m_arRenderState[i].iBackupValue != m_arRenderState[i].value)
				render.SetRenderState(m_arRenderState[i].type, m_arRenderState[i].value);
		}
		nSize = m_arTexState.size();
		for (int i=0; i<nSize; ++i)
		{
			m_arTexState[i].iBackupValue = render.GetTextureStageState(m_arTexState[i].iTex, m_arTexState[i].type);
			if (m_arTexState[i].iBackupValue != m_arTexState[i].value)
				render.SetTextureStageState(m_arTexState[i].iTex, m_arTexState[i].type, m_arTexState[i].value);
		}
	}
	virtual void EndRender(QcDeviceRender& render)
	{
		int nSize = m_arRenderState.size();
		for (int i=0; i<nSize; ++i)
		{
			u32 iTemp = render.GetRenderState(m_arRenderState[i].type);
			if (m_arRenderState[i].iBackupValue != iTemp)
				render.SetRenderState(m_arRenderState[i].type, m_arRenderState[i].iBackupValue);
		}
		nSize = m_arTexState.size();
		for (int i=0; i<nSize; ++i)
		{
			u32 iTemp = render.GetTextureStageState(m_arTexState[i].iTex, m_arTexState[i].type);
			if (m_arTexState[i].iBackupValue != iTemp)
				render.SetTextureStageState(m_arTexState[i].iTex, m_arTexState[i].type, m_arTexState[i].iBackupValue);
		}
	}
protected:
	QcArray<QsRenderState, 4> m_arRenderState;
	QcArray<QsTexState, 4> m_arTexState;
};