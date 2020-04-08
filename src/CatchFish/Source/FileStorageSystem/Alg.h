#ifndef _IOALG_H_
#define _IOALG_H_

//#define QmHardAlg

/*
�������� �ṹ����� ��д����Ӧ�� ����ƽ̨֮�䴫������
GUN gcc:
typedef struct St2Tag
{
St1 st1;
char ch2;
}__attribute__ ((packed)) St2;

ARMCC:
typedef __packed struct St2Tag
{
St1 st1;
char ch2;
}St2;

VC:
#pragma pack(1)
typedef struct St2Tag
{
St1 st1;
char ch2;
}St2;
#pragma pack()

��Բ�ͬ�ı�������ͳһд��
#ifdef __GNUC__
#define GNUC_PACKED    __attribute__ ((packed))
#else
#define GNUC_PACKED
#endif

#ifdef __arm
#define ARM_PACKED    __packed
#else
#define ARM_PACKED
#endif

#ifdef WIN32
#pragma pack(1)
#endif

typedef ARM_PACKED struct St2Tag
{
St1 st1;
char ch2;
}GNUC_PACKED St2;

#ifdef WIN32
#pragma pack()
#endif

*/

typedef signed char		s8;
typedef signed short	s16;
typedef signed long		S32;
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned long	U32;

enum
{
	eCase0,
	eCase1,
	eCase_killFish,
	eCase_AlgInit,
	eCase_GetAlgData,
	eCase_AlgReset,
	eCase_InOutRevise,
	eCase_RejustDiff,
};



typedef struct{
	S32 inMulti; // ����
	S32 inBulletScore; // ��ֵ

	S32 gTotalBenifit;   // ������Ӯ��
	S32 gBibeiTotalBenifit;   // �ȱ���Ӯ��

	u8 bibei_flag; // �ȱ���־
	u8 seqNum; // ���
}STR_AlgDataInput;

typedef struct{
		u8 u8Dead;    // 0��������1����
		u8 seqNum; // ���
}STR_AlgDataOutput;

//�߼��������
typedef struct{
	u16 level;
	u8 seqNum; // ���
}STR_AlgRejustDiff;

//�߼�����������
typedef struct{
	u8 result;
	u8 seqNum; // ���
}STR_AlgRejustDiffResult;

//in��out����
typedef struct{
	S32 inScore;
	S32 outScore;
	u8 seqNum; // ���
}STR_AlgInOutRevise;

//in��out�������
typedef struct{
	u8 reviseResult;
	u8 seqNum; // ���
}STR_AlgInOutReviseResult;

//��ʼ������
typedef struct{
	S32 coin_rate;
	S32 max_send;
	u8 seqNum; // ���
}STR_AlgDataFromPC;

//��ʼ�����
typedef struct{
	U32 algVerNum;
	u8 diff;
	u8 initResult;
	u8 seqNum; // ���
}STR_AlgInitResult;

//������
typedef struct{
	u8 resetResult;
	u8 seqNum; // ���
}STR_AlgResetResult;

// ���Դ�ӡ����
typedef struct{
	U32 gRejustIn;
	U32 gRejustOut;
	S32 coin_rate;
	S32 max_send;
	S32 gTotalBenifit;   
	S32 gBibeiTotalBenifit;
	U32 biBeiIn;
	U32 biBeiOut;
	u16 rejustNum;
	s16 randomCount;
	u16 randCodeReuse;
	u8 diff;
}STR_AlgTestData;


 typedef struct{
 	U32 rejustIn;
 	U32 rejustOut;
 	u16 checkSum;
 	u8 diff;
 }STR_FramUserData;

typedef struct{
	U32 gRejustInOut[2];
	u8 diff;
}STR_AlgData;

typedef void (*AlgSaveFn)(STR_AlgData* pData);
typedef u8 (*AlgLoadFn)(STR_AlgData* pData);
typedef U32 (*AlgGetRandomFn)(void);
typedef s16 (*AlgGetRandomCountFn)(void);

void AlgInit(AlgLoadFn pLoadFn, AlgSaveFn pSaveFn, AlgGetRandomFn pRandomFn,AlgGetRandomCountFn pAlgGetRandomCount);
void AlgoHandle(u8 iCase,u8 iDataSize,u8 *piDataBuff,u8 *poDataSize,u8 *poDataBuff);

#endif
