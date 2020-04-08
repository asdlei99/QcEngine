#pragma once

class QcResManager;
class QcSceneStartMgr;
class QcSceneMgr;
class QcAppConfig;
class QcFileStorageMgr;
class QcScene : public QcObject
{
public:
	explicit QcScene();
	virtual ~QcScene();

	virtual bool InitApp(QcAppConfig& appConfig);
	virtual bool ExitApp();
	virtual bool LoadResouce();
	virtual bool OnCreateDevice(IDirect3DDevice9* pd3dDevice);
	virtual bool OnDestroyDevice();
	virtual bool OnResetDevice(IDirect3DDevice9* pd3dDevice);
	virtual bool OnLostDevice();
	virtual void OnKeyDown(u32 key, const QsKeyInfo& info);
	virtual void OnKeyUp(u32 key, const QsKeyInfo& info);

	virtual bool InitScene();
	virtual bool StartFrame(f64 fTime, f32 fElapsedTime);
	virtual void HandleCollision();
	virtual bool Draw(float fElapsedTime);

	inline QcResManager* GetResMgr()
	{
		return m_pResMgr.Get();
	}
	inline QcSceneMgr* GetSceneMgr()
	{
		return m_pSceneMgr.Get();
	}
	inline QcFileStorageMgr* GetGameFileMgr()
	{
		return m_pGameFileMgr.Get();
	}
	QcDeviceRender& GetRender() {return m_render;}
	void RecoveryNode(const QcSceneNode* pNode)
	{
		//QmAssert(pNode->GetRefCount() > 0);
		m_arRecoveryNode[giScreenHandleing].PushBack(pNode);
	}
protected:
	void RecoveryNode()
	{
		for (int i=0; i<QmMaxScreen; ++i)
		{
			QmAutoRevertInit(s32, giScreenHandleing, i);
			m_arRecoveryNode[i].clear();
		}
	}
private:
	QcSceneNodePtrList m_arRecoveryNode[QmMaxScreen];
	QcDeviceRender m_render;
	QcSharedPtr<QcSceneStartMgr> m_pStartMgr;
	QcSharedPtr<QcSceneMgr> m_pSceneMgr;
	QcSharedPtr<QcResManager> m_pResMgr;
	QcSharedPtr<QcFileStorageMgr> m_pGameFileMgr;
};