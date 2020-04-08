#pragma once

#include "Res\QcResImageDesc.hpp"
class QcResBulletDesc : public QcResImageDesc
{
	DeclareRuntimeClass(QcResBulletDesc);
public:
	QcResBulletDesc();
	~QcResBulletDesc();

	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	void PostLoad(QcResManager* pResMgr);

	const QcObjType& GetNetKind() const {return m_netObj;}
	const QcObjType& GetNetEffA() const {return m_netEffA;}
	const QcObjType& GetNetNameA() const {return m_netNameA;}

	const QcObjType& GetNetEffB() const {return m_netEffB;}
	const QcObjType& GetNetNameB() const {return m_netNameB;}
	int GetIsShock() const {return m_bIsShock;}
	const QcPointList& GetSubFirePosList() const {return m_SubFirePosList;}
protected:
	QcObjType m_netNameA;
	QcObjType m_netObj;
	QcObjType m_netEffA;
	QcObjType m_netNameB;
	QcObjType m_netEffB;
	QcPointList	m_SubFirePosList;
	int m_bIsShock;
};