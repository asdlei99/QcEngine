#pragma once

#include "Setting\QcWidget.hpp"
class QcButtonWidget : public QcWidget
{
	DeclareRuntimeClass(QcButtonWidget);
public:
	QcButtonWidget();

	virtual bool OnOkKey(const QsKeyInfo& info);
};