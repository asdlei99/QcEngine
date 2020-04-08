#pragma once

//计算打码校验码
extern u16 JiaoYan_JYM_encipher(u32 QuanBuZong_LR, u32 BenCi_ShiJi_LR, u32 FenJi_HM, u32 BaoMa_CS, u8* jym);
extern u8 CheckNumChanged(u8 iDifficultyValue,u8 *piCheckNum,u8 poCheckNumChangedBuff[5]);
//计算打码条码
extern u8 JiSuan_DKXZM_decrypt(u32 QuanBuZong_LR, u32 BenCi_ShiJi_LR, u32 FenJi_HM, u32 BaoMa_CS, u8* jym, u8 *pData, u8* Num);
//计算打码特征码
extern void get_JMTZM(u32 FenJi_HM, u8* JieMa_TeZheng_M);
//计算打码条码
extern u8 JiSuan_TZM_decrypt(u32 JMTZM, u8* Data, u8* State_1, u8* State_2, u8* year, u8* month, u8* date, u8* hour, u8* min);

u8 verify_modify_xh_jth_pwd(u32 xh, u32 jth, u8 flag, u8* pwd);
u8 verify_modify_ExShowDiff(u32 xh, u32 jth, u8 *pwd);
u8 verify_modify_ExShowDiff2(u32 xh, u32 jth, u32 iRandCode,u8*pwd);
