#pragma once

#include "Common\QcReferenceCounted.hpp"
class QiCrypt : public QcReferenceCounted
{
public:
	virtual bool SetKey(const char *pKey, int keyLen) = 0;
	virtual void Encrypt(char *pData, int nLen) = 0;
	virtual void Decrypt(char *pData, int nLen) = 0;

	virtual int CalRandomEnBufSize(int nRawSize);
	virtual bool RandomEncrypt(char *pData, int nEnSize, int nSourceLen);
	virtual bool RandomDecrypt(char *pData, int nDeSize, int& nSource);
protected:
	u32 m_nMinEnSize;
};