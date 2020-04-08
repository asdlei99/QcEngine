#pragma once

class QcStdFile
{
public:
	QcStdFile(){}

	bool Open(const char* psFile, long openMode)
	{
		m_file.open(psFile, openMode);
		return !m_file.fail();
	}
	void Seek(long lDistanceToMove, std::ios::_Seekdir _Way)
	{
		m_file.seekg(lDistanceToMove, _Way);
	}
	long GetPos()
	{
		return (long)m_file.tellg();
	}
	long GetSize()
	{
		long savePos = GetPos();
		Seek(0, std::ios::end);
		long ret = GetPos();
		Seek(savePos, std::ios::beg);
		return ret;
	}
	void Close()
	{
		m_file.close();
	}
	long Read(void* buffer, u32 sizeToRead)
	{
		m_file.read((char*)buffer, sizeToRead);
		return (long)m_file.gcount();
	}
	long Write(const void* buffer, u32 sizeToWrite)
	{
		m_file.write((const char*)buffer, sizeToWrite);
		return sizeToWrite;
	}
protected:
	std::fstream m_file;
};