#pragma once

#include "Scene\Fish\QcFish.hpp"
class QcFrozenFish : public QcFish
{
	DeclareRuntimeClass(QcFrozenFish);
public:
	QcFrozenFish();
	
	virtual bool ToDie(bool bDieTimer = true);
};

