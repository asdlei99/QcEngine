#pragma once

class QcSceneNode;

typedef boost::function<void(u32)> QcAnimatorCb;
typedef QcAnimatorCb QcFinishAnimatorCb;

class QiAnimator : public QcObject
{
	struct QsCallBackInfo
	{
		QsCallBackInfo(const QcFinishAnimatorCb& cb, u32 iParam) : m_cb(cb), m_iParam(iParam) {}

		QcFinishAnimatorCb m_cb;
		u32 m_iParam;
	};
	friend class QiAnimatorDeque;
public:
	QiAnimator() 
		: m_bFinished(false)
		, m_bSuspend(false)
		, m_fStartTime(0.f)
		,m_fLivingTime(0.f)
		,m_iCurrentFrame(0)
		,m_bInDeque(false)
	{
	}
	void Animate(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime)
	{
		if (pSceneNode && !m_bSuspend)
		{
			if (IsReady())
			{
				AnimateNode(pSceneNode, dCurrentTime, fElapsedTime);
				m_iCurrentFrame = giFrameID;
				if (m_fLivingTime > ROUNDING_ERROR_f32)
				{
					m_bFinished = false;
					m_fLivingTime -= fElapsedTime;
					if (m_fLivingTime < ROUNDING_ERROR_f32)
					{
						m_bFinished = true;
					}
				}
				if (m_bFinished)
				{
					DoFinishedAction(pSceneNode);
				}
			}
			else
			{
				if (!QcMath::equals(m_iCurrentFrame, giFrameID))
				{
					m_fStartTime -= fElapsedTime;
					m_iCurrentFrame = giFrameID;
                    if (IsReady())
                        DoStartAction();
				}
			}
		}
	}
	void Reset()
	{
		m_iCurrentFrame = 0;
		m_fLivingTime = 0.f;
		m_fStartTime = 0.f;
		m_bSuspend = false;
		m_bFinished = false;
		m_bInDeque = false;
        m_pStartCallBackerList.clear();
        m_pSuspendCallBackerList.clear();
        m_pResumeCallBackerList.clear();
		m_pFinishCallBackerList.clear();
	}
	virtual void AddToSceneNode(QcSceneNode* pSceneNode){}
	virtual bool IsMoveAnimator() const {return false;}
	virtual bool IsTextureAnimator() const {return false;}
	virtual bool IsAnimatorDeque() const {return false;}

    void AddStartCallBack(const QcFinishAnimatorCb& cb, u32 iParam = 0) { m_pStartCallBackerList.push_back(QsCallBackInfo(cb, iParam));}
    void AddSuspendCallBack(const QcFinishAnimatorCb& cb, u32 iParam = 0) { m_pSuspendCallBackerList.push_back(QsCallBackInfo(cb, iParam));}
    void AddResumeCallBack(const QcFinishAnimatorCb& cb, u32 iParam = 0) { m_pResumeCallBackerList.push_back(QsCallBackInfo(cb, iParam));}
    void AddFinishCallBack(const QcFinishAnimatorCb& cb, u32 iParam = 0) { m_pFinishCallBackerList.push_back(QsCallBackInfo(cb, iParam));}

	bool IsFinished(void) const{ return m_bFinished;}
	bool IsReady() const {return m_fStartTime < ROUNDING_ERROR_f32;}
	bool IsSuspend() const {return m_bSuspend; }
	void Suspend(bool bSuspend);

	void SetLivingTime(f32 fLivingTime) { m_fLivingTime = abs(fLivingTime);}
	void SetStartTime(f32 RelativeTime) { m_fStartTime = RelativeTime; }
protected:
    void DoStartAction();
    void DoFinishedAction(QcSceneNode* pSceneNode);
	virtual void AnimateNode(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime) = 0;
protected:
    std::vector<QsCallBackInfo> m_pStartCallBackerList;
    std::vector<QsCallBackInfo> m_pSuspendCallBackerList;
    std::vector<QsCallBackInfo> m_pResumeCallBackerList;
    std::vector<QsCallBackInfo> m_pFinishCallBackerList;
	u64 m_iCurrentFrame;		//用于一个QiAnimator控制多个SceneNode
	bool m_bFinished;
	bool m_bSuspend;
	f32  m_fStartTime;
	f32  m_fLivingTime;
	bool m_bInDeque;
};