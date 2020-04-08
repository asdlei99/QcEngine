#pragma once

class QcWinFile
{
public:
	QcWinFile() : m_hFile(INVALID_HANDLE_VALUE), m_bAppend(false) {}
	~QcWinFile()
	{
		Close();
	}

	bool Open(const char* psFile, long openMode)
	{
		//std::ios::in;		//��
		//std::ios::out;	//д
		//std::ios::ate;	//��ʱ�ļ���β
		//std::ios::app;	//ÿ��д�������ƶ����ļ�β
		//std::ios::trunc;	//���Ѵ��ڵ��ļ������������
		if (openMode & std::ios::_Nocreate)
			openMode |= std::ios::in;	// file must exist
		if (openMode & std::ios::app)
			openMode |= std::ios::out;	// extension -- app implies out

		DWORD dwDesiredAccess = 0;
		DWORD dwShareMode = 0;
		DWORD dwCreationDisposition = 0;
		if (openMode & std::ios::in)
		{
			dwDesiredAccess |= GENERIC_READ;
			dwShareMode |= FILE_SHARE_READ;
			dwCreationDisposition = OPEN_EXISTING;
		}
		if (openMode & std::ios::out)
		{
			dwDesiredAccess |= GENERIC_WRITE;
			dwShareMode |= FILE_SHARE_WRITE | FILE_SHARE_READ;
			if (openMode & std::ios::trunc)
				dwCreationDisposition = TRUNCATE_EXISTING;
			else if ( (openMode & std::ios::app) || (openMode & std::ios::in))
				dwCreationDisposition = OPEN_ALWAYS;
			else
				dwCreationDisposition = CREATE_ALWAYS;
		}
		if (openMode & std::ios::_Nocreate)		//��ʾ�ļ������ڣ���򿪲���ʧ��
		{
			dwCreationDisposition = OPEN_EXISTING;
		}
		if (openMode & std::ios::_Noreplace)	//��ʾ�ļ����ڣ���򿪲���ʧ��
		{
			dwCreationDisposition = CREATE_NEW;
		}
		m_hFile = CreateFileA(psFile, dwDesiredAccess, dwShareMode, 0, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			if ((openMode & std::ios::app) || (openMode & std::ios::ate))
				Seek(0, std::ios::end);
		}
		m_bAppend = (openMode & std::ios::app) != 0;
		return m_hFile != INVALID_HANDLE_VALUE;
	}
	void Seek(long lDistanceToMove, std::ios_base::seekdir _Way)
	{
		SetFilePointer(m_hFile, lDistanceToMove, NULL, _Way);
	}
	long GetPos()
	{
		return SetFilePointer(m_hFile, 0, 0, FILE_CURRENT);
	}
	long GetSize()
	{
		return GetFileSize(m_hFile, NULL);
	}
	void Close()
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
	s32 Read(void* buffer, u32 sizeToRead)
	{
		DWORD dwOut = 0;
		ReadFile(m_hFile, buffer, sizeToRead, &dwOut, NULL);
		return dwOut;
	}
	s32 Write(const void* buffer, u32 sizeToWrite)
	{
		if (m_bAppend)
			Seek(0, std::ios::end);
		DWORD dwOut = 0;
		WriteFile(m_hFile, buffer, sizeToWrite, &dwOut, NULL);
		return dwOut;
	}
protected:
	HANDLE m_hFile;
	bool m_bAppend;
};