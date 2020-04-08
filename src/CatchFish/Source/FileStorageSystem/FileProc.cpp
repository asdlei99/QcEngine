#include "pch.hpp"
#include "windows.h"
#include "math.h"
#include "FileProc.h"


std::auto_ptr<CFileProc> CFileProc::gsInstance;

CFileProc::CFileProc( const char sFileNameRec[] )
{
	m_cRC4.Init();
	m_uWriteLen = 0;
	m_bSaveFlag = 0;

	SetResFileName(sFileNameRec);
}

void CFileProc::SetResFileName( const char sFileNameRec[] )
{
	_snprintf_s(FileNameRec,sizeof(FileNameRec),MAX_PATH, "%s", sFileNameRec );

	for(int i = 0 ; i < 3 ; ++i)
	{
		/*char szSystemName [MAX_PATH] = {'\0'};
		GetSystemDirectoryA(szSystemName,sizeof(szSystemName));*/
		_snprintf_s(this->m_cSaveFileName[i],sizeof(this->m_cSaveFileName[i]),"%s\\%s%d",QmGlobalData.gsFileStoragePath.c_str(),FileNameRec,i);
	}
}

void CFileProc::Destory()
{
	gsInstance.reset(NULL);
}

CFileProc::~CFileProc()
{
}

CFileProc * CFileProc::Instance( const char sFileNameRec[] )
{
	if(gsInstance.get() == NULL)
	{
		gsInstance.reset(new CFileProc( sFileNameRec ));
	}
	else
	{
		gsInstance->SetResFileName( sFileNameRec );
	}

	return gsInstance.get();
}

#if 0
bool CFileProc::LoadRec( BYTE * databuf, WORD wlen )
{	
	bool bLoadSuccess = false;
	try
	{
		memset( databuf, 0, wlen );
		char szFileName [MAX_PATH] = {0};
		char szSystemName [MAX_PATH] = {0};
		GetSystemDirectoryA(szSystemName,sizeof(szSystemName));
	
		FILE * pRecFile = NULL;

		for ( int i = 2 ; i > -1 ; --i )
		{
			_snprintf_s(szFileName, sizeof(szFileName),MAX_PATH, "%s\\%s%d",szSystemName,FileNameRec,i);
			fopen_s(&pRecFile, szFileName,"rb"); 
			if(pRecFile != NULL)
			{
				rewind(pRecFile);
				///获取长度
				long length = 0;
				fseek( pRecFile, 0, SEEK_END );
				length = ftell( pRecFile );
				fseek( pRecFile, 0, SEEK_SET );
				BYTE * buf = new BYTE[length+1];
				memset ( buf, 0, length+1 );
				int readb = fread(buf,length,1,pRecFile);
				if (readb != 1 && length != wlen + 2  ) 
				{
					fclose(pRecFile);
					pRecFile = NULL;
					delete [] buf;
					continue;
				}

				buf[length] = 0;
				WORD CS = 0;
				memcpy(&CS,buf,sizeof(CS));
				
				char * dataBuff = new char [ wlen ];
				this->m_cRC4.EnCrypt((char *)buf+2,length-2,dataBuff);

				///校验
				WORD checkCS = GetCheckCode((BYTE *)dataBuff, wlen);
				if ( checkCS != CS )
				{
					delete [] dataBuff;
					delete [] buf;
					fclose(pRecFile);
					pRecFile = NULL;
					continue;
				}				

				memcpy( databuf,dataBuff,wlen );
				delete [] dataBuff;
				delete [] buf;
				fclose( pRecFile );
				pRecFile = NULL;

				bLoadSuccess = true;  //成功标记
				break;
			}
		}//end for				
	}
	catch(...)
	{
	}

	if( bLoadSuccess == false )
	{
		memset( databuf,0,wlen );
		return false;
	}

	return true;
}


