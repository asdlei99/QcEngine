#pragma once

#include "Common\QcEncryptXmlLoader.hpp"
class QcAppConfig : public QcEncryptXmlLoader
{
public:
	QcAppConfig();

	virtual void Parse(const QcXmlNodeIterator& pRootNode, void* pParam);

	bool IsFullScreen() const {return m_bFullScreen;}
	bool IsImmediatelPresent() const {return m_bImmediatelPresent;}
	const QcString& GetMonitorName() const {return m_sMonitorName;}
private:
	bool IsHaveEDID () const;
	bool SearchBestConfig(f32& width, f32& height);
private:
	QcString m_sMonitorName;
	bool m_bFullScreen;
	bool m_bImmediatelPresent;
};