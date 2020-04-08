#pragma once

#include "Res\QcResImageDesc.hpp"

class QcResPlayerDesc : public QcResImageDesc
{
	DeclareRuntimeClass(QcResPlayerDesc);
public:
	QcResPlayerDesc();
	virtual ~QcResPlayerDesc();

	virtual bool LoadAttibute(const QcXmlNodeIterator& node);

	const QcObjType& GetFort() const {return m_oFort;}
	const QcObjType& GetSuperFort() const {return m_oSuperFort; }
	const QcObjType& GetGunLevelFort() const {return m_oGunLevelFort;}

	const QcObjType& GetScoreFrame() const {return m_oScoreFrame;}
	const QcObjType& GetGoldColumn() const {return m_oGoldColumn;}
	const QcObjType& GetFlag() const {return m_oFlag;}
	const QcObjType& GetMovingFlag() const {return m_oMovingFlag;}
	const QcColor& GetColorFlag() const {return m_colorFlag;}
protected:
	QcObjType m_oFort;
	QcObjType m_oSuperFort;
	QcObjType m_oGunLevelFort;
	QcObjType m_oScoreFrame;
	
	QcObjType m_oGoldColumn;
	QcObjType m_oFlag;
	QcObjType m_oMovingFlag;
	QcColor m_colorFlag;
};