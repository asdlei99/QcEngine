#pragma once

#include "Encrypt\QiCrypt.hpp"

class QcTEACrypt : public QiCrypt
{
public:
	virtual bool SetKey(const char *pKey, int keyLen);
	virtual void Encrypt(char *pData, int nLen);
	virtual void Decrypt(char *pData, int nLen);
private:
	u32 m_key[4];
};