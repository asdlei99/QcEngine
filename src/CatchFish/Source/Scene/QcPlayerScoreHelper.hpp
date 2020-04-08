#pragma once

class QcPlayerScoreHelper : public QcObjectEx
{
	friend class QcDebugHelper;
public:
	QcPlayerScoreHelper();

	virtual void OnTimer(u32 nTimer, void* pContext);

	void Init(QcPlayer* m_pPlayer);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	bool OnKeyDown(QeKey key, const QsKeyInfo& info);
	bool OnKeyUp(QeKey key, const QsKeyInfo& info);
	void HitAddScore(u32 iScore);
	s32 GetLockFireScore() const {return m_iLockFireScore;}
	s32 GetBaoJiScore() const {return m_iBaoJiScore;}
protected:
	s32 GetLockSendScore() const;
	bool IsWinMode() const {return m_bWinMode;}
	bool IsCoinMode() const {return m_bCoinMode; }
	bool IsError() const {return m_bError;}
	bool IsExiting() {return m_iExitCountBuf > 0;}
	bool AddScore(s32 nCount, bool bCoin = true);
	void SetError();
	void Finish();
	bool ToExitCoin();
	void ToExitCoin(s32 iExitNumber);		//发送退币命令
	void ExitedCoin(s32 iExitNumber);		//成功退币
	void ExitedLotteryCoinRatio(s32 nTime = 1);

	void DownAllScore();
	void DownScore(s32 iMinCoin);
	QcPlayer* GetPlayer() {return m_pPlayer;}
	QcPlayerInfo* GetInfo() {return m_pPlayerInfo;}
	QcSystemSetting* GetSysSetting() const {return m_pSystemSetting;}
protected:
	bool m_bWinMode;
	bool m_bCoinMode;
	bool m_bError;
	bool m_bOutCoinKeyDown;			//下完所有的分
	f32 m_fLastOutCoinTime;
	s32 m_iExitCountBuf;
	s32 m_iLockFireScore;
	s32 m_iNeedExitLottery;
	s32 m_iBaoJiScore;


	QcPlayer* m_pPlayer;
	QcPlayerInfo* m_pPlayerInfo;
	QcSystemSetting* m_pSystemSetting;

	QcSceneNodePtr m_pExitError;
	QcSceneNodePtr m_pExiting;
};