void CFileProc::SaveRec( BYTE * databuf, WORD wlen )
{
	if( databuf == NULL )
	{
		return;
	}

	try
	{
		BYTE sbuf[ QmMaxbuf ];
		DWORD startTime = GetTickCount();
		FILE * pRecFile = NULL;
		fopen_s(&pRecFile, m_cSaveFileName[2],"rb+");
		if(pRecFile == NULL)////不存在创建文件
		{
			fopen_s(&pRecFile, m_cSaveFileName[2],"w");
			if(pRecFile != NULL)
				fclose(pRecFile);
			///再次打开
			fopen_s(&pRecFile, m_cSaveFileName[2],"rb+");
		}
		if(pRecFile != NULL)
		{
			///禁用缓冲区
			setvbuf(pRecFile,NULL,_IONBF,0);
			
			
			memcpy( sbuf, databuf, wlen );
			
			int writeDataLen = wlen;
			char * writeData = new char[ wlen ];
			m_cRC4.EnCrypt((char *)sbuf,writeDataLen,writeData);
			
			m_uWriteLen = wlen+2;
			m_pWriteBuf = new BYTE [m_uWriteLen];

			WORD CS = GetCheckCode( sbuf, wlen );
			memcpy(m_pWriteBuf,&CS,sizeof(CS));
			memcpy(m_pWriteBuf+2,writeData,writeDataLen);

			int ret = fwrite((char *)m_pWriteBuf,m_uWriteLen,1,pRecFile);

			delete [ ] m_pWriteBuf;
			delete [ ] writeData;
			m_pWriteBuf = NULL;
			if(ret != 1)///存储错误
			{
			}
			else
			{
				fflush( pRecFile );
			}
		}
		else///打开失败 
		{
			return;
		}
		
		if(pRecFile != NULL && fclose(pRecFile) != 0)		
		{

		}
		
		DeleteCopyDataFile();
	}
	catch(...)
	{
		_fcloseall();
	}
}
#else
bool CFileProc::LoadRec( BYTE * databuf, WORD wlen )
{	
	bool bLoadSuccess = false;
	try
	{
		memset( databuf, 0, wlen );

		FILE * pRecFile = NULL;
		for ( int i = 2 ; i > -1 ; --i )
		{
			fopen_s(&pRecFile, m_cSaveFileName[i],"rb"); 
			if(pRecFile != NULL)
			{
				rewind(pRecFile);
				///获取长度
				long length = 0;
				fseek( pRecFile, 0, SEEK_END );
				length = ftell( pRecFile );
				fseek( pRecFile, 0, SEEK_SET );
				BYTE * buf = new BYTE[length+1];
				memset ( buf, 0, length+1 );
				int readb = fread(buf,length,1,pRecFile);
				if (readb != 1 && length != wlen + 2  ) 
				{
					fclose(pRecFile);
					pRecFile = NULL;
					delete [] buf;
					continue;
				}

				buf[length] = 0;
				WORD CS = 0;
				memcpy(&CS,buf,sizeof(CS));

				char * dataBuff = new char [ wlen ];
				this->m_cRC4.EnCrypt((char *)buf+2,length-2,dataBuff);

				///校验
				WORD checkCS = GetCheckCode((BYTE *)dataBuff, wlen);
				if ( checkCS != CS )
				{
					delete [] dataBuff;
					delete [] buf;
					fclose(pRecFile);
					pRecFile = NULL;
					continue;
				}				

				memcpy( databuf,dataBuff,wlen );
				delete [] dataBuff;
				delete [] buf;
				fclose( pRecFile );
				pRecFile = NULL;

				bLoadSuccess = true;  //成功标记
				break;
			}
		}//end for				
	}
	catch(...)
	{
	}

	if( bLoadSuccess == false )
	{
		memset( databuf,0,wlen );
		return false;
	}

	return true;
}


void CFileProc::SaveRec( BYTE * databuf, WORD wlen )
{
	if( databuf == NULL )
	{
		return;
	}
	try
	{
		FILE * pRecFile = NULL;
		fopen_s(&pRecFile, m_cSaveFileName[2],"rb+");
		if(pRecFile == NULL)////不存在创建文件
		{
			fopen_s(&pRecFile, m_cSaveFileName[2],"w");
			if(pRecFile != NULL)
				fclose(pRecFile);
			///再次打开
			fopen_s(&pRecFile, m_cSaveFileName[2],"rb+");
		}
		if(pRecFile != NULL)
		{
			///禁用缓冲区
			setvbuf(pRecFile,NULL,_IONBF,0);

			WORD CS = GetCheckCode(databuf, wlen);
			QcArray<char, QmMaxbuf + sizeof(CS)> writeData(wlen + sizeof(CS), true);

			memcpy(writeData.pointer(), &CS, sizeof(CS));
			m_cRC4.EnCrypt((char*)databuf, wlen, writeData.pointer() + sizeof(CS));
			int ret = fwrite((char *)writeData.pointer(),writeData.size(),1,pRecFile);
			m_uWriteLen = writeData.size();

			if(ret != 1)///存储错误
			{
			}
			else
			{
				fflush( pRecFile );
			}
		}
		else///打开失败 
		{
			return;
		}

		if(pRecFile != NULL && fclose(pRecFile) != 0)		
		{

		}

		DeleteCopyDataFile();
	}
	catch(...)
	{
		_fcloseall();
	}
}
#endif

WORD CFileProc::GetCheckCode( BYTE *sData, WORD wlen )
{
	WORD res = 0;
	BYTE * CSData = (BYTE *)sData;
	for(int i = 0; i < wlen ; ++i)
	{
		res = ((*CSData)+res)%0xFFFF;
		++CSData;
	}
	return res;
}

void CFileProc::DeleteCopyDataFile()
{
	DWORD flength = 0;
	HANDLE handle = CreateFileA(m_cSaveFileName[2], FILE_READ_EA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (handle != INVALID_HANDLE_VALUE)
    {
        flength = GetFileSize(handle, NULL);
        CloseHandle(handle);
    }
	if(flength == m_uWriteLen)
	{
		if(!CopyFileA(m_cSaveFileName[2],m_cSaveFileName[m_bSaveFlag],false))
		{
			return;
		}

		m_bSaveFlag = (++m_bSaveFlag)%2;
	}
	else
	{
		DeleteFileA(m_cSaveFileName[2]);
	}
}