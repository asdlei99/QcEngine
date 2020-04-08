#pragma once

#define QmMaxPlayerNumOneBoard (10)
#define QmMaxIOBoard (2)
#define QmPlayerNum (QmMaxPlayerNumOneBoard * QmMaxIOBoard)

#define	 TALK_CODE_RETURN_ERROR     238
#define	 DECODING_RETURN_ERROR		222

typedef enum
{
	Normal=0,//����
	MemoryLack,//�ڴ治��
	DeviceIDError,//�豸ID����
	DeviceWIError,//�豸����ʼ������
	DeviceRIError,//�豸д��ʼ������
	VerifyError1,//��1��У��ʧ��
	VerifyError2,//��2��У��ʧ��
	VerifyError3,//��3��У��ʧ��
	DeviceLose,//�豸��ʧ
	CommuVerifiyError,//ͨѶУ��ʧ��
	DeviceNeetToReboot,//�豸��Ҫ���� ��⵽�������ʱ ��������ɨ��

	eAlgError1,
}EncryptCardError;

enum QeKey
{
	ePlayerKeyUp		= 0,			//��
	ePlayerKeyDown		= 1,			//��
	ePlayerKeyLeft		= 2,			//��
	ePlayerKeyRight		= 3,			//��
	ePlayerKeyFire		= 4,			//����
	ePlayerKeyAddGunLevel = 5,			//Ѻ��
	ePlayerKeyAddScore	= 6,			//�Ϸ�
	ePlayerKeyDownScore	= 7,			//ϴ��
	ePlayerKeyAddCoin	= 8,			//Ͷ��
	ePlayerKeyDownCoin	= 9,			//�˱�
	ePlayerKeyCoinOut	= 11,			//�˱�����
	ePlayerKeyTotalNum = 12,

	eSmallKeyUp = (QmPlayerNum * ePlayerKeyTotalNum) + 1,
	eSmallKeyDown,
	eSmallKeyLeft,
	eSmallKeyRight,
	eSmallKeyOk,
	eSmallKeyCancel,
	eSmallKeySmallGame,
	eKeyTotal,
};

#define QmMaxIOBuf (40)
struct QsRecvUsbData
{
	unsigned char eMsg;
	unsigned char cParam;
	unsigned char iErrorFlag;
	unsigned char dataBuf[QmMaxIOBuf];
	unsigned char dataLen;
};

BYTE HardRandomData();
long GetUsableRandomCount();
bool InitCom();
int GetCardErrorCode(void);
void SetCardErrorCode(int iCode);
bool IO1IsConnected();			//IO
bool IO2IsConnected();			//IO
void GetKeyMap(__int32 keymap[eKeyTotal]);
void GetIOReadBuf(char sTmp[1024], int iBoard = 0);
void GetIOConnectInfo(char sTmp[1024]);
void ExitCoin(BYTE player_id, int iCount, bool bTicket);
bool AlgKillFish(BYTE iCase, const BYTE* pAlgInData, BYTE nDataSize, BYTE* pOutBuffer, BYTE bufferSize);
void ShutDownIOThread(void);
/*��ȡ�ߺš���̨��*/
extern BYTE GetClassGroupNum(WORD* pOxianhao,DWORD* pOjitaihao);
/*�޸��ߺš���̨��*/
extern BYTE ModifyClassGroupNum(WORD Ixianhao,DWORD Ijitaihao);
/*��ȡͼƬ���ļ�������Կ : Iflag:1Ϊ��ȡͼƬ��Կ��2Ϊ��ȡ�ļ���Կ*/
BYTE GetEnKey(BYTE Iflag,DWORD* Opdata);

inline void encrypt(unsigned long *v, unsigned long *k) //����
{
	unsigned long z=v[1], y=v[0], sum=0, e, delta=0x9e3779b9;
	int p,q,n=2;
	q =6+52/n;
	while(q-- > 0){
		sum += delta;
		e = (sum >> 2) & 3;
		for (p=0; p<n-1; p++) 
		{
			y = v[p+1];
			v[p] += (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (k[p&3^e]^z);
			z = v[p];
		}
		y = v[0];
		z = v[n-1] += (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (k[p&3^e]^z);
	}
}

inline void decrypt(unsigned long *v, unsigned long *k)  //����
{
	unsigned long z=v[1], y=v[0], sum=0, e, delta=0x9e3779b9;
	int p,q,n=2;
	q =6+52/n;
	sum =q*delta;
	while (sum != 0) {
		e = (sum >> 2) & 3;
		for (p=n-1; p>0; p--) z = v[p-1], y = v[p] -= (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (k[p&3^e]^z);
		z = v[n-1];
		y = v[0] -= (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (k[p&3^e]^z);
		sum -= delta;
	}
}

void EncryptBytes(BYTE* pSrc, int nCountByte, unsigned long *key, int iGap = 8);
void DecryptBytes(BYTE* pSrc, int nCountByte, unsigned long *key, int iGap = 8);

//����
/*�����оƬ���ͼ��㹫ʽ: *pIbuffBCD: 8��BCD�루��ͨ��С�����޸ģ�*/
extern BYTE TransmitComputationalFormula(BYTE* pIbuffBCD);
extern BYTE TalkingCode_DataReturn(BYTE* pOdatabuff);
extern void TalkingCodeOperating_SendOrder(DWORD IallTotProfit,DWORD IactualProfit,DWORD ImachineNum,WORD ItalkNum,BYTE* pIchackNum,BYTE* pIkeypanelbuff);
extern void DecodeAdjustOperating_SendOrder(DWORD IdNum,BYTE* pIpanelNum);
extern BYTE DecodeAdjust_DataReturn(BYTE* pOstatus1,BYTE* pOstatus2,BYTE* pOyear,BYTE* pOmonth,BYTE* pOdate,BYTE* pOhour,BYTE* pOminute);



inline BYTE CheckDataXor( BYTE * pData, WORD wLen )
{
	BYTE base = pData[ 0 ];
	for( int i = 1; i < wLen - 1; i ++ )
	{
		base ^= pData[ i ];
	}
	return base;
}
inline unsigned long U8ArrayToU32( unsigned char *src, unsigned char pos )
{
	unsigned long temp, temp1, temp2;
	temp = 0x00000000;
	temp1 = 0x00000000;
	temp2 = 0x00000000;
	temp |= ( src[pos + 3] );
	temp = temp << 24;
	temp1 |= ( src[pos+2] );
	temp1 = temp1 << 16;
	temp |= temp1;
	temp2 |= ( src[pos+1] );
	temp2 = temp2 << 8;
	temp |= temp2;
	temp |= ( src[pos] );
	return temp;
}
inline void U16ToU8Array( unsigned char *dest, unsigned short src)
{
	( *dest ) = ( src & 0x00ff );
	( *( dest + 1 ) ) = ( ( src>>8 ) & 0x00ff );
	return;
}


inline void U32ToU8Array( unsigned char *dest, unsigned long src)
{
	( *dest ) = ( src & 0x000000ff );
	( *( dest + 1 ) ) = ( ( src>>8 ) & 0x000000ff );
	( *( dest + 2 ) ) = ( ( src>>16 ) & 0x000000ff );
	( *( dest + 3 ) ) = ( ( src>>24 ) & 0x000000ff );
	return;
}
inline unsigned short U8ArrayToU16( unsigned char *src, unsigned char pos )
{
	unsigned short temp;
	temp = 0x0000;
	temp |= ( src[pos + 1] );
	temp = temp << 8;
	temp |= ( src[pos] );
	return temp;
}