#include "pch.hpp"
#include "QcInputMgr.hpp"
#include "Scene\QcScene.hpp"

QsKeyInfo m_arKeyMap[eKeyTotal] = {{0.f, 0, 0.f, 0.05f, false, false}};
#define QmDefaultGapTime (0.25f)

static __int32 keymap[eKeyTotal] = {0};

bool IsKeyDown(QeKey key,int ID)
{
	return keymap[key+ ID*ePlayerKeyTotalNum] != 0;
}

QcInputMgr::QcInputMgr(QcScene* pScene)
: m_pScene(pScene)
{
	struct KeyMap
	{
		QeKey gameKey;
		f32 fGapTime;
	};
	KeyMap playerKeyMap[] = 
	{
		{ePlayerKeyUp, 0.01f},
		{ePlayerKeyDown, 0.01f},
		{ePlayerKeyLeft, 0.f},
		{ePlayerKeyRight, 0.f},
		{ePlayerKeyFire, 0.333f},
		{ePlayerKeyAddGunLevel, 0.15f},
		{ePlayerKeyAddScore, QmDefaultGapTime},
		{ePlayerKeyDownScore, QmDefaultGapTime},
		{ePlayerKeyDownCoin, QmDefaultGapTime},
		{ePlayerKeyAddCoin, 0.0f},
		{ePlayerKeyCoinOut, 0.0f},
	};

	KeyMap smallKeyMap[] =
	{
		{eSmallKeyUp, QmDefaultGapTime},
		{eSmallKeyDown, QmDefaultGapTime},
		{eSmallKeyLeft, QmDefaultGapTime},
		{eSmallKeyRight, QmDefaultGapTime},
		{eSmallKeyOk, QmDefaultGapTime},
		{eSmallKeyCancel, QmDefaultGapTime},
		{eSmallKeySmallGame, QmDefaultGapTime},
	};

	for (int j=0; j<QmCountOf(playerKeyMap); ++j)
	{
		for (int i=0; i<QmPlayerNum; ++i)
		{
			m_arKeyMap[i * ePlayerKeyTotalNum + playerKeyMap[j].gameKey].fGapTime =  playerKeyMap[j].fGapTime;
		}
	}
	for (int j=0; j<QmCountOf(smallKeyMap); ++j)
	{
		m_arKeyMap[smallKeyMap[j].gameKey].fGapTime = smallKeyMap[j].fGapTime; 
	}
}

void QcInputMgr::Poll()
{
	static __int64 _iLastTime = QfGetCPUTicks();
	__int64 iCurrentTime = QfGetCPUTicks();
	f32 fElapsedTime = (iCurrentTime - _iLastTime)/(f32)giCPUFrequency;
	_iLastTime = iCurrentTime;

	GetKeyMap(keymap);
#ifdef QmTestVersion
	QmDebugHelper.PollKey(keymap, fElapsedTime);
#endif
	Poll(keymap, fElapsedTime);
}

void QcInputMgr::Poll(__int32 key[eKeyTotal], f32 fElapsedTime)
{
	for (int i=0; i<eKeyTotal; ++i)
	{
		QsKeyInfo& keyInfo = m_arKeyMap[i];
		keyInfo.fTimeCount += fElapsedTime;
		if (key[i])
		{
			if (keyInfo.bOnlyOneDown && keyInfo.nKeyDownCount != 0)
				continue;

			keyInfo.fKeyDownTime += fElapsedTime;
			keyInfo.nKeyDownCount = key[i];
			if (keyInfo.fTimeCount >= keyInfo.fGapTime)
			{
				keyInfo.fTimeCount = fElapsedTime;
				m_pScene->OnKeyDown((QeKey)i, keyInfo);
				//keyInfo.fTimeCount -= keyInfo.fGapTime;
			}
		}
		else
		{
// 			if (keyInfo.fTimeCount >= keyInfo.fGapTime)
// 				keyInfo.fTimeCount -= keyInfo.fGapTime;

			if (keyInfo.nKeyDownCount != 0)
			{
				if (!keyInfo.bIngoreUp)
				{
					m_pScene->OnKeyUp((QeKey)i, keyInfo);
				}
				keyInfo.fKeyDownTime = 0.f;
				keyInfo.nKeyDownCount = 0;
				keyInfo.bIngoreUp = false;
				keyInfo.bOnlyOneDown = false;
			}
		}
	}
}