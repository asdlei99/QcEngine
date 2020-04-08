#include "pch.hpp"
#include "QcFishAlgorithm.hpp"
#include "Scene\QcScene.hpp"
#include "Scene\QcPlayerMgr.hpp"
#include "FileStorageSystem\QcFileStorageMgr.hpp"

static u8 gAlgSeqID = 0;

#ifndef QmHardAlg
//#pragma comment(lib, "Alg.lib")
unsigned long AlgGetHardRandomData()
{
	return unsigned long(SPDCRandom() * 0x00ffffff);
}
s16 AlgGetRandomCount()
{
	return 0;
}
void AlgSaveData(STR_AlgData* pSaveData)
{
	QmFishAlg->SaveData(pSaveData);
}
u8 AlgLoadData(STR_AlgData* pSaveData)
{
	return (u8)QmFishAlg->LoadData(pSaveData);
}
#endif

QcFishAlgorithm::QcFishAlgorithm()
{
#ifndef QmHardAlg
	memset(&m_sAlgData2, 0, sizeof(m_sAlgData2));
	m_bHaveAlgData = false;
#endif
	memset(&m_sAlgData, 0, sizeof(m_sAlgData));
	m_sAlgNum = 0;
	m_diff = 0;
}

void QcFishAlgorithm::Init()
{
	m_sAlgNum = 0;
	m_diff = 0;

#ifndef QmHardAlg
	//AlgInit(AlgLoadData, AlgSaveData, AlgGetHardRandomData, AlgGetRandomCount);
#endif
	STR_AlgDataFromPC algInit = {0};
	STR_AlgInitResult algRet = {0};
	algInit.coin_rate = QmSysSetting->GetFireQtyOneCoin();
	algInit.max_send = QmSysSetting->GetMaxSend();
	if (AlgIOSendRecv(eCase_AlgInit, algInit, algRet))
	{
		if (algRet.initResult != 0 && algRet.initResult != 1)
			SetCardErrorCode(eAlgError1 + algRet.initResult);
		else
		{
			m_sAlgNum = algRet.algVerNum;
			m_diff = algRet.diff;
		}
	}
	Repair();
}

int QcFishAlgorithm::KillFish(QsKillInfo* pInfo)
{
	QmProfileFragment(KillFish);
	{
		int fish_num = pInfo->fish_num;
		int have_remote_bomb = 0;
		for (int i = 0; i < fish_num; i++)
		{
			if (pInfo->fish[i].zd_type == 1)
			{
				have_remote_bomb = 1;
				break;
			}
		}
		if (have_remote_bomb)
		{
			fish_num = 2;
		}
		else
		{
			f32 ran = Randomf();
			if (ran < 0.47)
			{
				fish_num = fish_num > 1 ? 1: fish_num;
			}
			else if (ran < 0.85)
			{
				fish_num = fish_num > 2 ? 2: fish_num;
			}
			else
			{
				fish_num = fish_num > 3 ? 3: fish_num;
			}
		}
		if (have_remote_bomb == 0) 
		{
			if (fish_num == 2)
			{
				if (pInfo->fish[0].power + pInfo->fish[1].power > 18)
				{
					fish_num = 1;
				}
			}
			else if (fish_num == 3)
			{
				if (pInfo->fish[0].power + pInfo->fish[1].power + pInfo->fish[2].power > 30)
				{
					fish_num = 2;

					if (pInfo->fish[0].power + pInfo->fish[1].power > 18)
					{
						fish_num = 1;
					}
				}
			}
		}
		int inMulti = 0;
		for (int i = 0; i < fish_num; ++i)
		{
			if (pInfo->fish[i].can_die == 1)
			{
				inMulti += pInfo->fish[i].power * pInfo->fireInfo.multiple;
			}
		}
		int iScore = pInfo->fireInfo.power;
		if (KillFishPtr(iScore, inMulti, false))
		{
			for (int i=0; i<fish_num; ++i)
			{
				if (pInfo->fish[i].can_die == 1)
				{
					pInfo->fish[i].over = 1;
				}
			}
		}
	}
	return 0;
}

