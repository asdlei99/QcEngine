#pragma once

#include "Encrypt\QiCrypt.hpp"
struct aes_ctx;
class QcAESCrypt : public QiCrypt
{
public:
	QcAESCrypt() : m_pAes(NULL){}
	~QcAESCrypt();

	virtual bool SetKey(const char *pKey, int keyLen);
	virtual void Encrypt(char *pData, int nLen);
	virtual void Decrypt(char *pData, int nLen);
private:
	aes_ctx *m_pAes;
};