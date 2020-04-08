#pragma once

struct QsParaValues;
class QcPacketSetting
{
	friend class QcPacketPerformer;
	struct QsParaValues
	{
		QsParaValues()
		{
			m_iMergeFlag = -1;
			m_iPacketFlag = -1;
			m_iDelTmpAfterFinish = -1;
			m_iEnCryptFlag = -1;
			m_iExcuteFlag = 0;
		}
		QcString m_sSettingFile;
		QcString m_sPacketSourcePath;
		QcStringList m_sModuleList;
		int m_iMergeFlag;
		int m_iPacketFlag;
		int m_iDelTmpAfterFinish;
		int m_iEnCryptFlag;
		int m_iExcuteFlag;
	};
public:
	explicit QcPacketSetting();
	bool ReadSetting(const char* psSetting, const char* psPara);
	
	const QcString& GetOutputPath() const
	{
		return m_sOutPath;
	}
protected:
	void AnalysisPara(const QcString& strPara, QsParaValues& para);
private:
	QcString m_sSourceTargetDir;		//��ǰ·��
	QcString m_mediaPath;				//��Դ·��
	QcString m_imageMediaPath;			//ͼƬ��Դ·��
	QcString m_imageMergePath;			//��ͼ��Դ·��
	QcString m_assistImagePath;			//�ü����·��
	QcString m_pakPath;					//���ļ�·��
	QcStringList m_sDllPathList;				//Dll·��
	QcString m_sOutPath;				//���·��

	QcString m_sPacketVersion;
	QcString m_sPacketTool;				//����Ĺ���
	QcString m_sImgeMergeTool;			//��ͼ�Ĺ���

	QcString m_sPacketSourceName;		//�������exe����
	QcString m_sPacketDestFolder;		//������exe����
	QcStringList m_arCutFilterList;

	int m_iPacketFlag;
	int m_iEnCryptFlag;					//0:����ͼƬ���м��ܣ�1:����
    int m_iDelTmpFlagAfterFinish;

	bool m_bHaveEnKey;
	int m_iExcuteFlag;
};