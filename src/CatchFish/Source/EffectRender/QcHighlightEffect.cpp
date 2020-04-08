#include "Pch.hpp"
#include "QcHighlightEffect.hpp"

ImplRuntimeClass(QcHighlightEffect);

void QcHighlightEffect::OnAnimate(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
{
	if (IsAcitve() && m_nPeriod > 0)
	{
		f32 fGap = m_fUnit * fElapsedTime;

		if (m_iFlag != -1 && m_fLuminance + fGap> m_fMaxLuminance)
		{
			m_iFlag = 1;
			m_fLuminance = m_fMinLuminance;
			m_nPeriod--;
			if (m_nPeriod == 0)
			{
				m_fLuminance = 0.f;
				m_iFlag = 0;
			}
		}
		else if (m_iFlag != 1 && m_fLuminance - fGap < m_fMinLuminance)
		{
			m_fLuminance = m_fMinLuminance;
			m_iFlag = 1;
			m_nPeriod--;
			if (m_nPeriod == 0)
			{
				m_fLuminance = 0.f;
				m_iFlag = 0;
			}
		}
		else
		{
			m_fLuminance += (m_iFlag == 1) ? fGap : -fGap;
		}
	}
}

bool QcHighlightEffect::OnRender(QcDeviceRender& render, const QcTexture* pTexture)
{
	if (IsAcitve() && m_nPeriod > 0)
	{
		m_pEffect->SetFloat("gfLuminance", m_fLuminance);
		return QcEffect::OnRender(render, pTexture);
	}
	return false;
}

void QcHighlightEffect::FlashLuminance(f32 fMinLuminance, f32 fMaxLuminance, f32 fPeriodTime, u32 nPeriod)
{
	if (fMinLuminance > fMaxLuminance)
	{
		m_fMaxLuminance = fMinLuminance;
		m_fMinLuminance = fMaxLuminance;
	}
	else
	{
		m_fMinLuminance = fMinLuminance;
		m_fMaxLuminance = fMaxLuminance;
	}

	m_fUnit = (m_fMaxLuminance - m_fMinLuminance)/(fPeriodTime*0.5f);
	m_fLuminance = 0.f;

	if (nPeriod == 0)
		m_nPeriod = u32(-1);
	else if (m_fLuminance < fMinLuminance)
		m_nPeriod = nPeriod + 1;
}

void QcHighlightEffect::OnHit()
{
	FlashLuminance(0.1f, 1.0f, 0.1f, 3);
}