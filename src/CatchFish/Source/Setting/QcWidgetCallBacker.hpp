#pragma once

class QcWidgetCallBacker
{
public:
	QcWidgetCallBacker(){}
	QcWidgetCallBacker(const QcFunDialogCb& cb): m_pDlgFunCb(cb){}
	QcWidgetCallBacker(const QcFunBtnCb& cb): m_pFunBtnCb(cb){}
	QcWidgetCallBacker(const QcFunDigitalCb& cb): m_pFunDigitalCb(cb){}
	QcWidgetCallBacker(const QcFunInputCb& cb): m_pFunInputCb(cb){}
	QcWidgetCallBacker(const QcFunInputCheckCb& cb) : m_pFunInputCheckCb(cb) {}

	operator bool()
	{
		return m_pDlgFunCb || m_pFunBtnCb || m_pFunDigitalCb || m_pFunInputCb || m_pFunInputCheckCb;
	}
	void operator()(int iFlag) const
	{
		m_pDlgFunCb(iFlag);
	}
	void operator()() const
	{
		m_pFunBtnCb();
	}
	void operator()(s32& iValue, bool bUpdate) const
	{
		m_pFunDigitalCb(iValue, bUpdate);
	}
	void operator()(BYTE* iValue,s32& nBit,bool bUpdate) const
	{
		m_pFunInputCb(iValue, nBit, bUpdate);
	}
	s32 operator()(BYTE* pValue,s32 nBit) const
	{
		return m_pFunInputCheckCb(pValue, nBit);
	}
protected:
	QcFunDialogCb m_pDlgFunCb;
	QcFunBtnCb m_pFunBtnCb;
	QcFunDigitalCb m_pFunDigitalCb;
	QcFunInputCb m_pFunInputCb;
	QcFunInputCheckCb m_pFunInputCheckCb;
};