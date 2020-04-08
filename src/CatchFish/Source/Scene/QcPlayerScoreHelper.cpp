#include "pch.hpp"
#include "QcPlayerScoreHelper.hpp"
#include "QcPlayer.hpp"
#include "Animator\QcAnimatorFlash.hpp"
#include "FileStorageSystem\QcPlayerInfo.hpp"

QcPlayerScoreHelper::QcPlayerScoreHelper()
: m_iBaoJiScore(0)
{

}

void QcPlayerScoreHelper::OnTimer(u32 nTimer, void* pContext)
{
	if (nTimer == eCoinErrorTimer)
	{
		KillTimer(eCoinErrorTimer);
		m_bError = false;
		if (IsWinMode())
		{
			if (m_iNeedExitLottery > 0)
			{
				ToExitCoin(m_iNeedExitLottery);
			}
			else
			{
				ToExitCoin();
			}
		}
	}
}

void QcPlayerScoreHelper::Init(QcPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	m_pSystemSetting = QmSysSetting;

	m_bWinMode = m_pSystemSetting->GetGameMode() == eOutWinCoin || m_pSystemSetting->GetGameMode() == eOutWinLottery;
	m_bCoinMode = m_pSystemSetting->GetGameMode() ==  eOutWinCoin || m_pSystemSetting->GetGameMode() == eOutCoin;
	m_bError = false;
	m_bOutCoinKeyDown = false;
	m_iExitCountBuf = 0;
	m_fLastOutCoinTime = 0.f;
	m_iNeedExitLottery = 0;
	m_iLockFireScore = 0;
	m_iBaoJiScore = m_pSystemSetting->GetBaoJiScore();

	if (IsCoinMode())
	{
		m_pExiting = QmResMgr->MakeGameInstance(QmObjType(ExitCoin));
		m_pExitError = QmResMgr->MakeGameInstance(QmObjType(ExitCoinError));
	}
	else
	{
		m_pExiting = QmResMgr->MakeGameInstance(QmObjType(ExitLottery));
		m_pExitError = QmResMgr->MakeGameInstance(QmObjType(ExitLotteryError));
	}
	QcAnimatorFlash* pFlashAnimator = static_cast<QcAnimatorFlash*>(QmAnimatorCreator->MakeFlashAnimator(0.3f, 0.3f));
	m_pExitError->AddAnimator(pFlashAnimator);
	m_pExiting->AddAnimator(pFlashAnimator);
	m_pExitError->SetPosition(GetSysSetting()->GetRalativeCoinExiterPt());
	m_pExiting->SetPosition(GetSysSetting()->GetRalativeCoinExiterPt());

	m_pPlayerInfo = m_pPlayer->GetInfo();

	//清理加密卡数据
	if (!IsWinMode())
	{
		m_pPlayerInfo->AddScore(m_pPlayerInfo->GetWinModeWinScore());
		m_pPlayerInfo->SetWinModeWinScore(0);
	}
	else
	{
		ToExitCoin();
	}
}

bool QcPlayerScoreHelper::AddScore(s32 nCount, bool bCoin)
{
	int iAdded = nCount * QmSysSetting->GetFireQtyOneCoin();
	if (m_iBaoJiScore >= iAdded + GetInfo()->GetCurScore())
	{
		if (bCoin)
			GetInfo()->AddInCoin(nCount);
		else
			GetInfo()->AddInScore(nCount);

		GetInfo()->AddScore(iAdded);
		return true;
	}
	return false;
}

