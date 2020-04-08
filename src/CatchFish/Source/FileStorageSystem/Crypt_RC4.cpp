#include "crypt_rc4.h"

CCrypt_RC4::CCrypt_RC4(void)
{
}

CCrypt_RC4::~CCrypt_RC4(void)
{
}

void CCrypt_RC4::Init()
{
	int j = 0;
	int temp;
	int kmax = strlen((char *)vendor_code);
	for(int i = 0 ; i < CODE_LENGTH ; ++i)//////////////给字节状态矢量和可变长的密钥数组赋值
	{
		m_is[i] = i;
		m_it[i] = vendor_code[i%kmax];
	}
	for(int i = 0;i < CODE_LENGTH;++i) //////使用可变长的密钥数组初始化字节状态矢量数组s
	{
		j = (j+m_is[i]+m_it[i])%256;
		temp = m_is[i];
		m_is[i] = m_is[j];
		m_is[j] = temp;
	} 
}

void CCrypt_RC4::EnCrypt(const char *enStr,int nLen,char * outStr)
{
	int temps [CODE_LENGTH] = {0};
	memcpy(temps,m_is,sizeof(temps));

	int m = 0;
	int n = 0;
	int	q = 0;
	int temp = 0;
	int max = nLen;
	for(int i = 0 ; i < max ; ++i)/////////////由字节状态矢量数组变换生成密钥流并对密文字符进行解密
	{
		m = (m+1)% CODE_LENGTH;
		n = (n+temps[n])% CODE_LENGTH;
		temp = temps[m];
		temps[m] = temps[n];
		temps[n] = temp;
		q = (temps[m]+temps[n])%CODE_LENGTH;
		outStr[i] = (char)(enStr[i]^temps[q]); 
	} 
}
