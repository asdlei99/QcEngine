#include "pch.hpp"
#include "QcPlayer.hpp"
#include "QcPlayerMgr.hpp"
#include "FileStorageSystem\QcPlayerInforList.hpp"
#include "Res\QcResImageDesc.hpp"

#define LOTTERY_BULLET_TIMER        10001
QcPlayerMgr::QcPlayerMgr()
{
    m_bLotteryBulletCanFire = false;
    m_bLotteryBulletTimerOn = false;
}

QcPlayerMgr::~QcPlayerMgr()
{

}

void QcPlayerMgr::Init(QcResManager* pResMgr, QeMachineType eMachine)
{
	struct QsPosInfor
	{
		int idBegin;
		int num;
		QcPoint beginPt;
		QcPoint endPt;
	};
	QsPosInfor* pPostionList = NULL;
	int iNum = 0;
	f32 halfWidth = QmGlobalData.gScreenRect.right();
	f32 halfHeiht = QmGlobalData.gScreenRect.top();
	switch (eMachine)
	{
	case eMachine_10_1:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 3, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 2, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_8_1:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 3, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_6_1:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_4_1:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_4_2:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 4, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_3_2:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_6_Stand_1IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_8_Stand_1IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 4, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 4, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_8_Lie_1IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(0, halfHeiht)},
				{0, 2, QcPoint(0, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_10_Lie_1IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(0, halfHeiht)},
				{0, 2, QcPoint(0, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_6_Stand_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{QmMaxPlayerNumOneBoard, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_8_Stand_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 4, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{QmMaxPlayerNumOneBoard, 4, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_8_Lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(0, halfHeiht)},
				{QmMaxPlayerNumOneBoard, 2, QcPoint(0, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_10_Lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(0, halfHeiht)},
				{QmMaxPlayerNumOneBoard, 2, QcPoint(0, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_12_Lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 3, QcPoint(halfWidth, halfHeiht), QcPoint(0, halfHeiht)},
				{QmMaxPlayerNumOneBoard, 3, QcPoint(0, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_14_Lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 3, QcPoint(halfWidth, halfHeiht), QcPoint(0, halfHeiht)},
				{QmMaxPlayerNumOneBoard, 3, QcPoint(0, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_UionScreen_16_Lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(0, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 3, QcPoint(halfWidth, halfHeiht), QcPoint(0, halfHeiht)},
				{QmMaxPlayerNumOneBoard, 3, QcPoint(0, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 2, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(0, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	//·ÖÆÁ
	case eMachine_TwoScreen_4_Stand_1IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_6_Stand_1IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_8_Stand_1IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 4, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 4, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_8_Lie_1IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_6_Stand_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{QmMaxPlayerNumOneBoard, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_8_Stand_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 4, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{QmMaxPlayerNumOneBoard, 4, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_8_lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{QmMaxPlayerNumOneBoard, 2, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_12_lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 2, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{QmMaxPlayerNumOneBoard, 2, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{0, 2, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_16_lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 3, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{QmMaxPlayerNumOneBoard, 3, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 1, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 1, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	case eMachine_TwoScreen_20_lie_2IO:
		{
			static QsPosInfor posArray[] = 
			{
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
				{0, 3, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 2, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{QmMaxPlayerNumOneBoard, 3, QcPoint(halfWidth, halfHeiht), QcPoint(-halfWidth, halfHeiht)},
				{0, 2, QcPoint(-halfWidth, halfHeiht), QcPoint(-halfWidth, -halfHeiht)},
				{0, 3, QcPoint(-halfWidth, -halfHeiht), QcPoint(halfWidth, -halfHeiht)},
				{0, 2, QcPoint(halfWidth, -halfHeiht), QcPoint(halfWidth, halfHeiht)},
			};
			pPostionList = posArray;
			iNum = QmCountOf(posArray);
			break;
		}
	}
	m_pPlayerInfoList = new QcPlayerInforList();
	int idPlayer = 0;
	f32 fPlayerWidth = 400.f;
	for (int i=0; i<iNum; ++i)
	{
		QcVector vec(pPostionList[i].beginPt, pPostionList[i].endPt);
		f32 fRotation = (f32)vec.getRadAngleTrig();
		f32 flen = vec.getLength();
		f32 fGap = (flen - fPlayerWidth * pPostionList[i].num)/(pPostionList[i].num + 1);
		vec.normalize(flen);

		if (pPostionList[i].idBegin != 0)
			idPlayer = pPostionList[i].idBegin;

		for (int j=0; j<pPostionList[i].num; ++j)
		{
			QcPlayer* pPlayer = pResMgr->MakeGameInstance(QaPlayerList[idPlayer])->As<QcPlayer*>();
			m_arPlayerList[idPlayer] = pPlayer;
			pPlayer->SetPosition(pPostionList[i].beginPt + vec * (fGap * (j + 1) + fPlayerWidth * (j + 0.5f) - 100.f));
			pPlayer->SetRotation(fRotation);
			++idPlayer;

			m_arScreenPlayerList[eScreen1].push_back(pPlayer);
		}
	}
	if (QmSysSetting->IsTwoScreenMode())
	{
		std::vector<QcPlayer*>::iterator iter(m_arScreenPlayerList[eScreen1].begin());
		iter += m_arScreenPlayerList[eScreen1].size()/2;
		m_arScreenPlayerList[eScreen2].insert(m_arScreenPlayerList[eScreen2].begin(), iter, m_arScreenPlayerList[eScreen1].end());
		m_arScreenPlayerList[eScreen1].erase(iter, m_arScreenPlayerList[eScreen1].end());
	}
}


void QcPlayerMgr::Update()
{
	for (int i=0; i<QmMaxScreen; ++i)
	{
		QmAutoRevertInit(s32, giScreenHandleing, i);
		std::vector<QcPlayer*>::iterator iter(m_arScreenPlayerList[i].begin());
		std::vector<QcPlayer*>::iterator endIter(m_arScreenPlayerList[i].end());
		for (; iter != endIter; ++iter)
		{
			int nID = QmArrayFind(QaPlayerList,(*iter)->GetDesc()->GetObjType());
			(*iter)->Update(nID, m_pPlayerInfoList->GetPlayerInfo(nID));
		}
	}
}

bool QcPlayerMgr::IsLockNormalScene()
{
	for (int i=0; i<QmMaxPlayer; ++i)
	{
		if (m_arPlayerList[i] && m_arPlayerList[i]->GetBulletNum())
			return true;
	}
	return false;
}

void QcPlayerMgr::OnKeyDown(u32 key, const QsKeyInfo& info)
{
	if (key >= ePlayerKeyUp && key < eSmallKeyUp)
	{
		u32 iPlayer = key/ePlayerKeyTotalNum;
		if (iPlayer < QmMaxPlayer && m_arPlayerList[iPlayer])
		{
#ifndef QmTestVersion
			if ( (iPlayer<QmMaxPlayerNumOneBoard && IO1IsConnected()) || (iPlayer>=QmMaxPlayerNumOneBoard && IO2IsConnected()))
#endif
			{
				QmAutoRevertInit(s32, giScreenHandleing, m_arPlayerList[iPlayer]->GetScreenID());
				m_arPlayerList[iPlayer]->OnKeyDown(QeKey(key%ePlayerKeyTotalNum), info);
			}
		}
	}
}

void QcPlayerMgr::OnKeyUp(u32 key, const QsKeyInfo& info)
{
	if (key >= ePlayerKeyUp && key < eSmallKeyUp)
	{
		u32 iPlayer = key/ePlayerKeyTotalNum;
		if (iPlayer < QmMaxPlayer && m_arPlayerList[iPlayer])
		{
#ifndef QmTestVersion
			if ( (iPlayer<QmMaxPlayerNumOneBoard && IO1IsConnected()) || (iPlayer>=QmMaxPlayerNumOneBoard && IO2IsConnected()))
#endif
			{
				QmAutoRevertInit(s32, giScreenHandleing, m_arPlayerList[iPlayer]->GetScreenID());
				m_arPlayerList[iPlayer]->OnKeyUp(QeKey(key%ePlayerKeyTotalNum), info);
			}
		}
	}
}

int get_random_time();
void QcPlayerMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
    bool bNoCredit = false;

	for (int i=0; i<QmMaxScreen; ++i)
	{
		QmAutoRevertInit(s32, giScreenHandleing, i);
		std::vector<QcPlayer*>::iterator iter(m_arScreenPlayerList[i].begin());
		std::vector<QcPlayer*>::iterator endIter(m_arScreenPlayerList[i].end());
		for (; iter != endIter; ++iter)
		{
			(*iter)->Animate(dCurrentTime, fElapsedTime);
			if ((*iter)->GetInfo()->GetCurScore() == 0)
				bNoCredit = true;
		}
	}

    if (bNoCredit)
    {
        if (!m_bLotteryBulletTimerOn && !m_bLotteryBulletCanFire && QmSysSetting->HasLotteryBullet())
        {
            m_bLotteryBulletTimerOn = true;
			f32 fTime = get_random_time()*60.f;
			SetTimer(LOTTERY_BULLET_TIMER, fTime, true);
        }
    }
    else
    {
        if (m_bLotteryBulletTimerOn)
        {
            m_bLotteryBulletTimerOn = false;
            KillTimer(LOTTERY_BULLET_TIMER);
        }
        m_bLotteryBulletCanFire = false;
    }
}

void QcPlayerMgr::Render(QcDeviceRender& render, u32 iFlag)
{
	std::vector<QcPlayer*>& arSceenPlayer = m_arScreenPlayerList[giScreenHandleing];
	std::vector<QcPlayer*>::iterator iter(arSceenPlayer.begin());
	std::vector<QcPlayer*>::iterator endIter(arSceenPlayer.end());
	for (;iter != endIter; ++iter)
	{
		(*iter)->Render(render, iFlag);
	}
}
void QcPlayerMgr::GetTotalInOut(INT64& in, INT64& out)
{
	in = 0;
	out = 0;
	if (m_pPlayerInfoList)
	{
		for (int i=0; i<QmMaxPlayer; ++i)
		{
			in += m_pPlayerInfoList->GetPlayerInfo(i)->GetFireScore();
			out += m_pPlayerInfoList->GetPlayerInfo(i)->GetCatchScore();
		}
	}
}

void QcPlayerMgr::OnTimer( u32 nTimer, void* pContext )
{
    m_bLotteryBulletCanFire = true;
    m_bLotteryBulletTimerOn = false;
}

void QcPlayerMgr::PlayerFreeFired()
{
    m_bLotteryBulletCanFire = false;
}

int get_random_time()
{
    double prob[] = {0.26, 0.23, 0.16, 0.13, 0.1, 0.06, 0.04, 0.02};
    int i;
    double ran;
    int t;

    ran = SPDCRandom();

    i = 0;
    while(i < sizeof(prob) && ran >= prob[i])
    {
        ran -= prob[i];
        i++;
    }

    t = 3 + i * 5 + (int)(5 * SPDCRandom());

    return t;

}