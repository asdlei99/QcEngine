#pragma once

class QcPlayerInforList;

#define QmEachPlayerCall(playerMgr, fun)\
	for (int ___iPlayer=0; ___iPlayer<QmMaxPlayer; ++___iPlayer)\
	{\
		QcPlayer* pPlayer = playerMgr->GetPlayer(___iPlayer);\
		if (pPlayer)\
			pPlayer->fun;\
	}
class QcPlayerMgr : public QcObjectEx
{
public:
	QcPlayerMgr();
	~QcPlayerMgr();

	void Init(QcResManager* pResMgr, QeMachineType eMachine);
	void Update();
 	void OnKeyDown(u32 key, const QsKeyInfo& info);
 	void OnKeyUp(u32 key, const QsKeyInfo& info);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render, u32 iFlag);

	QcPlayer* GetPlayer(u32 id){ return m_arPlayerList[id].Get();}
	QcPlayerInforList* GetPlayerInfoList() const {return m_pPlayerInfoList.Get();}

	void SetActivePlayer(int val, int playerID){}
	const int* GetActivePlayerList() const{ return NULL;}

	void GetTotalInOut(INT64& in, INT64& out);
    void PlayerFreeFired();
    bool CanFreeFire() {return m_bLotteryBulletCanFire;}
	bool IsLockNormalScene();
protected:

    void OnTimer(u32 nTimer, void* pContext);

	QcSharedPtr<QcPlayerInforList> m_pPlayerInfoList;
	QcSharedPtr<QcPlayer> m_arPlayerList[QmMaxPlayer];
	std::vector<QcPlayer*> m_arScreenPlayerList[QmMaxScreen];

    bool         m_bLotteryBulletTimerOn;
    bool         m_bLotteryBulletCanFire;
};