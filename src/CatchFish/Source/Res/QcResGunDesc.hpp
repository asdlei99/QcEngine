#pragma once

#include "Res\QcResImageDesc.hpp"
class QcResGunDesc : public QcResImageDesc
{
	DeclareRuntimeClass(QcResGunDesc);
public:
	QcResGunDesc();
	~QcResGunDesc();

	virtual bool LoadAttibute(const QcXmlNodeIterator& node);

	f32 GetSpeed() const {return m_fSpeed;}
	void GetExpodeRadiu(f32& minRadiu, f32& maxRadiu) const {minRadiu = m_fMinExplodeRadius; maxRadiu = m_fMaxExplodeRadius;}
	const QcString& GetBulletType() const {return m_sBullet;}
	const QcObjType& GetGunEffect() const {return m_sGunFireEffect;}
	u32 GetLevelPerAdd() const {return m_nLevelPerAdd;}
	u32 GetLevelMin() const {return m_nMinLevel;}
	u32 GetLevelMax() const {return m_nMaxLevel;}
	u32 GetTextureFortIndex() const {return m_iTextureFortIndex; }
	bool GetSuperGun() const {return m_bSuperGun;}
protected:
	u32 m_iTextureFortIndex;
	f32 m_fSpeed;
	f32 m_fMinExplodeRadius;
	f32 m_fMaxExplodeRadius;
	QcString m_sBullet;
	QcObjType m_sGunFireEffect;
	u32 m_nLevelPerAdd;
	u32 m_nMinLevel;
	u32 m_nMaxLevel;
	bool m_bSuperGun;
};