bool QcPlayerScoreHelper::OnKeyDown(QeKey key, const QsKeyInfo& info)
{
	switch (key)
	{
	case ePlayerKeyAddCoin:
		{
			if (AddScore(info.nKeyDownCount, true))
			{
				//QmSoundMgr->Play(QmSoundType(InCoinSound));
			}
			break;
		}
	case ePlayerKeyAddScore:
		{
			if (info.fKeyDownTime > 1.f) //长按
			{
				if (AddScore(GetSysSetting()->GetCoinCountMultiAddScorceKey(), false))
				{
					//QmSoundMgr->Play(QmSoundType(InScore));
					QmPlaySound(InScore);
					info.bIngoreUp = true;
				}
				info.bOnlyOneDown = true;
				
			}
			break;
		}
	case ePlayerKeyDownScore:
		{
			if (info.fKeyDownTime > 1.5f)
			{
				DownAllScore();
				//目前下分时播上分的声音
				//QmSoundMgr->Play(QmSoundType(OutScore));
				QmPlaySound(OutScore);
				info.bOnlyOneDown = true;
			}
			break;
		}
	case ePlayerKeyDownCoin:
		{
			if (!m_bOutCoinKeyDown && !IsError())
			{
				m_bOutCoinKeyDown = true;
				if (m_iNeedExitLottery > 0)
				{
					ToExitCoin(m_iNeedExitLottery);
				}
				else
				{
					m_bOutCoinKeyDown = ToExitCoin();
				}
				m_fLastOutCoinTime = 0.f;
				//QmSoundMgr->Play(QmSoundType(DownCoinSound));
			}
			break;
		}
	case ePlayerKeyCoinOut:
		{
			ExitedCoin(info.nKeyDownCount);
			m_fLastOutCoinTime = 0.f;
			break;
		}
	default:
		return false;
	}
	return true;
}

bool QcPlayerScoreHelper::OnKeyUp(QeKey key, const QsKeyInfo& info)
{
	switch (key)
	{
	case ePlayerKeyAddScore:
		{
			//if (info.fKeyDownTime <= 1.5f)
			{
				if (AddScore(GetSysSetting()->GetCoinCountPerAddScorceKey(), false))
				{
					QmPlaySound(InScore);
				}
			}
			break;
		}
	case ePlayerKeyDownScore:
		{
			if (info.fKeyDownTime < 1.5f)
			{
				DownScore(GetSysSetting()->GetCoinCountPerAddScorceKey());
				//目前下分时播上分的声音
				QmPlaySound(OutScore);
			}
			break;
		}
	default:
		return false;
	}
	return true;
}
void QcPlayerScoreHelper::DownScore(s32 iCoin)
{
	int iScoreOneCoin = GetSysSetting()->GetFireQtyOneCoin();
	int iScore = GetInfo()->GetCurScore();
	int iWinScore = 0;
	int iDownScore = iCoin * iScoreOneCoin;
	if (IsWinMode())
	{
		iWinScore = GetInfo()->GetWinModeWinScore();
		iScore += iWinScore;
	}
	iScore -= GetLockSendScore();

	if (iScore >= iDownScore)
	{
		GetInfo()->AddOutScore(iCoin);

		if (iDownScore <= iWinScore)
			GetInfo()->AddWinModeWinScore(-iDownScore);
		else
		{
			GetInfo()->SetWinModeWinScore(0);
			GetInfo()->AddScore(iWinScore - iDownScore);
		}
	}
}

void QcPlayerScoreHelper::DownAllScore()
{
	int iScoreOneCoin = GetSysSetting()->GetFireQtyOneCoin();
	int iScore = GetInfo()->GetCurScore();
	if (IsWinMode())
	{	
		iScore += GetInfo()->GetWinModeWinScore();
	}
	iScore -= GetLockSendScore();
	int iCoin = iScore/iScoreOneCoin;
	if (iCoin > 0)
	{
		DownScore(iCoin);
	}
}
void QcPlayerScoreHelper::SetError()
{
#ifdef QmIOUsbOn
	ToExitCoin(0);
#endif
#ifdef QmTestVersion
	QmDebugHelper.AddFailExitCoin(GetPlayer()->GetID(), m_iExitCountBuf);
#endif
	m_bError = true;
	m_bOutCoinKeyDown = false;
	m_iLockFireScore = 0;
	m_iExitCountBuf = 0;
	m_fLastOutCoinTime = 0.f;
	SetTimer(eCoinErrorTimer, 5.f, true);
}

