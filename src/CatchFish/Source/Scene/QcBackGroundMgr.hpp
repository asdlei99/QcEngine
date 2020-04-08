#pragma once

class QcBackGround;
class QcSceneLogo;
class QcAnimatorDither;
class QcBackGroundMgr : public QcObject
{
public:
	explicit QcBackGroundMgr();
	virtual ~QcBackGroundMgr();

	bool Init(QcResManager* pResManager);
	void Animate(double dCurrentTime,f32 fElapsedTime);
	void Render(QcDeviceRender& render);
    void RenderWave(QcDeviceRender& render);
	void RenderBgSceneNode(QcDeviceRender& render);
	bool RenderLogoAnimation(QcDeviceRender& render);

    void WaveFinishCallBack(u32 wavePtr);

	void StartLogoAnimation();
	void NextScene(bool bStartGame = false);
	void Dither(f32 fShakeTime, f32 fShakeScope = 4.0f);

	u32 GetSenceId() const {return m_iCurrent;}
	QcBackGround* GetCurrentBg() {return GetBackGound(m_iCurrent); }

    bool IsChanging() {return m_bChanging;}
private:
	QcBackGround* GetBackGound(u32 idx)
	{
		return m_arSceneNodeList[idx];
	}
	QcBackGround* GetCurrentMirrorBg() {return GetMirrorBg(m_iCurrent);}
	QcBackGround* GetMirrorBg(u32 idx)
	{
		if (m_arMirrorBgList.size())
			return m_arMirrorBgList[idx];
		return NULL;
	}
	u32 GetNext()
	{
		if (m_iCurrent == m_arSceneNodeList.size() -1)
			return 0;
		return m_iCurrent + 1;
	}
	void MakeWave(int iScene);
private:
	QcSceneNodePointerList<QcBackGround> m_arSceneNodeList;
	QcSceneNodePointerList<QcBackGround> m_arMirrorBgList;
	QcSharedPtr<QcSceneLogo> m_oSceneLogo;
	QcSharedPtr<QcAnimatorDither> m_pDitherAnimator;
    
    QcSceneNodePtr m_pWave;
	u32 m_iCurrent;
	bool m_bChanging;
};