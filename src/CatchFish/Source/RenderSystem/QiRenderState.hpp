#pragma once

struct QsRenderState
{
	u32 type;
	u32 value;
	u32 iBackupValue;
};

struct QsTexState
{
	u32 iTex;
	u32 type;
	u32 value;
	u32 iBackupValue;
};

class QiRenderState : public QcObject
{
public:
	enum QeFlag
	{
		eNormal,
		eOnlySelf,
	};
	QiRenderState() : m_eRenderFlag(eNormal){}
	virtual void BeforeRender(QcDeviceRender& render) = 0;
	virtual void EndRender(QcDeviceRender& render) = 0;
	QeFlag GetRenderFlag() const {return m_eRenderFlag; }
	void SetRenderFlag(QiRenderState::QeFlag eFlag){m_eRenderFlag = eFlag;}
protected:
	QeFlag m_eRenderFlag; 
};