#include "pch.hpp"
#include "QcGravityFish.hpp"

ImplRuntimeClass(QcGravityFish);

QcGravityFish::QcGravityFish()
{

}

void QcGravityFish::Animate(double dCurrentTime,f32 fElapsedTime)
{
	QcFish::Animate(dCurrentTime, fElapsedTime);
}

bool QcGravityFish::ToDie(bool bDieTimer)
{
	return QcFish::ToDie(bDieTimer);
}


