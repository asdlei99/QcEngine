#ifndef FILE_PROC_H
#define FILE_PROC_H

#include <list>
#include "FileStorageSystem\Crypt_RC4.h"

using namespace std;

class CFileProc
{
private:
	
	static std::auto_ptr<CFileProc> gsInstance;
	///轨迹数组

	///文件存储
	ULONG  m_uWriteLen;
	
	char FileNameRec[MAX_PATH];	///记录的文件名ltjdxr
	char m_cSaveFileName [3][MAX_PATH];

	CCrypt_RC4 m_cRC4;
	///文件存储标识
	BYTE m_bSaveFlag;

public:
	
	CFileProc( const char sFileNameRec[] );
	~CFileProc();

	static CFileProc * Instance( const char sFileNameRec[] );
	static void Destory();

	WORD GetCheckCode( BYTE *sData, WORD wlen );

	///加载记录文件
	bool LoadRec( BYTE * databuf, WORD wlen );
	///保存记录文件
	void SaveRec( BYTE * databuf, WORD wlen );
	///删除并覆盖原文件
	void DeleteCopyDataFile();
   //设置记录文件
   void SetResFileName( const char sFileNameRec[ ] );
};
#endif