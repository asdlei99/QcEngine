#pragma once

class QcSceneAnimationDeque : public QcObject
{
public:
	void AddAnimation(const QcSceneAnimationPtr& animation);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render);
	size_t Size() const {return m_arAnimation.size();}
	const std::deque<QcSceneAnimationPtr>& GetAnimationDeque() const {return m_arAnimation; }
private:
	std::deque<QcSceneAnimationPtr> m_arAnimation;
};