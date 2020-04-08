#include "pch\Pch.hpp"
#include "QcTEACrypt.hpp"

typedef unsigned int uint32_t;
#define QmMinAESEnSize (8)
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

inline void bteaEncrypt(u32 *v, unsigned n, const u32 key[4])
{
	if (n >= 2)
	{
		uint32_t y, z, sum;
		unsigned p, rounds, e;

		rounds = 6 + 52/n;
		sum = 0;
		z = v[n-1];
		do {
			sum += DELTA;
			e = (sum >> 2) & 3;
			for (p=0; p<n-1; p++) {
				y = v[p+1]; 
				z = v[p] += MX;
			}
			y = v[0];
			z = v[n-1] += MX;
		} while (--rounds);
	}
}

inline void bteaDecrypt(u32 *v, unsigned n, const u32 key[4])
{
	if (n >= 2)
	{
		uint32_t y, z, sum;
		unsigned p, rounds, e;

		rounds = 6 + 52/n;
		sum = rounds*DELTA;
		y = v[0];
		do {
			e = (sum >> 2) & 3;
			for (p=n-1; p>0; p--) {
				z = v[p-1];
				y = v[p] -= MX;
			}
			z = v[n-1];
			y = v[0] -= MX;
		} while ((sum -= DELTA) != 0);
	}
}

bool QcTEACrypt::SetKey(const char *pKey, int keyLen)
{
	m_nMinEnSize = 8;

	if (keyLen >= 16)
	{
		memcpy(m_key, pKey, 16);
		return true;
	}
	else if (keyLen > 0)
	{
		memcpy(m_key, pKey, keyLen);
		char* p = (char*)m_key;
		for (int i=keyLen; i<16; ++i)
		{
			p[i] = pKey[(i+1)%keyLen];
		}
		return true;
	}
	return false;
}
void QcTEACrypt::Encrypt(char *pData, int nLen)
{
	unsigned iSize = (nLen/m_nMinEnSize) * m_nMinEnSize;
	bteaEncrypt((u32*)pData, iSize/sizeof(long), m_key);
	pData += iSize;
	for (int i=iSize; i<nLen; ++i)
	{
		pData[i] ^= ((char*)m_key)[i%16];
	}
}
void QcTEACrypt::Decrypt(char *pData, int nLen)
{
	unsigned iSize = (nLen/m_nMinEnSize) * m_nMinEnSize;
	bteaDecrypt((u32*)pData, iSize/sizeof(long), m_key);
	pData += iSize;
	for (int i=iSize; i<nLen; ++i)
	{
		pData[i] ^= ((char*)m_key)[i%16];
	}
}