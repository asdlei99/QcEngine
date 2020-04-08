#include "pch.hpp"
#include "QcResPlayerDesc.hpp"
#include "Scene\QcPlayer.hpp"

ImplRuntimeClass(QcResPlayerDesc);

QcResPlayerDesc::QcResPlayerDesc()
{
	m_pRuntimeClass = QmRuntimeClass(QcPlayer);
}

QcResPlayerDesc::~QcResPlayerDesc()
{

}

bool QcResPlayerDesc::LoadAttibute(const QcXmlNodeIterator& node)
{
	bool bRet = QcResImageDesc::LoadAttibute(node);
	if (bRet)
	{
		m_oFort = node.Attribute("Fort");
		m_oSuperFort = node.Attribute("SuperFort");
		m_oGunLevelFort = node.Attribute("GunLevelFort");
		m_oScoreFrame = node.Attribute("ScoreFrame");
		m_oGoldColumn = node.Attribute("GoldColumn");
		m_oFlag = node.Attribute("Flag");
		m_oMovingFlag = node.Attribute("MovingFlag");
		m_colorFlag = node.Attribute("ColorFlag", QcColor(255,255,255,255));
	}
	return bRet;
}