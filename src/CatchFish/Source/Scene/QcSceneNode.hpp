#pragma once

#include "Collision\QcRegion.hpp"
#include "Common\QcObjectEx.hpp"
#include "Scene\QiSceneNodeMonitor.hpp"

class QcDeviceRender;
class QcTexture;
class QcEffect;
class QiRenderState;
class QcSceneNode : public QcObjectEx, public QiSceneNodeMonitor
{
	DeclareRuntimeClass(QcSceneNode);
public:
	QcSceneNode();
	virtual ~QcSceneNode();

	virtual void Init(const QcResImageDesc& desc, bool bLoadSubNode = false);
	virtual void OnTimer(u32 nTimer, void* pContext);
	virtual void Animate(double dCurrentTime,f32 fElapsedTime);
	virtual void Render(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	//virtual void RenderMirror(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	virtual void RenderSub(QcDeviceRender& render, const QcMatrix* pWorldMatrix = NULL);
	virtual bool Intersect(QcSceneNode* pSceneNode);
	virtual bool Intersect(const QcRegion* pRegion);

    inline void SetRenderJustSelf(bool flag){m_bRenderJustSelf = flag;}

	//QiAnimator
	virtual void AddAnimator(QiAnimator* pAnimator);
	virtual void RemoveAnimator(const QiAnimator* pAnimator); //pAnimator == NULL, Clear All
	virtual void FinishAnimator(const QiAnimator* pAnimator);
	void AddAnimator2(QiAnimator* pAnimator);
	QiAnimator* MakeTextureAnimator(QeState eNowState);
	template<class T>
	T* FindAnimator()
	{
		int iPos = FindAnimator<T>(0);
		if (iPos != -1)
			return static_cast<T*>(m_arAnimatorList[iPos].Get());
		return NULL;
	}
	template<class T>
	void RemoveAnimator()
	{
		int iPos = FindAnimator<T>(0);
		if (iPos != -1)
			m_arAnimatorList.EraseAt(iPos);
	}

	//Monitor
	virtual void AddMonitor(const QiSceneNodeMonitor* pMonitor);
	virtual void RemoveMonitor(const QiSceneNodeMonitor* pMonitor);

	//State
	inline QeState GetState() const {return m_eCurrentState;}
	inline void SetState(QeState eState, bool bCallChange = true) 
	{ 
		if (m_eCurrentState != eState && bCallChange)
			StateChange(m_eCurrentState, eState); 
		m_eCurrentState = eState;
	}
	virtual QeState ToShowState(QeState eState) {return eStateNone;}
	virtual void StateChange(QeState eRawState, QeState eNowState);
	virtual bool ToDie(bool bDieTimer = true);
	virtual void ToDead();
	
	//SubSceneNode
	void AddSubSceneNode(const QcSceneNode* pSceneNode);
	void RemoveSubSceneNode(const QcSceneNode* pSceneNode);
	void RemoveSubSceneNodeAt(int index);
	QcSceneNode* GetSubSceneNode(int index);
	QcSceneNode* GetSubSceneNode(const QcObjType& name);

	//Position, scale, rotation.
	inline const QcPoint& GetPosition() const{ return m_vPos;}
	inline void SetPosition(const QcPoint& vPos)
	{ 
		m_vPos = vPos; 
		UpdateRegion(eMaskPos);
	}
	inline f32 GetRotation() const{ return m_fRotate;}
	inline void SetRotation(f32 rotate) 
	{ 
		m_fRotate = rotate;
		UpdateRegion(eMaskRotation);
	}
	inline f32 GetScaleX() const { return m_fScale.X;}
	inline f32 GetScaleY() const { return m_fScale.Y;}
	inline void SetScale(f32 val, QeMatrixMask eMask = eMaskXYZ) 
	{ 
		if (eMask & eMaskX)
			m_fScale.X = val;
		if (eMask & eMaskY)
			m_fScale.Y = val;
		UpdateRegion(eMaskScale);
	}
	inline void ScaleTo(f32 width, f32 height)
	{
		m_fDestWidth = width;
		m_fDestHeight = height;
	}
	inline float GetWidth() const {return m_fWidth;}
	inline float GetHeight() const {return m_fHeight;}
	inline void SetWidth(f32 fWidth){ m_fWidth = fWidth; UpdateRegion(eMaskScale); }
	inline void SetHeight(f32 fHeight){ m_fHeight = fHeight; UpdateRegion(eMaskScale); }
	QcMathNodeInfo GetNodeInfo() const {return QcMathNodeInfo(GetPosition(), GetRotation(), m_fScale.X, m_fScale.Y); }
	void SetNodeInfo(const QcMathNodeInfo& info){ m_vPos = info.GetPos(); m_fRotate = info.GetRotate(); m_fScale.X = info.GetScaleX(); m_fScale.Y = info.GetScaleY(); UpdateRegion(eMaskAll);}

	//Color,Transparency,Texture
	inline void SetCurrentTransparency(u32 alphaValue) { m_color.a = alphaValue/255.f;}
	inline u32 GetCurrentTransparency() const { return u32(m_color.a * 255.f);}
	inline const QcColorf& GetColor() const {return m_color;}
	inline void SetColor(const QcColorf& color){ m_color = color;}
	inline const QcTexture* GetCurrentTexture() const { return m_pTexture; }
	inline void SetCurrentTexture(const QcTexture* pTexture){ m_pTexture = const_cast<QcTexture*>(pTexture);}

	//Util
	inline const QcMatrix& GetMatrix()
	{
		if (m_bMatrixDirty)
		{
			m_matrix.MakeMatrix(m_vPos.X, m_vPos.Y, GetRotation(), GetScaleX(), GetScaleY());
			m_bMatrixDirty = false;
		}
		return m_matrix;
	}
	inline bool IsIntersectScreen()
	{
		if (m_iIntersectScreenFlag == -1)
		{
			CalIntersectScreenFlag();
		}
		return m_iIntersectScreenFlag == 1;
	}
	inline bool IsReady() const {return m_fStartTime < ROUNDING_ERROR_f32; }
	inline bool IsLiving() const {return m_bIsLiving;}	
	inline bool IsVisible() const{ return m_bVisible;}
	inline void SetVisible(bool bVis){ m_bVisible = bVis;}
	inline void SetRender(QcEffect* pEffectRender){ m_pEffectRender = pEffectRender;}
	inline QcEffect* GetRender(){return m_pEffectRender;}
	inline void SetStartTime(f32 fTime) {m_fStartTime = fTime;}
	inline const QcObjType& GetObjType() const {return m_objType;}
	inline const QcResImageDesc* GetDesc() const { return m_pDesc;}
	inline void SetRenderState(QiRenderState* pRenderState) { m_pRenderState = pRenderState; }
	void UpdateFrameRegion(const QcRegion* pRegion);
protected:
	inline const QcRegionAutoPtr& GetRegion() const { return m_pRegion; }
	void CalIntersectScreenFlag();
	template<class T>
	int FindAnimator(int iBeginPos)
	{
		QcAnimatorPtrListIter iter(m_arAnimatorList.begin() + iBeginPos);
		QcAnimatorPtrListIter endIter(m_arAnimatorList.end());
		for (; iter != endIter; ++iter)
		{
			if (*iter && (*iter)->Is<T*>())
			{
				return iter - m_arAnimatorList.begin();
			}
		}
		return -1;
	}
	void UpdateRegion(QeMathNodeInfoMask eMask);
protected:
	QcSceneNodePtrList m_arSubList;
	QcPointerList<QiSceneNodeMonitor> m_arMonitorList;
	QcAnimatorPtrList m_arAnimatorList;

	QcSharedPtr<QiRenderState> m_pRenderState;
	QcSharedPtr<QcEffect> m_pEffectRender;
	QcResImageDesc*	m_pDesc;
	QcRegionAutoPtr m_pRegion;
	QcRegionAutoPtr m_pSourceRegion;
	mutable QcSceneNode* m_pParentNode;
	QcTexture* m_pTexture;

	QcObjType m_objType;
	QcColorf m_color;


	QcPoint m_vPos;
	QcVector m_fScale;
	f32		m_fRotate;

	QeState	m_eCurrentState;
	bool m_bVisible;
	bool m_bIsLiving;
private:
	f32		   m_fStartTime;
	f32		   m_fWidth;
	f32		   m_fHeight;
	f32			m_fDestWidth;
	f32			m_fDestHeight;
	f32		   m_fHalfMaxLen;
	QcMatrix   m_matrix;
	bool	   m_bMatrixDirty;
	int		   m_iIntersectScreenFlag;

    bool        m_bRenderJustSelf;
};