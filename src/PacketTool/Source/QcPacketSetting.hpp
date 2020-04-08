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
	QcString m_sSourceTargetDir;		//当前路径
	QcString m_mediaPath;				//资源路径
	QcString m_imageMediaPath;			//图片资源路径
	QcString m_imageMergePath;			//合图资源路径
	QcString m_assistImagePath;			//裁剪后的路径
	QcString m_pakPath;					//大文件路径
	QcStringList m_sDllPathList;				//Dll路径
	QcString m_sOutPath;				//输出路径

	QcString m_sPacketVersion;
	QcString m_sPacketTool;				//打包的工具
	QcString m_sImgeMergeTool;			//合图的工具

	QcString m_sPacketSourceName;		//被打包的exe名称
	QcString m_sPacketDestFolder;		//打包后的exe名称
	QcStringList m_arCutFilterList;

	int m_iPacketFlag;
	int m_iEnCryptFlag;					//0:不对图片进行加密，1:加密
    int m_iDelTmpFlagAfterFinish;

	bool m_bHaveEnKey;
	int m_iExcuteFlag;
};