bool QcFishAlgorithm::StartGamble(int iScore, int inMulti)
{
	return KillFishPtr(iScore, inMulti, true);
}
bool QcFishAlgorithm::KillFishPtr(int iScore, int inMulti, bool bGamble)
{
	bool bInValid = true;
	bool bDead = false;
	STR_AlgDataInput inData = {0};
	STR_AlgDataOutput outData = {0};

	inData.inBulletScore = iScore;
	inData.inMulti = inMulti;
	inData.bibei_flag = bGamble ? 1 : 0;
	inData.gTotalBenifit = s32(m_sAlgData.gTotalIn - m_sAlgData.gTotalOut)/QmSysSetting->GetFireQtyOneCoin();
	inData.gBibeiTotalBenifit = s32(m_sAlgData.gGambleIn - m_sAlgData.gGambleOut)/QmSysSetting->GetFireQtyOneCoin();
	inData.seqNum = gAlgSeqID + 1;
	//if (AlgIOSendRecv(eCase_killFish, inData, outData))
	if (inMulti > 0)
		outData.u8Dead = Randomf() < 1.f/inData.inMulti ? 1 : 0;
	{
		++gAlgSeqID;
		bDead = outData.u8Dead != 0;
		bInValid = false;
	}
	CalScore(inData.inBulletScore, bDead ? inData.inMulti *  inData.inBulletScore : 0, bGamble, bInValid);
	return bDead;
}

void QcFishAlgorithm::CalScore(int iIn, int iOut, bool bGamble, bool bInvalid)
{ 
	QmLocker(GetCS());
	if (bGamble)
	{
		m_sAlgData.gGambleIn += iIn;
		m_sAlgData.gGambleOut += iOut;
	}
	m_sAlgData.gTotalIn += iIn; 
	m_sAlgData.gTotalOut += iOut; 
	if (bInvalid)
	{
		m_sAlgData.gRepairInScore += iIn; 
		m_sAlgData.gTotalRepairIn += iIn;
	}
	SetDirty(true); 
}

void QcFishAlgorithm::Repair(int iScore)
{
	//RepairScore
	if (iScore)
	{
		QmLocker(GetCS());
		m_sAlgData.gRepairOutScore += iScore;
		m_sAlgData.gTotalRepairOut += iScore;
		m_sAlgData.gTotalOut += iScore;
		SetDirty(true); 
	}
	Repair();
}

void QcFishAlgorithm::Repair()
{
	STR_AlgInOutRevise reviseData = {0};
	reviseData.inScore = m_sAlgData.gRepairInScore;
	reviseData.outScore = m_sAlgData.gRepairOutScore;
	if (reviseData.inScore != 0 || reviseData.outScore != 0)
	{
		reviseData.seqNum = gAlgSeqID + 1;
		if (AlgIOSend(eCase_InOutRevise, reviseData))
		{
			QmLocker(GetCS());
			++gAlgSeqID;
			m_sAlgData.gRepairInScore -= reviseData.inScore;
			m_sAlgData.gRepairOutScore -= reviseData.outScore;
			SetDirty(true);
		}
	}
}

void QcFishAlgorithm::Reset()
{
	{
		QmLocker(GetCS()); 
		memset(&m_sAlgData, 0, sizeof(m_sAlgData));
		SetDirty(true);
	}
	STR_AlgResetResult resetOut = {0};
	AlgIORecv(eCase_AlgReset, resetOut);
}

void QcFishAlgorithm::GetPlayerTestInfo(STR_AlgTestData& info)
{
	static STR_AlgTestData sData = {0};
	AlgIORecv(eCase_GetAlgData, sData);
	info = sData;
}

void QcFishAlgorithm::SetAlgDiff(u32 InOutType)
{
	STR_AlgRejustDiff reDiffIn = {0};
	STR_AlgRejustDiffResult reDiffOut = {0};
	reDiffIn.level = (u16)InOutType;
	AlgIOSendRecv(eCase_RejustDiff, reDiffIn, reDiffOut);
}



void QcFishAlgorithm::StopSuperFire(int player_id)
{
}

void QcFishAlgorithm::ContinueFire(int player_id)
{
}