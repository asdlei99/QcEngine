#include "pch.hpp"
#include "QcBullet.hpp"
#include "QcBulletMgr.hpp"
#include "Fish\QcFishMgr.hpp"

QcBulletMgr::QcBulletMgr()
{

}
QcBulletMgr::~QcBulletMgr()
{

}

void QcBulletMgr::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcBulletPtrList& bulletList = GetBulletList();
	bulletList.Animate(dCurrentTime, fElapsedTime);
}

void QcBulletMgr::Render(QcDeviceRender& render)
{
	QcBulletPtrList& bulletList = GetBulletList();
	QcBulletPtrList::iterator iter(bulletList.begin());
	QcBulletPtrList::iterator endIter(bulletList.end());
	for (; iter != endIter; ++iter)
	{
		if ((*iter))
			(*iter)->Render(render);
	}
}

void QcBulletMgr::Add(const QcBullet* bullet)
{
	GetBulletList().Insert(bullet);
}

void QcBulletMgr::Remove(const QcBullet* bullet)
{
	GetBulletList().Erase(bullet);
}