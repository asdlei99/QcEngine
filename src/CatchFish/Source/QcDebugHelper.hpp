#pragma once

#include "QcParaModifier.hpp"
class QcResPathDesc;

enum QeProfilerLoadingType
{
	eDecryptTime = 0,
	eUncompressTime,
	eTextureTime,

	eLoadingParticleTime,
	eLoadingSoundTime,
	eLoadingPathTime,
	eLoadingBg,
	eLoadingNormalImage,
	eLoadingFish,
	eLoadingPlayer,
	eLoadingSetting,
	eInitSceneTime,

	eAllLoadingTime,
	eProfilerLoadingCount,
};


class QcDebugHelper
{
	struct QsProfilerLoadingInfor
	{
		f32 m_fTotalTime;
		__int64 m_iEnterTime;
		const char* m_psName;
	};
public:
	QcDebugHelper();

	void StartGame();
	void Frame();
	void OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnWinKeyUp(u32 wKey);
	void PollKey(__int32 key[eKeyTotal], f32 fElapsedTime);
	void Render(QcDeviceRender& render, f32 fElapsedTime);

	void CalTexMem(u32 nMem, bool bFailInTexMem);
	void FinishedLoadingDesc(const QcString& sName);
	void CalLoadingTime(QeProfilerLoadingType eType, const char* psName, bool bEnter);
	bool OnFrameMove() { return m_iFrameMoveFlag > 0;}
	bool IsEncrypt() const { return m_bEncrypt;}
	BYTE KillFishFlag()const{ return m_bKillFishFlag;}

	char* GetPlayerInforBuf(u32 id)
	{
		if (m_bPrintPlayerText && m_bPrintText)
			return m_sPlayInfoBuf[id];
		return NULL;
	}
	void AddCurrentScore(int iScore)
	{
		QmAssert(iScore > 0);
		m_iScore += iScore;
	}
	void AddFailExitCoin(int playID, int iCoin) { m_arFailCoinBuf[playID] += iCoin;}
	bool IsShowPathIDOn() const{ return m_bShowPathID != 0;}
	bool IsShowIOError() { return m_bShowIOError != 0 && m_bPrintText != 0;}
    bool IsStartCaptureScreen(){bool b = m_bScreenCapture==1; m_bScreenCapture=0; return b;}

	template<class T>
	s32 AddModifyItem(T& value, T min, T max, T gap, const char* psName)
	{
		m_oParaModifer.AddModifyItem(value, min, max, gap, psName);
		return 0;
	}
protected:
	static void LogProfilerString(int id, const char* sName, int iCount, __int64 totalTime, __int64 maxTime, __int64 minTime);
	void GetKeyMapDebug(__int32 keymap[eKeyTotal]);
protected:
	bool m_bEncrypt;
	BYTE m_bKillFishFlag;						//0:����	1:һ����ɱ		2:����    3��4:�س���λը��
	BYTE m_bLZPOn;								//����������
	BYTE m_bProfileOn;							//��������
	BYTE m_bAutoFire;							//�Զ�����
	BYTE m_bAutoTrackFish;						//�Զ�׷��
	BYTE m_playerKeyFlag[QmMaxPlayer];
	BYTE m_bRenderParticle;
	BYTE m_bShowPathID;
	BYTE m_bShowIOError;
	BYTE m_bAutoRegular;
    BYTE m_bScreenCapture;

	//��ӡ��Ϣ
	BYTE m_bPrintText;							
	//IO����
	int m_arFailCoinBuf[QmMaxPlayer];
	BYTE m_bPrintIOText;						
	//�㷨����
	char m_sPlayInfoBuf[QmMaxPlayer][2048];		
	BYTE m_bPrintPlayerText;								
	//��������
	QsProfilerLoadingInfor m_arLoadingInfo[eProfilerLoadingCount];
	BYTE m_bPrintProfileInfo;

	QcList<QcPointList> m_arPathPointList;
	
	int m_iCurrentScaleFish;
	BYTE m_bScaleFish;							//������Ĵ�С

	BYTE m_iFrameMoveFlag;						//����֡
	f32 m_fFrame;
	f32 m_fMaxTime;
	QcString m_sMonitorName;
	f32 m_fScaleView;							//������ͼ��С
	u64 m_iScore;
	bool m_bScaleViewDirty;

	DWORD m_dwAdapterRam;
	DWORD m_dwDedicatedVideoMemory;
	DWORD m_dwDedicatedSystemMemory;
	DWORD m_dwSharedSystemMemory;

	DWORD m_dwAllTexMemNeed;
	DWORD m_dwFailTexMem;
	DWORD m_dwCurrentFailMem;
	QcString m_sFailTexMem;

	//ʵʱ���Բ���
	QcParaModifier m_oParaModifer;
	BYTE m_bActiveParaModifer;

	//ʱ��ͳ��
	struct QsStatisticsElapsedTm
	{
		QsStatisticsElapsedTm(int dwTick) : dwTickTm(dwTick), dwTm(0) {}
		int dwTickTm;
		int dwTm;
	};
	void InitStatisticsTm();
	void StatisticsElapsedTime(int dwTick);
	std::vector<QsStatisticsElapsedTm> m_arElapsedTime;
};