#ifndef FILE_PROC_H
#define FILE_PROC_H

#include <list>
#include "FileStorageSystem\Crypt_RC4.h"

using namespace std;

class CFileProc
{
private:
	
	static std::auto_ptr<CFileProc> gsInstance;
	///�켣����

	///�ļ��洢
	ULONG  m_uWriteLen;
	
	char FileNameRec[MAX_PATH];	///��¼���ļ���ltjdxr
	char m_cSaveFileName [3][MAX_PATH];

	CCrypt_RC4 m_cRC4;
	///�ļ��洢��ʶ
	BYTE m_bSaveFlag;

public:
	
	CFileProc( const char sFileNameRec[] );
	~CFileProc();

	static CFileProc * Instance( const char sFileNameRec[] );
	static void Destory();

	WORD GetCheckCode( BYTE *sData, WORD wlen );

	///���ؼ�¼�ļ�
	bool LoadRec( BYTE * databuf, WORD wlen );
	///�����¼�ļ�
	void SaveRec( BYTE * databuf, WORD wlen );
	///ɾ��������ԭ�ļ�
	void DeleteCopyDataFile();
   //���ü�¼�ļ�
   void SetResFileName( const char sFileNameRec[ ] );
};
#endif