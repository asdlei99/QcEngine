#pragma once

#include "Scene\QcSceneNodePointerList.hpp"
class QcBulletMgr : public QcObject
{
public:
	QcBulletMgr();
	~QcBulletMgr();

	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render);
	
	void Add(const QcBullet* bullet);
	void Remove(const QcBullet* bullet);
	QcBulletPtrList& GetBulletList() { return m_arBullet;}
protected:
	QcSceneNodePointerList<QcBullet> m_arBullet;
};