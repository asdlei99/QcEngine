#pragma once

#include "Res\QcResFishDesc.hpp"
class QcResCombineFishDesc : public QcResFishDesc
{
	DeclareRuntimeClass(QcResCombineFishDesc);
public:
	struct QsInfo
	{
		QcObjType m_circleImage;
		QcObjType m_combineFish;
		QcPoint m_pos;
		QcSize m_size;
	};
	QcResCombineFishDesc();

	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual void PostLoad(QcResManager* pResMgr);

	bool IsSameFish() const {return m_bSameFish;}
	const QcStringList& GetFishPool() const {return m_arFishPool;}
	const QcList<QsInfo>& GetCombineFishInfo() const {return m_arCombineFishInfo;}
protected:
	virtual bool LoadCombineInfo(const QcXmlNodeIterator& node);
protected:
	QcStringList m_arFishPool;
	bool m_bSameFish;
	QcList<QsInfo> m_arCombineFishInfo;
};