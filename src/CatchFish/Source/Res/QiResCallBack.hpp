#pragma once

class QcResDesc;
class QiResCallBack
{
public:
	virtual void CallBack(const QcResDesc* pDesc) = 0;
};