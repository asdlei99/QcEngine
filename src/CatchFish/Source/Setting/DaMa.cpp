#include "pch.hpp"
/*#include "rand.h"*/

#ifdef QmTestVersion
#define DIFF (0)
#else
//专用打码值 比倍鱼
#define DIFF (0x87845f) 
#endif


//需要修改的函数
void tea_encipher(u32* plain, u32* key, u32* crypt)
{
	u32 z=plain[1], y=plain[0], sum=0, e, delta=0x9e3779b9;
	int p,q,n=2;
	q =6+52/n;
	u32 diff = QmSysSetting->GetDaMaPassword(); //决战岛屿专版
	key[0] = key[0] + diff, key[1] = key[1] + diff, key[2] = key[2] + diff,  key[3] = key[3] + diff;
	while(q-- > 0){
		sum += delta;
		e = (sum >> 2) & 3;
		for (p=0; p<n-1; p++) y = plain[p+1], z = plain[p] += (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (key[p&3^e]^z);
		y = plain[0];
		z = plain[n-1] += (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (key[p&3^e]^z);
	}

	crypt[0] = y;
	crypt[1] = z;
}


u8 U32ToBCD(u32 Idata,u8 *pOdataBCD)
{
	u32 temp_data =Idata;
	u8 count =0;
	do{
		*(pOdataBCD+count) =temp_data%10;
		temp_data =temp_data/10;
		if(++count >10)
			break;
		if(!temp_data)
			break;
	}while(1,1);
	return count;
}

u32 BCDToU32(u8 *BCDdata,u8 BCDdataLen)
{
	u8 i=0;
	u8 j=0;
	u8 temp_buff[20] ={0};
	u32 ret =0;
	u32 temp_data =1;
	for(i=0,j=BCDdataLen-1;i<BCDdataLen;j--,i++){ //数据位置转换
		temp_buff[j] =*(BCDdata+i);
	}
	for(i=0;i<BCDdataLen;i++){
		if(!i){
			ret +=temp_buff[i];
		}
		else{
			temp_data *=10;
			ret +=temp_buff[i]*temp_data;
		}
	}
	return ret;
}
u8 CheckNumChanged(u8 iDifficultyValue,u8 *piCheckNum,u8 poCheckNumChangedBuff[5])
{
    typedef union{
        u8 u8Buff[2];
        u16 u16Data;
    }STR_U16unionU8;
    u8 tBCDBuff_Difficulty[2] ={0};
    u8 tBuff_CheckNum[5] ={0};
    u8 tBCDBuff_CheckNumChangedBuff[5] ={0,0,0,0,0};
    u8 u8KeyBuff[4] ={0x12,0xaf,0x59,0xf3};
    STR_U16unionU8 v;
    u8 y,z,sum=0,i;
    u8 delta =0x9e;
    u8 a =u8KeyBuff[0],b =u8KeyBuff[1], c =u8KeyBuff[2],d =u8KeyBuff[3];
    u16 tCheckNum =0;
    int temp;
    if(iDifficultyValue >64)return 0;
    U32ToBCD((u32)iDifficultyValue,tBCDBuff_Difficulty);
    for(i=0;i<3;i++){
        tBuff_CheckNum[i+2] =*(piCheckNum+i);
    }
    tBuff_CheckNum[0] =tBCDBuff_Difficulty[1],tBuff_CheckNum[1] =tBCDBuff_Difficulty[0];
    tCheckNum =(u16)BCDToU32(tBuff_CheckNum,5);
    v.u16Data =tCheckNum;
    y =v.u8Buff[0],z =v.u8Buff[1];
    for(i=0; i < 4; i++) 
    {                       
        sum += delta;
        y += ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
        z += ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);
    }
    v.u8Buff[0]=y;
    v.u8Buff[1]=z;
    tCheckNum =v.u16Data;
    temp =U32ToBCD((u32)tCheckNum,tBCDBuff_CheckNumChangedBuff);
    for(i=0;i< 5;i++)
    {
        poCheckNumChangedBuff[i] =tBCDBuff_CheckNumChangedBuff[4-i];
    }
    return 1;
}


/*计数带控选择码 根据输入的四个数计算出4位十进制校验码并返回 
参数：
U32 QuanBuZong_LR = 0;//全部总利润 0~8位十进制
U32 BenCi_ShiJi_LR = 0;//本次实际利润 0~8位十进制
U32 FenJi_HM = 0;//分机号码 8位十进制
U32 BaoMa_CS = 0;//报码次数 0~5位十进制
U8* jym    一个4个元素的U8类型数组

返回值：
根据输入的四个数计算出4位十进制校验码并返回

*/
u16 JiaoYan_JYM_encipher(u32 QuanBuZong_LR, u32 BenCi_ShiJi_LR, u32 FenJi_HM, u32 BaoMa_CS, u8* jym)
{
    u32 key[4] = {3955564564+DIFF, 1202724977+DIFF, 3597265564+DIFF, 2109874265+DIFF};
    //DWORD key[4] = {3256494985u, 1436254594u, 4165748379u, 1236439473u};
    u32 plain[2];
    u32 crypt[2];
    BYTE i;
    u32 u32_temp;

    u8 EFlag=0;//0未加难 1加难
    EFlag=jym[0];

    u32_temp = 0;
    plain[0] = QuanBuZong_LR, plain[1] = BenCi_ShiJi_LR;
    tea_encipher(plain, key, crypt);
    u32_temp += crypt[0];
    u32_temp += crypt[1];

    plain[0] = FenJi_HM, plain[1] = BaoMa_CS;
    tea_encipher(plain, key, crypt);
    u32_temp += crypt[0];
    u32_temp += crypt[1];

    for(i = 4; i > 0; i--)
    {
        jym[i-1] = u32_temp % 10;
        u32_temp = u32_temp / 10;
        if(i==4)
        {
            jym[3] =0;
            //if(EFlag==1)//加难 奇
            //{
            //	if (jym[3]%2==0)//偶数时
            //	{
            //		jym[3]=(jym[3]+1)%10;
            //	}
            //}
            //else//未加难 偶
            //{
            //	if (jym[3]%2)//奇数时
            //	{
            //		jym[3]=(jym[3]+1)%10;
            //	}
            //}
        }
    }

    return 1;
}

u16 JiaoYan_JYM_decrypt(u32 QuanBuZong_LR, u32 BenCi_ShiJi_LR, u32 FenJi_HM, u32 BaoMa_CS, u8* jym)//打码器调用
{
	u32 key[4] = {3955564564, 1202724977, 3597265564, 2109874265};
//	u32 key[4] = {3256494985, 1436254594, 4165748379, 1236439473};
	u32 plain[2];
	u32 crypt[2];
	u8 i;
	u32 u32_temp;

	u8 jym_temp[4];


	u32_temp = 0;
	plain[0] = QuanBuZong_LR, plain[1] = BenCi_ShiJi_LR;
	tea_encipher(plain, key, crypt);
	u32_temp += crypt[0];
	u32_temp += crypt[1];

	plain[0] = FenJi_HM, plain[1] = BaoMa_CS;
	tea_encipher(plain, key, crypt);
	u32_temp += crypt[0];
	u32_temp += crypt[1];

	for(i = 4; i > 0; i--)
	{
		jym_temp[i-1] = u32_temp % 10;
		u32_temp = u32_temp / 10;
	}

	for(i = 0; i < 4; i++)
	{
		if(jym_temp[i] != jym[i])
		{
			return 0;
		}
	}

	return 1;

}


//参数要求：
//U8 Num ;//带控选择码类型
//“0为保持现状”
//“1为放水一千”
//“2为放水二千”
//“3为放水三千”
//“4为放水四千”
//“5为放水五千”
//“6为放水八千”
//“7为放水一万”
//“8为放水二万”
//“9为放水五万”
//“10为放水十万”
//“11为抽水一千”
//“12为抽水二千”
//“13为抽水三千”
//“14为抽水四千”
//“15为抽水五千”
//“16为抽水八千”
//“17为抽水一万”
//“18为抽水二万”
//“19为抽水五万”
//“20为抽水十万”

//U32 QuanBuZong_LR = 0;//全部总利润 0~8位十进制
//U32 BenCi_ShiJi_LR = 0;//本次实际利润 0~8位十进制
//U32 FenJi_HM = 0;//分机号码 8位十进制
//U32 BaoMa_CS = 0;//报码次数 0~5位十进制
//U8* jym    校验码，一个4个元素的U8类型数组
//U8 Num ;//带控选择码类型
//U8 Data[8];//用于存放带控选择码
//返回值为0表示打码失败，1表示低级打码器打码成功，2表示高级打码器打码成功
#ifdef QmIOUsbOn
u8 JiSuan_DKXZM_decrypt(u32 QuanBuZong_LR, u32 BenCi_ShiJi_LR, u32 FenJi_HM, u32 BaoMa_CS, u8* jym, u8 *pData,u8* Num)
{
	u8 ret=0;
	u8 temp_ret=1;
	DWORD times=0;
	TalkingCodeOperating_SendOrder(QuanBuZong_LR,BenCi_ShiJi_LR,FenJi_HM,(WORD)BaoMa_CS,jym,pData);
	while((times<150)&&(temp_ret!=0))
	{
		temp_ret=TalkingCode_DataReturn(Num);
		times++;
		if (temp_ret==0)
		{
			ret=1;
			break;
		}
		else if (temp_ret==1)
		{
			ret=2;
			break;
		}
		if (temp_ret==TALK_CODE_RETURN_ERROR)
		{
			ret=0;
			break;
		}
		Sleep(2);
	}
	return ret;

}

u8 JiSuan_TZM_decrypt(u32 JMTZM, u8* Data, u8* State_1, u8* State_2, u8* year, u8* month, u8* date, u8* hour, u8* min)
{
	u8 ret=0;
	u8 temp_ret=1;
	DWORD times=0;
	DecodeAdjustOperating_SendOrder(JMTZM,Data);
	while((times<150)&&(temp_ret!=0))
	{
		temp_ret =DecodeAdjust_DataReturn((BYTE*)State_1,(BYTE*)State_2,(BYTE*)year,(BYTE*)month,(BYTE*)date,(BYTE*)hour,(BYTE*)min);
		times++;
		if (!temp_ret)
		{
			ret=1;
			break;
		}
		if (temp_ret == DECODING_RETURN_ERROR)
		{
			ret=0;
			break;
		}
		Sleep(2);
	}
	return ret;

}
#else
u8 JiSuan_DKXZM_decrypt(u32 QuanBuZong_LR, u32 BenCi_ShiJi_LR, u32 FenJi_HM, u32 BaoMa_CS, u8* jym, u8 *pData, u8* Num)  //计算带控选择码 计算出的码存放到数组Data
{
	u32 key[4] = {1854946468u, 2987456464u, 2042654578u, 1068646747u};
//	U32 key[4] = {2793423498, 3465202843, 1702134343, 2234834981};
	u32 plain[2];
	u32 crypt[2];
	u8 i, j;
	u32 temp = 0;
	u32 jym_temp;
	u8 Data[8];



	plain[0] = QuanBuZong_LR, plain[1] = BenCi_ShiJi_LR;
	tea_encipher(plain, key, crypt);
	temp += crypt[0];
	temp += crypt[1];

	plain[0] = FenJi_HM, plain[1] = BaoMa_CS;
	tea_encipher(plain, key, crypt);
	temp += crypt[0];
	temp += crypt[1];

	jym_temp = U8ArrayToU32(jym, 0);
	plain[0] = jym_temp, plain[1] = jym_temp;
	tea_encipher(plain, key, crypt);
	temp += crypt[0];
	temp += crypt[1];

	temp = temp / 100;

	for(i = 0, j = 7; i < 8; i++, j--)
	{
		Data[j] = temp % 10;
		temp = temp / 10;
	}

	for(i = 0; i < 6; i++)
	{
		if(Data[i] != pData[i])
		{
			break;
		}
	}

	if(i < 6)
	{
		return 0;
	}

	*Num = pData[6] * 10 + pData[7];

	return 1;
}
u8 JiSuan_TZM_decrypt(u32 JMTZM, u8* Data, u32* State_1, u32* State_2, u32* year, u32* month, u32* date, u32* hour, u32* min)
{
	u32 temp, temp1, temp2;
	u32 key[4] = {2079816316, 1951264614, 1654649815, 2942165544};
//	U32 key[4] = {3293402834, 3429452038, 3942042444, 1793948094};
	u32 plain[2];
	u32 crypt[2];

	u32 TZM_State_1;
	u32 TZM_State_2;

	u8 i;

	temp = 0;
	for(i = 2; i < 12; i++)
	{
		temp = temp * 10 + Data[i];
	}

	plain[0] = JMTZM;
	plain[1] = 2404765961;
	tea_encipher(plain, key, crypt);

	temp = temp ^ crypt[1] ^ crypt[0];


	temp1 = temp & 0x61060C0;
	temp2 = JMTZM & 0x61060C0;
	if(temp1 != temp2)
	{
		return 0;
	}

	temp = temp & 0xF9EF9F3F; // 11111001 11101111 10011111 00111111

	if(Data[0] == 0 || Data[0] == 1 || Data[0] == 2 || Data[0] == 4)
	{
		TZM_State_2 = temp;
		TZM_State_2 = TZM_State_2 >> 27;
		*State_2 = TZM_State_2;

		TZM_State_1 = temp;
		TZM_State_1 = TZM_State_1 & 0x1E00000;
		TZM_State_1 = TZM_State_1 >> 21;
		*State_1 = TZM_State_1;
	}
	else
	{
		*State_1 = 3;

		*year = temp;
		*year = (*year) >> 27;

		*month = temp;
		*month = (*month) & 0x1E00000;
		*month = (*month) >> 21;

		*date = temp;
		*date = (*date) & 0xF8000;
		*date = (*date) >> 15;

		*hour = temp;
		*hour = (*hour) & 0x1F00;
		*hour = (*hour) >> 8;

		*min = temp;
		*min = (*min) & 0x3F;
	}

	return 1;
}
#endif

//根据调整码，提取出相关信息 

// U32 System_Time;//系统时间
// U8 Data[12];//存放着计算出来的调整码


// 获取解码特征码
// 输入参数：FenJi_HM--分机号
// 输出参数：U8 JieMa_TeZheng_M[8]
void get_JMTZM(u32 FenJi_HM, u8* JieMa_TeZheng_M)
{
	u32 key[4] = {2349959624+DIFF, 1565496125+DIFF, 3265721554+DIFF, 4035821684+DIFF};
//	U32 key[4] = {3809741010, 4128049456, 1733281782, 3905716334};
	u32 plain[2];
	u32 crypt[2];
	u8 i, j;
	u32 temp1 = 0, temp2 = 0;
	u8 temp4[4];

	for(i = 0; i < 4; i++)
	{
		temp4[i] = RandomInt() % 256;
	}
	temp2 = U8ArrayToU32(temp4, 0);
	temp2 = temp2 % 10000;

	key[0] = temp2;

	plain[0] = FenJi_HM, plain[1] = FenJi_HM << 2;
	tea_encipher(plain, key, crypt);
	temp1 = crypt[0] + crypt[1];

	

	for(i = 0, j = 7; i < 4; i++, j--)
	{
		JieMa_TeZheng_M[j] = temp1 % 10;
		temp1 = temp1 / 10;
	}

	for(i = 4, j = 3; i < 8; i++, j--)
	{
		JieMa_TeZheng_M[j] = temp2 % 10;
		temp2 = temp2 / 10;
	}
} 


/*
参数要求：
U32 JiTai_HM;//机台号码 8位十进制
U8 JieMa_TeZheng_M[8] ;//解码特征码 8位十进制
返回值：
校验成功为 1
校验失败为 0
*/
// 校验机台号码和解码校验码是否正确
u8 JiaoYan_JTHM_JMTZM(u32 FenJi_HM, u8* JieMa_TeZheng_M) // 打码器调用
{
	u32 key[4] = {2349959624, 1565496125, 3265721554, 4035821684};
//	U32 key[4] = {3809741010, 4128049456, 1733281782, 3905716334};
	u32 plain[2];
	u32 crypt[2];
	u8 i, j;
	u32 temp1 = 0, temp2 = 0;
	u8 data[8];

	temp1 = 0;
	for(i = 0; i < 4; i++)
	{
		temp1 = temp1 * 10 + JieMa_TeZheng_M[i];
	}
	key[0] = temp1;

	plain[0] = FenJi_HM, plain[1] = FenJi_HM << 2;
	tea_encipher(plain, key, crypt);
	temp2 = crypt[0] + crypt[1];

	for(i = 4; i > 0; i--)
	{
		data[i-1] = temp2 % 10;
		temp2 = temp2 / 10;
	}

	for(i = 0, j = 4; i < 4 && j < 8; i++, j++)
	{
		if(data[i] != JieMa_TeZheng_M[j])
		{
			return 0;
		}
	}

	return 1;

}

// 参数：
// U32 xh 线号，输入参数
// U32 jth 机台号，输入参数
// U8 flag 密码类型  0--修改线号密码，--修改机台号密码，输入参数
// U8* pwd 8个元素的U8数组，用于返回计算得到的密码，输出参数
void get_modify_xh_jth_pwd(u32 xh, u32 jth, u8 flag, u8* pwd)
{
	u32 key1[4] = {3465389035, 1735624538, 2276036476, 4073549484};
	u32 key2[4] = {3843245847, 2455678434, 1478954476, 3345345654};
	u32 key3[4] = {1236547895, 3654987247, 1536789546, 2623569668};
	u32 plain[2];
	u32 crypt[2];
	u32 temp;
	u8 i;

	plain[0] = xh;
	plain[1] = jth;


	if(flag == 0)
	{
		tea_encipher(plain, key1, crypt);
	}
	else if (flag == 1)
	{
		tea_encipher(plain, key2, crypt);
	}
	else if (flag == 2)
	{
		tea_encipher(plain, key3, crypt);
	}

	temp = crypt[0] + crypt[1];

	temp = temp / 100;
	temp = temp * 100;

	temp = temp + flag;

	for(i = 0; i < 8; i++)
	{
		pwd[i] = 0;
	}

	for(i = 8; i > 0; i--)
	{
		pwd[i-1] = temp % 10;
		temp = temp / 10;
	}
}


// 参数：
// U32 xh 线号，输入参数
// U32 jth 机台号，输入参数
// U8 flag 密码类型  0--修改线号密码，--修改机台号密码，输入参数
// U8* pwd 8个元素的U8数组，用于存放用户输入的密码，输入参数
// 
// 返回值：--正确，--失败
u8 verify_modify_xh_jth_pwd(u32 xh, u32 jth, u8 flag, u8* pwd)
{
	u8 data[8];
	u8 i;

	get_modify_xh_jth_pwd(xh, jth, flag, data);

	for(i = 0; i < 8; i++)
	{
		if(data[i] != pwd[i])
		{
			return 0;
		}
	}

	return 1;
}

u8 verify_ExShow_pwd(u32 rand_code, u32 pwd)
{
	u32 key[4] = {3465389034, 1735624538, 2276036476, 4073549484};
	u32 plain[2];
	u32 crypt[2];
	u8 temp1[6], temp2[6];
	u8 i;

	plain[0] = rand_code;
	plain[1] = rand_code << 2;

	tea_encipher(plain, key, crypt);

	for(i = 0; i < 6; i++)
	{
		temp1[i] = 0;
		temp2[i] = 0;
	}

	for(i = 6; i > 0; i--)
	{
		temp1[i-1] = crypt[0] % 10;
		crypt[0] = crypt[0] / 10;
	}

	for(i = 6; i > 0; i--)
	{
		temp2[i-1] = pwd % 10;
		pwd = pwd / 10;
	}

	for(i = 0; i < 6; i++)
	{
		if(temp1[i] != temp2[i])
		{
			return 0;
		}
	}

	return 1;

}
u8 verify_modify_ExShowDiff(u32 xh, u32 jth, u8*pwd)
{
	u32 uCode = 0;
	//U32 randCode = xh+jth;
	for(int i = 0; i < 6; i++)
	{
		uCode *= 10;
		uCode += pwd[i];
	}
	if(verify_ExShow_pwd(xh+jth, uCode))
		return 1;
	
	return 0;
}

u8 verify_modify_ExShowDiff2(u32 xh, u32 jth, u32 iRandCode,u8*pwd)
{
	u32 enNum = 0x00E2A5ED;
	u32 ret = ((iRandCode + jth + xh) ^ enNum)%100000000;
	
	u32 uCode = 0;
	for(int i = 0; i < 8; i++)
	{
		uCode *= 10;
		uCode += pwd[i];
	}

	return ret == uCode ? 1 : 0;
	//U32 uCode = 0;
	//U32 randCode = xh+jth;
	//for(int i = 0; i < 6; i++)
	//{
	//	uCode *= 10;
	//	uCode += pwd[i];
	//}
	//if(verify_ExShow_pwd(xh+jth+iRandCode%100000, uCode))
		//return 1;

}




