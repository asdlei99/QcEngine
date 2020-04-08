#include "pch.hpp"
#include "QcGoldColumnMgr.hpp"
#include "QcGoldColumn.hpp"
#include "Animator\QiAnimator.hpp"
#include "Res\QcResImageDesc.hpp"

QcGoldColumnMgr::QcGoldColumnMgr()
{
}

void QcGoldColumnMgr::Init(QcPlayer* pPlayer, const QcObjType& goldColumn)
{
	m_pColumnDesc = QmResMgr->GetImageDesc(goldColumn);

	m_vFirstColumnPos = QcPoint(-120.f, 0.f);

	m_vOffset = QcPoint(-(m_pColumnDesc->GetWidth() - 0.f), 0.f);
	
	m_bWinMode = QmSysSetting->GetGameMode() == eOutWinCoin || QmSysSetting->GetGameMode() == eOutWinLottery;

	m_pPlayer = pPlayer;

	m_iLastScore = 0;
}

void QcGoldColumnMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcGoldColumnPtrDeque::iterator iter(m_goldColumnDeque.begin());
	QcGoldColumnPtrDeque::iterator endIter(m_goldColumnDeque.end());
	for (int i = 0; iter != endIter; ++iter, ++i)
	{
		(*iter)->Animate(dCurrentTime, fElapsedTime, i < 4);
	}
}

void QcGoldColumnMgr::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (m_goldColumnDeque.size())
	{
		QcGoldColumnPtrDeque::iterator iter(m_goldColumnDeque.begin());
		QcGoldColumnPtrDeque::iterator endIter(m_goldColumnDeque.end());
		
		for (int i = 0; (iter != endIter) && (i < 4); ++iter, ++i)
		{
			(*iter)->Render(render, pWorldMatrix);
		}
		if (!IsWinMode())
		{
			QcGoldColumn* pGoldColumn = m_goldColumnDeque.front();
			if (pGoldColumn->IsFinish())
			{
				if (NULL == m_pAlphaAnimator && (NULL == m_pMoveAnimator || m_pMoveAnimator->IsFinished()))
				{	
					m_pMoveAnimator = NULL;
					f32 Alphatime = 0.3f;
					if (m_goldColumnDeque.size() == 1)
					{
						Alphatime = 0.8f;
					}
					m_pAlphaAnimator = QmAnimatorCreator->MakeAlphaAnimator2(1.0, 0.1f,Alphatime/* 0.8f*/);                      
                    m_pAlphaAnimator->SetStartTime(m_goldColumnDeque.size() > 4 ? 0.3f : 1.f);
					pGoldColumn->AddAnimator(m_pAlphaAnimator);
				}
				else if (m_pAlphaAnimator && m_pAlphaAnimator->IsFinished())
				{
					m_pAlphaAnimator.Set(NULL);
                    if (m_goldColumnDeque.size() > 4)
                    {
                        m_goldColumnDeque[4]->SetTimer(ADD_CHIP_TIMER, 0.03f);
                    }
					m_goldColumnDeque.pop_front();
					if (m_goldColumnDeque.size())
						MakeMoveAnimator(0.2f, abs(m_vOffset.X));
				}
			}
		}
	}
}

void QcGoldColumnMgr::MakeMoveAnimator(f32 fTime, f32 fLen)
{
	f32 speed = 250.f;
	f32 lastspeed = 150.f;
	if (m_goldColumnDeque.size() > 4)
	{
		speed = 650.f;
		lastspeed = 650.f;
	}
	//m_pMoveAnimator = QmAnimatorCreator->MakeLineAnimator(QcPoint(), QcPoint(fLen, 0.f), fTime);
	m_pMoveAnimator = QmAnimatorCreator->MakeLineAnimator2(QcPoint(), QcPoint(fLen, 0.f), speed);
	f32 a = (speed - lastspeed) / (fTime * 0.8f);
	QcAnimatorSpeedPtr pSpeedMoveToPlayer(new QcGravitySpeed(-a, fTime * 0.8f));
	m_pMoveAnimator->AddSpeedAnimator(pSpeedMoveToPlayer);

	QcGoldColumnPtrDeque::iterator iter(m_goldColumnDeque.begin());
	QcGoldColumnPtrDeque::iterator endIter(m_goldColumnDeque.end());
	for (; iter != endIter; ++iter)
	{
		(*iter)->AddAnimator(m_pMoveAnimator);
	}
}

void QcGoldColumnMgr::UpdateWinScore(u32 iScore, u32 gunLevel)
{
	if (IsWinMode())
	{
		bool bUpdate = m_iLastScore != (int)iScore;
		if (bUpdate)
			m_iLastScore = iScore;

		QcGoldColumn* pGoldColumn = NULL;
		if (m_goldColumnDeque.empty())
		{
			pGoldColumn = m_pColumnDesc->MakeInstance<QcGoldColumn>();
			pGoldColumn->SetPosition(m_vFirstColumnPos);
			m_goldColumnDeque.push_back(QcSharedPtr<QcGoldColumn>(pGoldColumn));
			bUpdate = true;
		}
		else
		{
			pGoldColumn = m_goldColumnDeque.front();
		}
		if (bUpdate)
        {
			pGoldColumn->ShowColumn(iScore, m_vFirstColumnPos, false);
            pGoldColumn->SetTimer(ADD_CHIP_TIMER, 0.03f);
        }
	}
}


void QcGoldColumnMgr::MakeColumn(u32 iScore)
{
	if (!IsWinMode())
	{
		QcGoldColumn* pGoldColumn = m_pColumnDesc->MakeInstance<QcGoldColumn>();
		if (m_goldColumnDeque.size())
		{
			pGoldColumn->SetPosition(m_goldColumnDeque.back()->GetPosition() + m_vOffset);
			if (m_pMoveAnimator && !m_pMoveAnimator->IsFinished())
				pGoldColumn->AddAnimator(m_pMoveAnimator);

		}
		else
		{
			pGoldColumn->SetPosition(m_vFirstColumnPos);
		}
		pGoldColumn->ShowColumn(iScore, m_vFirstColumnPos, false);

		m_goldColumnDeque.push_back(QcSharedPtr<QcGoldColumn>(pGoldColumn));
        //QmSoundMgr->Play(QmSoundType(ShowCoinColumnSound));

        if (m_goldColumnDeque.size() < 5)
            pGoldColumn->SetTimer(ADD_CHIP_TIMER, 0.03f);
	}
}