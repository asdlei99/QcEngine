#include "pch.hpp"
#include "QcSetting.hpp"
#include "QcButtonWidget.hpp"

ImplRuntimeClass(QcButtonWidget);

QcButtonWidget::QcButtonWidget()
{

}

bool QcButtonWidget::OnOkKey(const QsKeyInfo& info)
{
	if (m_cb)
		m_cb();
	return true;
}