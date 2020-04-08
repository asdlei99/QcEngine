#include "pch.hpp"
#include "QcFileStorageMgr.hpp"
#include "QcFileStorageBase.hpp"
#include "FileProc.h"
#include "Thread\QcLock.hpp"

QcFileStorageMgr::QcFileStorageMgr()
{

}
QcFileStorageMgr::~QcFileStorageMgr(void)
{

}

bool QcFileStorageMgr::OnIdle(u32 /*nIdle*/)
{
	std::vector<QsGameFileInfo>::iterator iter(m_arProxySaveFile.begin());
	std::vector<QsGameFileInfo>::iterator endIter(m_arProxySaveFile.end());
	for (; iter != endIter; ++iter)
	{
		if (iter->m_pFileBase && iter->m_pFileBase->IsDirty())
		{
			QcArray<BYTE,QmMaxbuf> buf(QmMaxbuf, true);
			QcFileWriter fileWriter(buf.pointer(), buf.size(), 0);
			{
				QmLocker(iter->m_pFileBase->GetCS());
				iter->m_pFileBase->Write(fileWriter);
				iter->m_pFileBase->SetDirty(false);
			}
			QmGlobalData.EncryptData((char*)fileWriter.GetBuf(), (int)fileWriter.GetLen());
			CFileProc::Instance(iter->m_sFileName.c_str())->SaveRec(fileWriter.GetBuf(), (WORD)fileWriter.GetLen());
		}
	}
	return false;
}

bool QcFileStorageMgr::ReadFile(const char* psFile)
{
	std::vector<QsGameFileInfo>::iterator iter(m_arProxySaveFile.begin());
	std::vector<QsGameFileInfo>::iterator endIter(m_arProxySaveFile.end());
	for (; iter != endIter; ++iter)
	{
		if (iter->m_pFileBase && (psFile != NULL && _strcmpi(psFile, iter->m_sFileName.c_str()) == 0))
		{
			QcArray<BYTE, QmMaxbuf> buf(QmMaxbuf, true);
			int nLen = iter->m_pFileBase->GetLen();
			if (nLen >= QmMaxbuf)
				QmLogError("ReadFile");

			if (CFileProc::Instance(iter->m_sFileName.c_str())->LoadRec(buf.pointer(), (WORD)nLen))
			{
				QmGlobalData.DecryptData((char*)buf.pointer(), nLen);
				QmLocker(iter->m_pFileBase->GetCS());
				QcFileReader fileReader(buf.pointer(), buf.size());
				iter->m_pFileBase->Read(fileReader);
				iter->m_pFileBase->SetDirty(false);
				return true;
			}
			return false;
		}
	}
	return false;
}

void QcFileStorageMgr::SaveFile(const char* psFile)
{
	std::vector<QsGameFileInfo>::iterator iter(m_arProxySaveFile.begin());
	std::vector<QsGameFileInfo>::iterator endIter(m_arProxySaveFile.end());
	for (; iter != endIter; ++iter)
	{
		if (iter->m_pFileBase && (psFile != NULL && _strcmpi(psFile, iter->m_sFileName.c_str()) == 0))
		{
			QcArray<BYTE,QmMaxbuf> buf(QmMaxbuf, true);
			QcFileWriter fileWriter(buf.pointer(), buf.size(), 0);
			{
				QmLocker(iter->m_pFileBase->GetCS());
				iter->m_pFileBase->Write(fileWriter);
				iter->m_pFileBase->SetDirty(false);
			}
			QmGlobalData.EncryptData((char*)fileWriter.GetBuf(), (int)fileWriter.GetLen());
			CFileProc::Instance(iter->m_sFileName.c_str())->SaveRec(fileWriter.GetBuf(), (WORD)fileWriter.GetLen());
			break;
		}
	}
}