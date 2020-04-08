#include "pch.hpp"

#ifdef QmTestVersion

#include "QcDebugHelper.hpp"
#include "FileStorageSystem\QcPlayerInforList.hpp"
#include "Scene\QcPlayerMgr.hpp"
#include "Scene\QcPlayer.hpp"
#include "Scene\Fish\QcFishMgr.hpp"
#include "Res\QcResBackGound.hpp"
#include "Res\QcPathMgr.hpp"
#include "Res\QcResPathDesc.hpp"
#include "Res\QcResManager.hpp"
#include "Regular\QcRegularMgr.hpp"
#include "Common\QcMediaFileFinder.hpp"
#include "RenderSystem\QcVideoMemoryHelper.hpp"

f32 gfCaiDanLeaveTime = 0.f;
u32 gnRandomCount = 0;
static QcString* gpProfileInfor = NULL;
static BYTE gsWinKeyMap[256] = {0};
inline bool IsWinKeyDown(DWORD key) { return (gsWinKeyMap[key] & 0x80) != 0; }

QcDebugHelper::QcDebugHelper()
{
#ifdef QmMemLeakOn
	if (!IsDebuggerPresent())
	{
		VLDSetReportOptions(VLD_OPT_REPORT_TO_FILE, L"MemLeak.txt");
	}
#endif
	m_bShowPathID = false;
	
	char szBuff[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, szBuff, MAX_PATH);	
	char flagBuf[] = "Encrypt";
	m_bEncrypt = strstr(szBuff, flagBuf) != NULL;

	m_bKillFishFlag = 0;
	m_bLZPOn = 0;
	m_bProfileOn = 0;
	m_bAutoFire = 0;
	m_bAutoTrackFish = 0;
	m_bShowIOError = 1;
	m_bRenderParticle = 1;
	m_bPrintText = 0;
	m_bPrintIOText = 1;
	m_bPrintPlayerText = 0;
	m_bPrintProfileInfo = 2;
    m_bScreenCapture = 0;

	m_bActiveParaModifer = 0;
	m_iCurrentScaleFish = 0;
	m_dwAllTexMemNeed = 0;
	m_dwFailTexMem = 0;
	m_dwCurrentFailMem = 0;
	m_bAutoRegular = 0;

	memset(m_arLoadingInfo, 0, sizeof(m_arLoadingInfo));
	memset(m_sPlayInfoBuf, 0, sizeof(m_sPlayInfoBuf));
	QmArrayInit(m_arFailCoinBuf, 0);

	m_fScaleView = 1.f;
	m_fFrame = 0.f;
	m_fMaxTime = 0.0;
	m_bScaleViewDirty = false;
	m_iFrameMoveFlag = 1;
	m_iScore = 0;

	m_dwAdapterRam = 0;
	m_dwDedicatedVideoMemory = 0;
	m_dwDedicatedSystemMemory = 0;
	m_dwSharedSystemMemory = 0;

	QmArrayInit(m_playerKeyFlag, 1);
}
void QcDebugHelper::GetKeyMapDebug(__int32 keymap[eKeyTotal])
{
	struct KeyMap
	{
		QeKey gameKey;
		int winKey;
	};
	static KeyMap playerKeyMap[] = 
	{
		{ePlayerKeyUp, VK_UP},
		{ePlayerKeyDown, VK_DOWN},
		{ePlayerKeyLeft, VK_LEFT},
		{ePlayerKeyRight, VK_RIGHT},
		{ePlayerKeyFire, 'F'},
		{ePlayerKeyAddGunLevel, 'G'},
		{ePlayerKeyAddScore, 'A'},
		{ePlayerKeyDownScore, 'D'},
		{ePlayerKeyAddCoin, 'C'},
		{ePlayerKeyDownCoin, 'L'},
	};
	static KeyMap smallKeyMap[] =
	{
		{eSmallKeyUp, VK_UP},
		{eSmallKeyDown, VK_DOWN},
 		{eSmallKeyLeft, VK_OEM_4},
 		{eSmallKeyRight, VK_OEM_6},
		{eSmallKeyOk, VK_RETURN},
		{eSmallKeyOk, 'O'},
		{eSmallKeyCancel, VK_ESCAPE},
		{eSmallKeyCancel, 'Q'},
		{eSmallKeySmallGame, 'S'},
	};

	for (int j=0; j<QmCountOf(playerKeyMap); ++j)
	{
		if (IsWinKeyDown(playerKeyMap[j].winKey))
		{
			for (int i=0; i<QmMaxPlayer; i++)
			{
				keymap[i * ePlayerKeyTotalNum + playerKeyMap[j].gameKey]  += 1;
			}
		}
	}
#ifndef QmIOUsbOn
    if (QmSceneMgr)
    {
        QcPlayerMgr* pPlayerMgr = QmSceneMgr->GetPlayerMgr();
        if (pPlayerMgr)
        {
            if (IsWinKeyDown(VK_CONTROL))
            {
				for (int i=0; i<QmMaxPlayer; ++i)
				{
					QcPlayer* pPlayer = pPlayerMgr->GetPlayer(i);
					if (pPlayer)
						keymap[i * ePlayerKeyTotalNum + ePlayerKeyCoinOut] += pPlayer->GetScoreHelper().IsExiting() ? 1 : 0;
					else
						keymap[i * ePlayerKeyTotalNum + ePlayerKeyCoinOut] = 0;
				}
            }    
        }
    }
#endif
	for (int j=0; j<QmCountOf(smallKeyMap); ++j)
	{
		if (IsWinKeyDown(smallKeyMap[j].winKey))
			keymap[smallKeyMap[j].gameKey] += 1; 
	}
	if (QmSceneMgr && (QmSceneMgr->GetSceneState() == QcSceneMgr::eSceneSettting))
	{
		static KeyMap appendSmallKeyMap[] =
		{
			{eSmallKeyLeft, VK_LEFT},
			{eSmallKeyRight, VK_RIGHT},
		};
		for (int j=0; j<QmCountOf(appendSmallKeyMap); ++j)
		{
			if (IsWinKeyDown(appendSmallKeyMap[j].winKey))
				keymap[appendSmallKeyMap[j].gameKey] += 1; 
		}
	}
}
void QcDebugHelper::OnWinKeyUp(u32 wKey)
{
	struct KeyMap
	{
		u32 winKey;
		BYTE* pValue;
		int nDownCount;
	};
	const DWORD dwCtrl = VK_CONTROL << 16;
	const DWORD dwAlt = VK_MENU << 16;
	static KeyMap keyMap[] = 
	{
		{VK_F2, &m_bPrintIOText, 0},
		{VK_F3, &m_bPrintPlayerText, 0},
		{VK_F5, &m_bAutoFire, 0},
		{VK_F6, &m_bAutoTrackFish, 0},
		{VK_F8, &m_bLZPOn, 0},
		{dwCtrl | VK_F12, &m_bActiveParaModifer, 0},
		{dwCtrl | VK_DOWN, &m_bShowIOError, 1},
		{'P', &m_bRenderParticle, 1},
        {dwCtrl | 'P', &m_bScreenCapture, 1},
		{'Y', &m_bShowPathID, 0},
		{dwCtrl | VK_TAB, &m_bAutoRegular, 0},

		{'1', &m_playerKeyFlag[0], 0},
		{'2', &m_playerKeyFlag[1], 0},
		{'3', &m_playerKeyFlag[2], 0},
		{'4', &m_playerKeyFlag[3], 0},
		{'5', &m_playerKeyFlag[4], 0},
		{'6', &m_playerKeyFlag[5], 0},
		{'7', &m_playerKeyFlag[6], 0},
		{'8', &m_playerKeyFlag[7], 0},
		{'9', &m_playerKeyFlag[8], 0},
		{'0', &m_playerKeyFlag[9], 0},
	};
	u32 destKey = wKey;
	if (IsWinKeyDown(VK_CONTROL))
		destKey |= dwCtrl;
	if (IsWinKeyDown(VK_MENU))
		destKey |= dwAlt;

	int i=0;
	for (i=0; i<QmCountOf(keyMap); ++i)
	{
		if (destKey == keyMap[i].winKey)
		{
			*keyMap[i].pValue = (*keyMap[i].pValue) == 0 ? 1 : 0;
			break;
		}
	}

	if (m_bActiveParaModifer)
	{
		if (wKey == VK_UP)
			m_oParaModifer.Prev();
		else if (wKey == VK_DOWN)
			m_oParaModifer.Next();
		else if (wKey == VK_LEFT)
			m_oParaModifer.Decreace();
		else if (wKey == VK_RIGHT)
			m_oParaModifer.Increace();
	}


	if (i == QmCountOf(keyMap))
	{
		switch(wKey)
		{
		case VK_F9:
			{
				if (m_bProfileOn == 0)
				{
					QmProfileOn();
					m_bProfileOn = 1;
				}
				else
				{
					QmProfileOff();
					m_bProfileOn = 0;
				}
				break;
			}
		case VK_DELETE:
			{
				QcSceneMgr* pSceneMgr = QmSceneMgr;
				if (pSceneMgr && pSceneMgr->GetPlayerMgr())
				{
					QcPlayerInforList* pPlayerInforList = pSceneMgr->GetPlayerMgr()->GetPlayerInfoList();
					for (int i=0; i<QmMaxPlayer; ++i)
					{
						pPlayerInforList->GetPlayerInfo(i)->ClearAll();
					}
					QmEachPlayerCall(pSceneMgr->GetPlayerMgr(), UpdateScore());
					QmFishAlg->Reset();
				}
				break;
			}
		case VK_F1:
			{
				if (IsWinKeyDown(VK_CONTROL))
					m_bPrintText = 2;
				else if (m_bPrintText)
					m_bPrintText = 0;
				else
					m_bPrintText = 1;
				break;
			}
		case VK_F4:
			{
				if (IsWinKeyDown(VK_MENU))
				{
					giExitFlag = 1;
					return;
				}
				else if (IsWinKeyDown(VK_CONTROL))
					m_bPrintProfileInfo = 2;
				else if (m_bPrintProfileInfo)
					m_bPrintProfileInfo = 0;
				else
					m_bPrintProfileInfo = 1;
				break;
			}
		case VK_F7:
			{
                if (IsWinKeyDown(VK_MENU))
                {
                    if (m_bKillFishFlag & (1<<3))
                        m_bKillFishFlag &= ~(1<<3);
                    else
                        m_bKillFishFlag |= 1<<3;
                }
                else if (IsWinKeyDown(VK_SHIFT))
                {
                    if (m_bKillFishFlag & (1<<2))
                        m_bKillFishFlag &= ~(1<<2);
                    else
                        m_bKillFishFlag |= 1<<2;
                }
                else
                {
                    BYTE t = (m_bKillFishFlag&(1<<2)) | (m_bKillFishFlag&(1<<3));
                    m_bKillFishFlag &= ~(3<<2);
                    if (IsWinKeyDown(VK_CONTROL))
                        m_bKillFishFlag = 2;
                    else if (m_bKillFishFlag)
                        m_bKillFishFlag = 0;
                    else
                        m_bKillFishFlag = 1;

                    m_bKillFishFlag |= t;
                }
				break;
			}
		case VK_OEM_MINUS:
			{
				QmArrayInit(m_playerKeyFlag, 0);
				break;
			}
		//恢复所有玩家的输入
		case VK_OEM_PLUS:
			{
				QmArrayInit(m_playerKeyFlag, 1);
				break;
			}
		//换场景
		case VK_TAB:
			{
				if (QmSceneMgr->GetSceneState() == QcSceneMgr::eSceneNormal)
				{
					QmSceneMgr->KillTimer(eSceneTimer);
					QmSceneMgr->PostEvent(eEvtSceneStateChange, QcSceneMgr::eSceneEnd);
				}
				else if (QmSceneMgr->GetSceneState() == QcSceneMgr::eSceneRegular)
				{
					QmSceneMgr->KillTimer(eSceneTimer);
					QmSceneMgr->PostEvent(eEvtSceneStateChange, QcSceneMgr::eSceneNormal);
				}
				break;
			}
			break;
		//快速上分
		case VK_F11:
			{
				QcSceneMgr* pSceneMgr = QmSceneMgr;
				if (pSceneMgr && pSceneMgr->GetPlayerMgr())
				{
					QcPlayerInforList* pInforList = pSceneMgr->GetPlayerMgr()->GetPlayerInfoList();
					if (pInforList)
					{
						for (int i=0; i<QmMaxPlayer; ++i)
						{
							pInforList->GetPlayerInfo(i)->AddInScore(QmSysSetting->GetBaoJiScore()/QmSysSetting->GetFireQtyOneCoin());
							pInforList->GetPlayerInfo(i)->AddScore(QmSysSetting->GetBaoJiScore());
						}
						QmEachPlayerCall(pSceneMgr->GetPlayerMgr(), UpdateScore());
					}
				}
				break;
			}
		}
	}
}
void QcDebugHelper::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOUSEWHEEL)
	{
		short value = HIWORD(wParam);
		if (value < 0)
		{
			m_fScaleView -= 0.01f;
			if (m_fScaleView < ROUNDING_ERROR_f32)
				m_fScaleView = 0.01f;
		}
		else
		{
			m_fScaleView += 0.01f;
		}
		m_bScaleViewDirty = true;
	}
}
void QcDebugHelper::StartGame()
{
#if 0
	HMONITOR hMonitor = DXUTMonitorFromWindow(DXUTGetHWND(), MONITOR_DEFAULTTOPRIMARY);
	QcVideoMemoryHelper vidieHelper(hMonitor);
	if (!vidieHelper.GetVideoMemoryViaDxDiag(&m_dwAdapterRam))
		vidieHelper.GetVideoMemoryViaWMI(&m_dwAdapterRam);
	vidieHelper.GetVideoMemoryViaDXGI(&m_dwDedicatedVideoMemory, &m_dwDedicatedSystemMemory, &m_dwSharedSystemMemory);
#endif

	InitStatisticsTm();
	QcSceneMgr* pSceneMgr = QmSceneMgr;
	if (pSceneMgr && pSceneMgr->GetPlayerMgr())
	{
		QcPlayerInforList* pInforList = pSceneMgr->GetPlayerMgr()->GetPlayerInfoList();
		if (pInforList)
		{
			for (int i=0; i<QmMaxPlayer; ++i)
			{
				m_iScore += pInforList->GetPlayerInfo(i)->GetCatchScore();
			}
		}
	}
	m_fMaxTime = 0.f;
}
void QcDebugHelper::Frame()
{
	GetKeyboardState(gsWinKeyMap);

	static u64 _iBeginTime = QfGetCPUTicks();
	u64 frameTm = QfGetCPUTicks() - _iBeginTime;
	m_fMaxTime = QcMath::max_(m_fMaxTime, frameTm/gfCPUFrequency2);
	_iBeginTime += frameTm;
}
void QcDebugHelper::CalLoadingTime(QeProfilerLoadingType eType, const char* psName, bool bEnter)
{	
	if (bEnter)
	{
		m_arLoadingInfo[eType].m_psName = psName;
		m_arLoadingInfo[eType].m_iEnterTime = QfGetCPUTicks();
	}
	else
	{
		m_arLoadingInfo[eType].m_fTotalTime +=  (QfGetCPUTicks() - m_arLoadingInfo[eType].m_iEnterTime)/gfCPUFrequency2;
	}
}
void QcDebugHelper::CalTexMem(u32 nMem, bool bFailInTexMem)
{
	m_dwAllTexMemNeed += nMem;
	if (bFailInTexMem)
	{
		m_dwFailTexMem += nMem;
		m_dwCurrentFailMem += nMem;
	}
}
void QcDebugHelper::FinishedLoadingDesc(const QcString& sName)
{
	if (m_dwCurrentFailMem > 0 && m_dwCurrentFailMem >= 1024)
	{
		static int iCount = 0;
		char sBuf[256];
		sprintf_s(sBuf, "%s=%d(K)", sName.c_str(), m_dwCurrentFailMem/1024);
		m_sFailTexMem += sBuf;
		QmLog(sBuf);
		if (++iCount%5 == 0)
			m_sFailTexMem += "\n";
		m_dwCurrentFailMem = 0;
	}
}
void QcDebugHelper::InitStatisticsTm()
{
	m_arElapsedTime.push_back(QsStatisticsElapsedTm(200));
	m_arElapsedTime.push_back(QsStatisticsElapsedTm(150));
	m_arElapsedTime.push_back(QsStatisticsElapsedTm(120));
	m_arElapsedTime.push_back(QsStatisticsElapsedTm(100));
	m_arElapsedTime.push_back(QsStatisticsElapsedTm(75));
}
void QcDebugHelper::StatisticsElapsedTime(int dwTick)
{
	std::vector<QsStatisticsElapsedTm>::iterator iter(m_arElapsedTime.begin());
	std::vector<QsStatisticsElapsedTm>::iterator endIter(m_arElapsedTime.end());
	for (;iter != endIter; ++iter)
	{
		if (iter->dwTickTm < dwTick)
		{
			++(iter->dwTm);
			break;
		}
	}
}
void QcDebugHelper::PollKey(__int32 key[eKeyTotal], f32 fElapsedTime)
{	
	GetKeyMapDebug(key);

	static int iFrameCouter = 0;
	static float fFrameTimer = 0.0f;
	static float fKeyTimer = 0.0f;

	StatisticsElapsedTime(int(1000.f * fElapsedTime));

	//计算帧率
	fFrameTimer += fElapsedTime;
	++iFrameCouter;
	if(fFrameTimer > 0.5f)
	{
		m_fFrame = iFrameCouter/fFrameTimer;
		iFrameCouter = 0;
		fFrameTimer = 0.0f;
	}

	//自动发炮
	if (m_bAutoFire)
	{
		for (int i=0; i<QmMaxPlayer; ++i)
		{
			key[i * ePlayerKeyTotalNum + ePlayerKeyFire] = 1;
		}
	}
	//自动追踪
	if (m_bAutoTrackFish)
	{
		for (int i=0; i<QmMaxPlayer; ++i)
		{
			key[i * ePlayerKeyTotalNum + ePlayerKeyDown] = 1;
		}
	}
	for (int i=0; i<QmCountOf(m_playerKeyFlag); ++i)
	{
		if (m_playerKeyFlag[i] == 0)
		{
			int iBegin = i * ePlayerKeyTotalNum;
			for (int j=0; j<ePlayerKeyTotalNum; ++j)
			{
				key[iBegin + j] = 0;
			}
		}
	}

	//调试帧
	if (m_iFrameMoveFlag == 2)
	{
		m_iFrameMoveFlag = 0;
	}
	if (IsWinKeyDown(VK_SPACE))
	{
		m_iFrameMoveFlag = 2;
	}
	if (IsWinKeyDown(VK_SPACE) && IsWinKeyDown(VK_CONTROL))
	{
		m_iFrameMoveFlag = 1;
	}
	if (m_iFrameMoveFlag == 0)
	{
		memset(key, 0, sizeof(__int32) * eKeyTotal);
	}
}
void QcDebugHelper::Render(QcDeviceRender& render, f32 fElapsedTime)
{
    if (m_bPrintText == 2)
        return;

	{
		if (m_bScaleViewDirty )
		{
			QcMatrix matView(gsIdentityMatrix);
			matView.setScale(m_fScaleView, m_fScaleView);

			render.SetTransform(D3DTS_VIEW, matView);
			m_bScaleViewDirty = false;
		}
		if (m_bActiveParaModifer)
		{
			QcString str;
			str.reserve(1024);
			m_oParaModifer.Print(str);
			render.RenderText(str.c_str(), QfMakeScreenRect(0, 0, QmGlobalData.gWindowSize.Width, QmGlobalData.gWindowSize.Height), DT_RIGHT | DT_NOCLIP);
		}
		if (!m_bPrintText)
		{
			QcString str;
			char buf[64];
			sprintf_s(buf, "%.1f ", m_fFrame);
			str = buf;

			std::vector<QsStatisticsElapsedTm>::iterator iter(m_arElapsedTime.begin());
			std::vector<QsStatisticsElapsedTm>::iterator endIter(m_arElapsedTime.end());
			for (; iter != endIter; ++iter)
			{
				sprintf_s(buf, "[%d,%d]", iter->dwTickTm, iter->dwTm);
				str += buf;
			}
			
			render.RenderText(str.c_str(), QfMakeScreenRect(0.f, 0.f, QmGlobalData.gWindowSize.Width, QmGlobalData.gWindowSize.Height), DT_NOCLIP);
			return;
		}
		QcRect rect;
#ifdef QmIOUsbOn
		if (m_bPrintIOText)
		{
			QcString str;
			char tempBuf[1024] = {0};

			for (int i=0; i<QmMaxIOBoard; ++i)
			{
				GetIOReadBuf(tempBuf, i);
				str += tempBuf;

				int iBase = i * QmMaxPlayerNumOneBoard;
				sprintf_s(tempBuf, 1024, "Fail Exit:%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d\n"
				,m_arFailCoinBuf[iBase+0]
				,m_arFailCoinBuf[iBase+01]
				,m_arFailCoinBuf[iBase+02]
				,m_arFailCoinBuf[iBase+03]
				,m_arFailCoinBuf[iBase+04]
				,m_arFailCoinBuf[iBase+05]
				,m_arFailCoinBuf[iBase+06]
				,m_arFailCoinBuf[iBase+07]
				,m_arFailCoinBuf[iBase+8]
				,m_arFailCoinBuf[iBase+9]);
				str += tempBuf;

				if (!(QmSysSetting->IsTwoIOMode()))
					break;
			}
			
			GetIOConnectInfo(tempBuf);
			str += tempBuf;

			render.RenderText(str.c_str(), QfMakeScreenRect(0.f, 50.f, QmGlobalData.gWindowSize.Width, QmGlobalData.gWindowSize.Height));
		}
#endif
		if (m_bPrintPlayerText)
		{
			QcSceneMgr* pSceneMgr = QmSceneMgr;
			if (pSceneMgr && pSceneMgr->GetPlayerMgr())
			{
				char buf[QmFormatBuffer];

				QsAlgInfo info = {0};
				QmFishAlg->GetPlayerTestInfo(info);

				sprintf_s(buf,"\
							  gRejustIn:%u\n\
							  gRejustOut:%u\n\
							  coin_rate:%d\n\
							  max_send:%d\n\
							  gTotalBenifit:%d\n\
							  gBibeiTotalBenifit:%d\n\
							  biBeiIn:%d\n\
							  biBeiOut:%d\n\
							  rejustNum:%d\n\
							  randomCount:%d\n\
							  randCodeReuse:%d\n\
							  diff:%d\n\
							  AlgorithVer:%d\n\
							  ChangDiMode:%d\n \
							  DifficultLevel:%d\n \
							  ScoreChaoChange:%d\n \
							  MaxGunLevel:%d\n \
							  FireQtyOneCoin:%d\n \
							  MaxSend:%d\n \
							  MinGunLevel:%d\n \
							  JiaPaoFuDu:%d\n \
							  GameRunningTm:%u\n \
							  DaMaPassword:%08u\n",
						      info.gRejustIn,
							  info.gRejustOut,
							  info.coin_rate,
							  info.max_send,
							  info.gTotalBenifit,
							  info.gBibeiTotalBenifit,
							  info.biBeiIn,
							  info.biBeiOut,
							  info.rejustNum,
							  info.randomCount,
							  info.randCodeReuse,
							  info.diff,
							  QmFishAlg->GetVerNum(),
							  QmSysSetting->GetChangDiMode(),
							  QmSysSetting->GetDifficultLevel(),
							  QmSysSetting->GetScoreChaoChange(),
							  QmSysSetting->GetMaxGunLevel(),
							  QmSysSetting->GetFireQtyOneCoin(),
							  QmSysSetting->GetMaxSend(),
							  QmSysSetting->GetMinGunLevel(),
							  QmSysSetting->GetJiaPaoFuDu(),
							  QmSysSetting->GetRunningTime(),
							  QmSysSetting->GetDaMaPassword()
							  );
				QcPoint AlgoPos(QmGlobalData.gWindowSize.Width/2 + 300.f, QmGlobalData.gWindowSize.Height/2);
				render.RenderText(buf,QfMakeScreenRect(AlgoPos.X ,AlgoPos.Y,100.f,20.f),DT_LEFT | DT_NOCLIP | DT_VCENTER, QcColorf(1.0f, 0.0f, 0.0f));
			}
		}

		s32 iFishCount = 0;
		QcSceneMgr* pSceneMgr = QmSceneMgr;
		if (pSceneMgr && pSceneMgr->GetFishMgr())
		{
			iFishCount = pSceneMgr->GetFishMgr()->GetFishCount();
		}

		//内存信息
		MEMORYSTATUSEX memInfo = {0};
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);

		PROCESS_MEMORY_COUNTERS pmc;
		ZeroMemory(&pmc, sizeof(pmc));
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		//纹理显存剩余量
		u32 dwAvailableTextureMem = render.GetAvailableTextureMem();
		u32 iM = 1024 * 1024;

		char strYingLi[QmFormatBuffer];

		sprintf_s(strYingLi, QmFormatBuffer,
			"GameMem=%d TotalMem=%-I64d AvailMem=%-I64d VideoMem=%d AvailTexMem=%d\n\
			FPS=%.1f GainScore=%-I64d FishCount=%d, width=%d height=%d"
            ,pmc.WorkingSetSize/iM, memInfo.ullTotalPhys/iM, memInfo.ullAvailPhys/iM, m_dwAdapterRam/iM, dwAvailableTextureMem/iM
			, m_fFrame, m_iScore, iFishCount, (int)QmGlobalData.gWindowSize.Width, (int)QmGlobalData.gWindowSize.Height
			);
		render.RenderText(strYingLi, QfMakeScreenRect(400.f, 100.f, QmGlobalData.gWindowSize.Width, QmGlobalData.gWindowSize.Height), DT_LEFT | DT_NOCLIP | DT_VCENTER);
    }

	if (m_bPrintProfileInfo)
	{
		QcString str;
		if (m_bPrintProfileInfo == 2)
		{
			str.reserve(512);
			char buf[256] = {0};
			for (int i=0; i<eProfilerLoadingCount; ++i)
			{
				if (m_arLoadingInfo[i].m_psName)
				{
					sprintf_s(buf, "%s:%lf(ms)\n", m_arLoadingInfo[i].m_psName, m_arLoadingInfo[i].m_fTotalTime);
					str += buf;
				}
			}
			u32 iM = 1024 * 1024;
			sprintf_s(buf, "TotalTexMem=%d(M), FailTexMem=%d(M)\n", m_dwAllTexMemNeed/iM, m_dwFailTexMem/iM);
			str += buf;
			str += m_sFailTexMem;
		}
		else
		{
			str.reserve(2048);
			char buf[256];
			sprintf_s(buf, "MaxFrameTime: %.3f\n", m_fMaxTime);
			str = buf;

			ProfilerDataList profileDataList;
			QmGetProfilerData(profileDataList);
			ProfilerDataList::iterator iter(profileDataList.begin());
			ProfilerDataList::iterator endIter(profileDataList.end());
			for (; iter != endIter; ++iter)
			{
				if (iter->_iCount)
				{
					sprintf_s(buf, "%s: NCall=%d TotalTm=%.3lf MaxTm=%.3lf\n", iter->_psName, iter->_iCount, iter->_iTotalTime/gfCPUFrequency2, iter->_iMaxTime/gfCPUFrequency2);
					str += buf;
				}
			}
		}
		render.RenderText(str.c_str(), QfMakeScreenRect(QmGlobalData.gWindowSize.Width * 0.5f - 250.f, 0.f, 500.f, 500.f), DT_LEFT | DT_NOCLIP);
	}
}
#endif