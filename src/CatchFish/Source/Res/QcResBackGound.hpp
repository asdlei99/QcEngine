#pragma once

#include "Res\\QcResImageDesc.hpp"

struct BottomPathBoundInfo;
class QcResBackGound : public QcResImageDesc
{
	DeclareRuntimeClass(QcResBackGound);
public:
	struct QsBgNode
	{
		QcObjType m_sNodeName;
		QcPoint m_vPos;
		QcPointList m_vPointList;
	};
	QcResBackGound();

	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual bool LoadAttibute(const QcXmlNodeIterator& node);
	virtual void PostLoad(QcResManager* pResMgr);

	u32 GetSenceId() const {return m_nSceneID;}
	const QcString& GetBgMusic() const {return m_oBgMusic; }
	const QcObjType& GetBgWave() const {return m_oBgWave; }
	const QcList<QsBgNode>& GetBgSceneNodeList() const {return m_arNodeList;}
    const QcList<QcObjType>& GetDownsideObjList() const {return m_arDownsideObj;}
    const QcList<QcObjType>& GetUpsideObjList() const {return m_arUpsideObj;}
	f32 GetBgWaveAlpha()const {return m_fWaveAlphaValue;}
protected:
	bool CalculateIntersectPt(const QcPointList& pointList, QcResPathDesc* pPathDesc, QcVector& vec);
protected:
	QcString m_oBgMusic;
	u32 m_nSceneID;
	QcList<QsBgNode> m_arNodeList;
    QcList<QcObjType> m_arDownsideObj;
    QcList<QcObjType> m_arUpsideObj;
	QcObjType m_oBgWave;
	f32 m_fWaveAlphaValue;
};