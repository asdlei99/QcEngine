#pragma once

#include "Alg.h"
typedef enum 
{
	FT_2A,
	FT_ZD,
	FT_SDLS,
	FY_TOTAL
}FISH_TYPE;

typedef struct
{
	int power;					// in|out	倍数
	short over;						// out		是否死亡
	short type;						// in		鱼的类型
	int can_die;
	int zd_type;   // 0: 普通炸弹  1: 遥控炸弹
}FISH;

typedef struct
{
	int power;					// in	炮值
	int multiple;					// in	多重炮
	short produce_ion_fire;			// out	是否产生离子炮
	short produce_atom_bomb;		// out	是否产生原子弹
	short project_atom_bomb;		// in	请求发射原子弹
}FIRE;

typedef struct
{
	UINT player_id;
	FISH * fish;
	UINT fish_num;
	FIRE fireInfo;
	const int* pOnline_player_flag;

	u32 iSDLS_Score;
	char* pPrint_info;
	int nlength;
}QsKillInfo;

struct QsAlgorithmData
{
	INT64 gTotalIn;
	INT64 gTotalOut;
	INT64 gGambleIn;
	INT64 gGambleOut;
	INT64 gTotalRepairIn;
	INT64 gTotalRepairOut;
	s32 gRepairInScore;
	s32 gRepairOutScore;
};

class QcScene;
#include "FileStorageSystem\QcFileStorageBase.hpp"
class QcFishAlgorithm  : public QcFileStorageBase
{
public:
	QcFishAlgorithm();
	~QcFishAlgorithm()
	{
	}
	void Init();
	int KillFish(QsKillInfo* pInfo);
	bool StartGamble(int iScore, int inMulti);
	void Repair(int iScore);
	void Reset();
	void SetAlgDiff(u32 InOutType);

	void GetPlayerTestInfo(STR_AlgTestData& info);
	QsAlgorithmData GetPCAlgData() const {return m_sAlgData; }
	int GetVerNum() const {return m_sAlgNum;}
	int GetDiff() const {return m_diff;}
	void StopSuperFire(int player_id);
	void ContinueFire(int player_id);
protected:
	template<class T1, class T2>
	bool AlgIOSendRecv(BYTE iCase, const T1& in, T2& out)
	{
		return AlgInterface(iCase, (BYTE*)&in, sizeof(in), (BYTE*)&out, sizeof(out));
	}
	template<class T1>
	bool AlgIOSend(BYTE iCase, const T1& in)
	{
		return AlgInterface(iCase, (BYTE*)&in, sizeof(T1), NULL, 0);
	}
	template<class T1>
	bool AlgIORecv(BYTE iCase, T1& out)
	{
		return AlgInterface(iCase, NULL, 0, (BYTE*)&out, sizeof(out));
	}

	bool AlgInterface(BYTE iCase, const BYTE* pAlgInData, BYTE nDataSize, BYTE* pOutBuffer, BYTE bufferSize)
	{
#if 1
		return 0;
#else
#ifndef QmHardAlg
		BYTE outData[65] = {0};
		BYTE outDataLen = 0;
		AlgoHandle(iCase, nDataSize, (u8*)pAlgInData, &outDataLen, pOutBuffer == NULL ? outData : pOutBuffer);
		return true;
#else
		return AlgKillFish(iCase, pAlgInData, nDataSize, pOutBuffer, bufferSize);
#endif
#endif
	}
	virtual void Write(QcFileWriter& writer)
	{ 
		writer.Write(m_sAlgData);
#ifndef QmHardAlg
		writer.Write(m_sAlgData2);
#endif
	}
	virtual void Read(QcFileReader& reader)
	{ 
		reader.Read(m_sAlgData);
#ifndef QmHardAlg
		reader.Read(m_sAlgData2);
		m_bHaveAlgData = true;
#endif
	}
	
	bool KillFishPtr(int iScore, int inMulti, bool bGamble);
	void CalScore(int iIn, int iOut, bool bGamble, bool bInvalid);
	void Repair();
#ifndef QmHardAlg
public:
	int LoadData(STR_AlgData* pData)
	{
		if (m_bHaveAlgData)
			*pData = m_sAlgData2;
		return m_bHaveAlgData ? 1 : 0;
	}
	void SaveData(const STR_AlgData* pData)
	{
		QmLocker(GetCS());
		m_sAlgData2 = *pData;
		m_bHaveAlgData = true;
		SetDirty(true); 
	}
	STR_AlgData m_sAlgData2;
	bool m_bHaveAlgData;
#endif
protected:
	QsAlgorithmData m_sAlgData;
	u32 m_sAlgNum;
	u8 m_diff;
};

typedef STR_AlgTestData QsAlgInfo;

