#pragma once

#include "Res\QcResDescParam.hpp"
class QcResEnumDescParam : public QcResDescParam
{
	DeclareRuntimeClass(QcResEnumDescParam);
public:
	QcResEnumDescParam();

	virtual void LoadParam(const QcXmlNodeIterator& node);

	QcList<QcResImageDescPtr>& GetEnumList() {return m_enumObjList;}
	QcList<QcResImageDescPtr>& GetTipList() {return m_tipObjList;}
	const QcPoint& GetTipPos() const {return m_vPos;}
	QeAlign GetTipAlign() const {return m_eAlign;}
protected:
	QcList<QcResImageDescPtr> m_enumObjList;
	QcList<QcResImageDescPtr> m_tipObjList;
	QeAlign m_eAlign;
	QcPoint m_vPos;
};