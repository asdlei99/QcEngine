#pragma once
#include "Res\QcResImageDesc.hpp"

class QcResImageDesc;
class QcResFishDesc : public QcResImageDesc
{
	DeclareRuntimeClass(QcResFishDesc);
public:
	QcResFishDesc();
	virtual bool LoadDesc(const QcXmlNodeIterator& node);
	virtual void LoadStateDesc(const QcXmlNodeIterator& node);
	virtual bool LoadAttibute(const QcXmlNodeIterator& node);
	virtual void PostLoad(QcResManager* pResMgr);
	virtual f32 GetDyingTime() const { return m_fDyingTime;}

	u32 GetScore() const {return m_nScore; }
	u32 GetFishType() const {return m_ID;}
	f32 GetSpeed() const {return m_fSpeed;}
	f32 GetScale() const { return m_fScale; }
	u32 GetPlayingFlag() const {return m_iPlayingFlag; }
	const QcString& GetBombEffect() const {return m_sBombEffect;}
	const QcObjType& GetDieSound() const {return m_oSound;}
	QcRuntimeClass* GetAnimator() const {return const_cast<QcRuntimeClass*>(m_pAnimator);}
protected:
	bool m_bIsBottomPath;
	u32 m_nScore;
	u32 m_ID;
	u32 m_iPlayingFlag;
	f32 m_fDyingTime;
	f32 m_fSpeed;
	f32 m_fScale;
	QcString m_sBombEffect;
	QcObjType m_oSound;
	QcRuntimeClass* m_pAnimator;
};