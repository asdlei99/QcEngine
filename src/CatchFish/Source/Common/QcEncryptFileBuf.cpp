#include "Pch.hpp"
#include "QcEncryptFileBuf.hpp"
#include "QcMediaFileFinder.hpp"
#include "Res\QcResManager.hpp"
#include "Res\QcResPackLoader.hpp"

int QcEncryptFileBuf::Load(const char* psFile, bool appendZeroChar)
{
	if (QmGlobalData.m_pFilePacker)
	{
		QcPackFileHandle hHandle = QmGlobalData.m_pFilePacker->FindFile(PathFindFileNameA(psFile));
		if (hHandle != QmInvaidHandle)
		{
			u32 iPos;
			int iEn;
			u32 iLen = 0;
			QmGlobalData.m_pFilePacker->GetPackInfo(hHandle, iPos, iLen, iEn);
			resize(appendZeroChar ? iLen + 1 : iLen);
			QmGlobalData.m_pFilePacker->ReadData(iPos, iLen, (char*)pointer());
 			if (iEn > 0)
			{
				QmProfilerLoading(eDecryptTime);
 				QmGlobalData.DecryptData(pointer(), iLen);
			}

			if (appendZeroChar)
				pointer()[iLen] = 0;
			return hHandle + 1;
		}
	}
	QcString fullFileName;
	if (QcMediaFileFinderInstance->FindFile(psFile, fullFileName))
	{
		DWORD iLen = 0;
		QcFile file;
		if (file.Open(fullFileName.c_str(), std::ios::binary | std::ios::in))
		{
			iLen = file.GetSize();
			file.Seek(0, ios::beg);
			resize(appendZeroChar ? iLen + 1 : iLen);
			file.Read(pointer(), iLen);
			file.Close();
		}

#ifdef QmTestVersion
		if (iLen >= QmMinEncryLen && QmDebugHelper.IsEncrypt())
#else
		if (iLen >= QmMinEncryLen)
#endif	
		{
			QmGlobalData.DecryptData(pointer(), iLen);
		}
		if (appendZeroChar)
			pointer()[iLen] = 0;
		return QmInvaidHandle;
	}
	return 0;
}