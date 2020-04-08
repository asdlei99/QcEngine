#pragma once
#include "QcPacketSetting.hpp"

class QcPacketPerformer
{
public:
	explicit QcPacketPerformer();
	bool Excute(const char* psPara);

	static QcPacketPerformer* GetInstance()
	{
		static QcPacketPerformer performer;
		return &performer;
	}

	void SetEncryptKey(void *pKey, unsigned long keyLen)
	{
		gpCrypt->SetKey((const char*)pKey, keyLen);
	}
	void EncryptData(char *pData, unsigned long dataLen, unsigned long gapLen = 16)
	{
		gpCrypt->Encrypt(pData, dataLen);
	}
	void DecryptData(char *pData, unsigned long dataLen, unsigned long gapLen = 16)
	{
		gpCrypt->Decrypt(pData, dataLen);
	}
private:
	bool DoPacketExcute(QcPacketSetting& setting);
	bool DoAssistImage(QcPacketSetting& setting);
	bool DoMergeImage(QcPacketSetting& setting);
	bool DoPacketImage(QcPacketSetting& setting);
	bool DoRepairImage(QcPacketSetting& setting);
	
	void EncryptFile(const QcStringList& fileList);
	void EncryptFile(const QcString& file);
	void CreateMBox2wSetting(const QcPacketSetting& setting);
	void CreateImageMergeSetting(const QcStringList& imageList, const QcString& fileName);
	void ExcuteShell(const char* psExe, const char* psPara);
	void AnalysisPara(const QcString& strPara, QsParaValues& para);
	bool SearchMediaPath(QcString& modulePath);
private:
	QcSharedPtr<QiCrypt> gpCrypt;
	QcPacketSetting m_packetSetting;
};