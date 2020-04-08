#pragma once

#include "Common\QcList.hpp"
class QcDeviceRender;
template<class T>
class QcSceneNodePointerList : public QcSharedPointerList<T>
{
public:
	void Animate(double dCurrentTime,f32 fElapsedTime)
	{
		if (size())
		{
			Tracker track(*this);
			QcSceneNodePointerList<T>::iterator iter(begin());
			QcSceneNodePointerList<T>::iterator endIter(end());
			for (; iter != endIter; ++iter)
			{
				if (*iter)
					(*iter)->Animate(dCurrentTime, fElapsedTime);
			}
		}
	}
	void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL)
	{
		QcSceneNodePointerList<T>::iterator iter(begin());
		QcSceneNodePointerList<T>::iterator endIter(end());
		for (; iter != endIter; ++iter)
		{
			if (*iter)
				(*iter)->Render(render, pWorldMatrix);
		}
	}
};