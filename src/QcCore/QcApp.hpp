#pragma once

class QcApp
{
public:
	QcApp();
	template<class T>
	static T* AppInstance()
	{
		return static_cast<T*>(gsApp);
	}
	virtual void ExitApp(bool bTerminateProcess = true){}
protected:
	static QcApp* gsApp;
};

#define QmAppInstance(Class) QcApp::AppInstance<Class>()
#define QmApp QmAppInstance(QcApp)

extern void SetLogPath(const char* psPath);
extern const char* GetLogPath();
#define QmSetLogPath(str) SetLogPath(str);
#define QmLogPath GetLogPath()