void QcPlayerScoreHelper::Finish()
{
	m_bError = false;
	m_bOutCoinKeyDown = false;
	m_iLockFireScore = 0;
	m_fLastOutCoinTime = 0.f;
}

void QcPlayerScoreHelper::HitAddScore(u32 iScore)
{
	GetInfo()->AddCatchScore(iScore);
	if (IsWinMode())
	{
		GetInfo()->AddWinModeWinScore(iScore);
		//没有按下退币键
		if (!IsExiting() && !IsError())
		{
			if (m_iNeedExitLottery > 0)
			{
				ToExitCoin(m_iNeedExitLottery);
			}
			else
			{
				ToExitCoin();
			}
		}
	}
	else
	{
		GetInfo()->AddScore(iScore);
	}
}

bool QcPlayerScoreHelper::ToExitCoin()
{
	int iScore = GetInfo()->GetCurScore();
	int iWinScore = 0;
	if (IsWinMode())
	{
		iWinScore = GetInfo()->GetWinModeWinScore();
		if (m_bOutCoinKeyDown)
			iScore += iWinScore;	//金币袋及分数框的分数一起退
		else 
			iScore = iWinScore;		//只退金币袋的分	
	}
	//减去已经发出去，尚未退出去的分
	iScore -= GetLockSendScore();	
	if (iScore <= 0)
		return false;

	//计算这次退的币或者票数
	s32 iExitNumber = 0;
	int iScoreOneCoin = QmSysSetting->GetFireQtyOneCoin();
	if (IsCoinMode())
	{
		iExitNumber = QcMath::min_(iScore/iScoreOneCoin, (s32)GetSysSetting()->GetExitCoinNumOnce());
	}
	else
	{
		int nCoinRatio = 0;
		int nLotteryRatio = 0;
		GetSysSetting()->GetCaiPiaoBiLi(nCoinRatio,  nLotteryRatio);

		//nCoinRatio 的倍数，最大20倍
		int iTime = QcMath::min_(iScore/(nCoinRatio * iScoreOneCoin), (s32)GetSysSetting()->GetExitCoinNumOnce());
		iExitNumber = iTime * nLotteryRatio;
	}
	if (iExitNumber)
	{
		ToExitCoin(iExitNumber);
		if (IsCoinMode())
			m_iLockFireScore = QcMath::max_(0, (m_iExitCountBuf + iExitNumber) * iScoreOneCoin -  iWinScore);
		else
			ExitedLotteryCoinRatio(1);	//先减一倍的分数
	}
	return iExitNumber > 0;
}

s32 QcPlayerScoreHelper::GetLockSendScore() const
{
	if (IsCoinMode())
	{
		return m_iExitCountBuf * QmSysSetting->GetFireQtyOneCoin();
	}
	else if (m_iExitCountBuf > m_iNeedExitLottery)
	{
		int nCoinRatio = 0;
		int nLotteryRatio = 0;
		GetSysSetting()->GetCaiPiaoBiLi(nCoinRatio,  nLotteryRatio);

		return ((m_iExitCountBuf - m_iNeedExitLottery)/nLotteryRatio) * (QmSysSetting->GetFireQtyOneCoin() * nCoinRatio);
	}
	return 0;
}

void QcPlayerScoreHelper::ToExitCoin(s32 iExitNumber)
{
#ifdef QmIOUsbOn
	if (iExitNumber < 0)
	{
		QmLogError("iExitNumber < 0");
	}
	if (IsCoinMode())
		ExitCoin((BYTE)GetPlayer()->GetID(), iExitNumber, false);
	else
		ExitCoin((BYTE)GetPlayer()->GetID(), iExitNumber, true);
#endif
	m_iExitCountBuf += iExitNumber;
}

