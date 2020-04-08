#pragma once
#include <vector>
#include "Thread\QcThread.hpp"

class QcFileStorageBase;
//�����ļ���Ϣ��������һ��ߵ�ϵͳ������Ϣ�ȡ���Ҫ�Ǵ������ʱ���ݶ�ʧ�����⡣
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
	void RegistryFileForSave(const QcFileStorageBase* pFileBase, const char* psFileName)//ע����Ҫ������ļ�����
	{
		m_arProxySaveFile.push_back(QsGameFileInfo(pFileBase, psFileName));
	}
protected:
	virtual bool  OnIdle(u32 nIdle);
	std::vector<QsGameFileInfo>	m_arProxySaveFile;	//��Ҫ������ļ��б�
};


