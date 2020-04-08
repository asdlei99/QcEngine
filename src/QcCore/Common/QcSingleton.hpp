#pragma once

//限制，不能在单件实例的构造函数里调用GetInstance获取单件指针，否则造成死循环。
template<class T>
class QcSingleton
{
friend typename T;
private:
	QmDisableCopyAssignment(QcSingleton);
	QcSingleton(){}
	~QcSingleton(){}
public:
	static T* GetInstance()
	{
		static QcSharedPtr<T> g_Instance;
		if (!g_Instance)
		{
			static volatile long lock = 0;
			if (InterlockedCompareExchange(&lock, 1, 0) == 0)
			{
				g_Instance.Set(new T());
				lock = 2;
			}
			else
			{
				while(lock != 2) SwitchToThread();
			}
		}
		return g_Instance.Get();
	}
};


template<class T>
T* GetInstance()
{
	return T::GetInstance();
}