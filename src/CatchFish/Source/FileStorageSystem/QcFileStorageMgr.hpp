#pragma once
#include <vector>
#include "Thread\QcThread.hpp"

class QcFileStorageBase;
//保存文件信息，例如玩家或者的系统设置信息等。主要是处理掉电时数据丢失的问题。
class QcFileStorageMgr : public QcThread
{
	struct QsGameFileInfo
	{
	public:
		QsGameFileInfo(const QcFileStorageBase* pFileBase, const char* psFileName)
			: m_pFileBase(const_cast<QcFileStorageBase*>(pFileBase)), m_sFileName(psFileName) {}

		QcString m_sFileName;
		QcFileStorageBase* m_pFileBase;
	};
public:
	QcFileStorageMgr();
	virtual ~QcFileStorageMgr(void);

	bool ReadFile(const char* psFile);
	void SaveFile(const char* psFile);
	void RegistryFileForSave(const QcFileStorageBase* pFileBase, const char* psFileName)//注册需要保存的文件代理
	{
		m_arProxySaveFile.push_back(QsGameFileInfo(pFileBase, psFileName));
	}
protected:
	virtual bool  OnIdle(u32 nIdle);
	std::vector<QsGameFileInfo>	m_arProxySaveFile;	//需要保存的文件列表
};


