#include "pch.hpp"
#include "QcResGunDesc.hpp"
#include "Scene\QcGun.hpp"

ImplRuntimeClass(QcResGunDesc);


QcResGunDesc::QcResGunDesc()
{
	m_pRuntimeClass = QmRuntimeClass(QcGun);
}
QcResGunDesc::~QcResGunDesc()
{

}

bool QcResGunDesc::LoadAttibute(const QcXmlNodeIterator& node)
{
	QcResImageDesc::LoadAttibute(node);

	m_fSpeed = node.Attribute("BulletSpeed", 1000.f);
	m_fMinExplodeRadius = node.Attribute("MinExplodeRadius", 60.f);
	m_fMaxExplodeRadius = node.Attribute("MaxExplodeRadius", 90.f);
	m_sBullet = node.Attribute("Bullet");
	m_nLevelPerAdd = node.Attribute("levelperadd", 1);
	m_nMinLevel = node.Attribute("minlevel", 1);
	m_nMaxLevel = node.Attribute("maxlevel", 1);
	m_bSuperGun = node.Attribute("SuperGun", 0) > 0;
	m_sGunFireEffect = node.Attribute("GunFireEffect");
	m_iTextureFortIndex = node.Attribute("GunLevelFort", 0);
	return true;
}