void QcPlayerScoreHelper::ExitedCoin(s32 iExitNumber)
{
	QmAssert(iExitNumber >= 0);
	m_iExitCountBuf -= iExitNumber;
	if (m_iExitCountBuf < 0)
	{
		QmLog("m_iExitCountBuf=%d, iExitNumber=%d", m_iExitCountBuf, iExitNumber);
		m_iExitCountBuf = 0;
#ifdef QmTestVersion
		if (!m_bError)
			SetError();
#else
		QmDump("Error");
#endif
		return;
	}

	if (IsCoinMode())
	{
		GetInfo()->AddOutCoin(iExitNumber);

		int iScoreOneCoin = QmSysSetting->GetFireQtyOneCoin();
		int iNeedSubScore = iExitNumber * iScoreOneCoin;
		if (IsWinMode())
		{
			int iWinModeWinScore = GetInfo()->GetWinModeWinScore();
			if (iWinModeWinScore >= iNeedSubScore)
			{
				GetInfo()->AddWinModeWinScore(-iNeedSubScore);
			}
			else
			{
				GetInfo()->SetWinModeWinScore(0);
				GetInfo()->AddScore(iWinModeWinScore - iNeedSubScore);
			}
			m_iLockFireScore = QcMath::max_(0, m_iExitCountBuf * iScoreOneCoin -  GetInfo()->GetWinModeWinScore());
		}
		else
		{
			GetInfo()->AddScore(-iNeedSubScore);
			m_iLockFireScore = m_iExitCountBuf * iScoreOneCoin;
		}
	}
	else
	{
		m_iNeedExitLottery -= iExitNumber;
		while (m_iNeedExitLottery <0 || (m_iNeedExitLottery == 0 && m_iExitCountBuf > 0) )
			ExitedLotteryCoinRatio(1);
	}

	if (m_iExitCountBuf < 5)
		ToExitCoin();
	if (m_iExitCountBuf == 0)
		Finish();
}

void QcPlayerScoreHelper::ExitedLotteryCoinRatio(s32 nTime)
{
	int nCoinRatio = 0;
	int nLotteryRatio = 0;
	GetSysSetting()->GetCaiPiaoBiLi(nCoinRatio,  nLotteryRatio);

	int iScoreOneCoin = QmSysSetting->GetFireQtyOneCoin();
	int iSubScore = nCoinRatio * iScoreOneCoin * nTime;
	if (IsWinMode())
	{
		int iWinScore = GetInfo()->GetWinModeWinScore();
		if (iWinScore >= iSubScore)
		{
			GetInfo()->AddWinModeWinScore(-iSubScore);
			iSubScore = 0;
		}
		else
		{
			GetInfo()->SetWinModeWinScore(0);
			iSubScore -= iWinScore;
		}
	}
	GetInfo()->AddScore(-iSubScore);
	GetInfo()->AddOutLottery(nCoinRatio);
	m_iNeedExitLottery += nLotteryRatio * nTime;

	int iLockScore = ((m_iExitCountBuf - m_iNeedExitLottery)/nLotteryRatio) * (iScoreOneCoin * nCoinRatio);
	QmAssert(iLockScore >= 0);
	m_iLockFireScore = QcMath::max_(0,  iLockScore - GetInfo()->GetWinModeWinScore());
}

void QcPlayerScoreHelper::Animate(double dCurrentTime,f32 fElapsedTime)
{
	if (IsExiting())
	{
		m_fLastOutCoinTime += fElapsedTime;
		if (!IsWinMode())
			m_pExiting->Animate(dCurrentTime, fElapsedTime);
		if (m_fLastOutCoinTime > 4.f)
			SetError();
	}
	else if (IsError())
		m_pExitError->Animate(dCurrentTime, fElapsedTime);
}
void QcPlayerScoreHelper::Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix)
{
	if (IsExiting())
	{
		if (!IsWinMode())
			m_pExiting->Render(render, pWorldMatrix);
	}
	else if (IsError())
		m_pExitError->Render(render, pWorldMatrix);
}