#include "pch\Pch.hpp"
#include "aes.h"
#include "QcAESCrypt.hpp"

QcAESCrypt::~QcAESCrypt() { QmSafeDelete(m_pAes); }

#define QmMinAESEnSize (16)
bool QcAESCrypt::SetKey(const char *pKey, int keyLen)
{
	QmRunOnce(gen_tabs);

	m_nMinEnSize = QmMinAESEnSize;
	aes_ctx *p = new aes_ctx;
	if (aes_set_key(p, (const u8*)pKey , keyLen) != 0)
	{
		delete p;
		return false;
	}
	QmSafeDelete(m_pAes);
	m_pAes = p;
	return true;
}

void QcAESCrypt::Encrypt(char *pData, int nLen)
{
	while(nLen >= QmMinAESEnSize)
	{
		aes_encrypt(m_pAes, (u8*)pData, (u8*)pData);
		pData += QmMinAESEnSize;
		nLen -= QmMinAESEnSize;
	}
	char* pXORKey = (char*)(m_pAes->buf);
	while(nLen > 0)
	{
		*pData = (*pData) ^ (*pXORKey);
		++pXORKey;
		++pData;
		--nLen;
	}
}

void QcAESCrypt::Decrypt(char *pData, int nLen)
{
	while(nLen >= QmMinAESEnSize)
	{
		aes_decrypt(m_pAes, (u8*)pData, (u8*)pData);
		pData += QmMinAESEnSize;
		nLen -= QmMinAESEnSize;
	}
	char* pXORKey = (char*)(m_pAes->buf);
	while(nLen > 0)
	{
		*pData = (*pData) ^ (*pXORKey);
		++pXORKey;
		++pData;
		--nLen;
	}
}