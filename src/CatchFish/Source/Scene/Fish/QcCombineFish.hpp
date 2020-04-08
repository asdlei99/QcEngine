#pragma once

#include "Scene\Fish\QcFish.hpp"

class QcCombineFish : public QcFish
{
	DeclareRuntimeClass(QcCombineFish);
public:
	QcCombineFish();

	virtual void Init(const QcResImageDesc& desc, bool = false);
};