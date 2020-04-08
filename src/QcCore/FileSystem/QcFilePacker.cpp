#include "Pch\Pch.hpp"
#include "QcFilePacker.hpp"

QcFilePacker::QcFilePacker(const QcString& packetName, QiCrypt* pCrytor)
: m_pDefaultCrytor(pCrytor)
, m_iCurrentPos(0)
, m_bDirty(false)
{
	if (m_fFile.Open(packetName.c_str(), std::ios::in | std::ios::out | std::ios::app | std::ios::binary) && m_fFile.GetSize())
	{
		//read file count.
		u32 headInfo[2] = {0};
		u32 endPos = 0;
		m_fFile.Seek(0 - sizeof(headInfo), std::ios::end);
		endPos = m_fFile.GetPos();
		m_fFile.Read(headInfo, sizeof(headInfo));
		DecryptData((char*)headInfo, sizeof(headInfo));

		m_fFile.Seek(headInfo[0], std::ios::beg);
		QcArray<char> buf;
		buf.resize(endPos - headInfo[0]);
		m_fFile.Read(buf.pointer(), buf.size());
		DecryptData(buf.pointer(), buf.size());

		QcMemFile memReader;
		memReader.OpenFromMem(buf.pointer(), buf.size());
		m_arPackInfoList.reserve(headInfo[1]);
		QsPackInfo fileInfo;
		for (u32 i=0; i<headInfo[1]; ++i)
		{
			memReader.Read(fileInfo.m_fileName);
			memReader.Read(fileInfo.m_iPos);
			memReader.Read(fileInfo.m_iLen);
			m_arPackInfoList.push_back(fileInfo);
		}
		
		m_iCurrentPos = headInfo[0];
		m_fFile.Seek(m_iCurrentPos, std::ios::beg);
	}
}

QcFilePacker::~QcFilePacker()
{
	if (m_bDirty)
	{
		m_arPackInfoList.insert(m_arPackInfoList.end(), m_arAppendPackInfoList.begin(), m_arAppendPackInfoList.end());
		std::sort(m_arPackInfoList.begin(), m_arPackInfoList.end(),  QsPackInfoSorter());

		std::vector<QsPackInfo>::iterator iter(m_arPackInfoList.begin());
		std::vector<QsPackInfo>::iterator endIter(m_arPackInfoList.end());

		u32 headInfo[2] = {m_iCurrentPos, m_arPackInfoList.size()};

		QcMemFile memReader;
		for (; iter != endIter; ++iter)
		{
			memReader.Write(iter->m_fileName);
			memReader.Write(iter->m_iPos);
			memReader.Write(iter->m_iLen);
		}
		WriteData(m_iCurrentPos, memReader.GetBuffer(), memReader.GetMaxSize());
		m_iCurrentPos += memReader.GetMaxSize();

		WriteData(m_iCurrentPos, (char*)headInfo, sizeof(headInfo));
		m_iCurrentPos += sizeof(headInfo);
	}
}

void QcFilePacker::PackData(const char* psName, const char* data, u32 nDataLen, QiCrypt* pCrytor)
{
	QcArray<char> buf(data, nDataLen);
	PackData2(psName, buf.pointer(), nDataLen, pCrytor);
}

void QcFilePacker::PackData2(const char* psName, char* data, u32 nDataLen, QiCrypt* pCrytor)
{
	m_bDirty = true;
	u32 iPos = m_iCurrentPos;
	QcPackFileHandle handle = FindFile(psName);
	if (handle == QmInvaidHandle)
	{
		m_arAppendPackInfoList.push_back(QsPackInfo());
		QsPackInfo& info = m_arAppendPackInfoList.back();
		info.m_fileName = psName;
		info.m_iLen = nDataLen;
		info.m_iPos = m_iCurrentPos;
		m_iCurrentPos += nDataLen;
	}
	else
	{
		QsPackInfo& info = GetInfo(handle);
		if (info.m_iLen >= nDataLen)
		{
			iPos = info.m_iPos;
			info.m_iLen = nDataLen;
		}
		else
			m_iCurrentPos += nDataLen;
	}
	WriteData(iPos, data, nDataLen, pCrytor);
}

char* QcFilePacker::ReadData(u32 iPos, u32 iLen, char* data, QiCrypt* pCrytor)
{
	m_fFile.Seek(iPos, std::ios::beg);
	m_fFile.Read(data, iLen);
	DecryptData(data, iLen, pCrytor);
	return data;
}

void QcFilePacker::WriteData(u32 iPos, char* data, u32 iDataLen, QiCrypt* pCrytor)
{
	EncryptData(data, iDataLen, pCrytor);

	m_fFile.Seek(iPos, std::ios::beg);
	m_fFile.Write(data, iDataLen);
}

void QcFilePacker::DecryptData(char* pData, u32 nDataSize, QiCrypt* pCrytor)
{
	if (pCrytor == NULL)
		pCrytor = m_pDefaultCrytor;
	if (pCrytor)
		pCrytor->Decrypt(pData, nDataSize);
}
void QcFilePacker::EncryptData(char* pData, u32 nDataSize, QiCrypt* pCrytor)
{
	if (pCrytor == NULL)
		pCrytor = m_pDefaultCrytor;
	if (pCrytor)
		pCrytor->Encrypt(pData, nDataSize);
}