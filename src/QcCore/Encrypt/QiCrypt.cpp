#include "pch\Pch.hpp"
#include "aes.h"
#include "QiCrypt.hpp"

int QiCrypt::CalRandomEnBufSize(int nRawSize)
{
	return ((nRawSize + sizeof(s32) + sizeof(char)  +  (m_nMinEnSize - 1) )/m_nMinEnSize) * m_nMinEnSize;
}

bool QiCrypt::RandomEncrypt(char *pData, int nEnSize, int nSourceLen)
{
	if (nEnSize - nSourceLen >= sizeof(s32) + sizeof(char))
	{
		char cRandomCh = rand();
		char* pTempData = pData;

		for (int i=0; i<nSourceLen; ++i)
		{
			*pTempData ^= cRandomCh;
			++pTempData;
		}

		int nRandomCount = nEnSize - nSourceLen - sizeof(s32) - sizeof(char);
		for (int i=0; i<nRandomCount; ++i)
		{
			*pTempData = rand();
			++pTempData;
		}

		*((s32*)pTempData) = s32(nSourceLen);
		pTempData += sizeof(s32);

		*pTempData = cRandomCh;

		Encrypt(pData, nEnSize);
		return true;
	}
	return false;
}

bool QiCrypt::RandomDecrypt(char *pData, int nDeSize, int& nSource)
{
	Decrypt(pData, nDeSize);

	char* pTempData = pData + nDeSize - 1;
	char cRandomCh = *pTempData;

	pTempData -= sizeof(s32);
	nSource = *((int*)pTempData);

	for (int i=0; i<nSource; ++i)
	{
		*pData ^= cRandomCh;
		++pData;
	}
	return true;
}