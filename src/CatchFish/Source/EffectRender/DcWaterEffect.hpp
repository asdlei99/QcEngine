#ifndef DcWaterEffect_hpp__
#define DcWaterEffect_hpp__
#include "EffectRender/QcEffect.hpp"

class DcWaterEffect : public QcEffect
{
    DeclareRuntimeClass(DcWaterEffect);

public:
    DcWaterEffect();
    ~DcWaterEffect();

    void Init(ID3DXEffect* pEffect);
    void OnAnimate(QcSceneNode* pSceneNode, double dCurrentTime,f32 fElapsedTime);
    bool OnRender(QcDeviceRender& render, const QcTexture* pTexture);

    bool SetWaterPos(const D3DXVECTOR3 &pos);
    // 水波扩散时，计算高度所用的更新时间，，并根据此时间决定更新次数
    bool SetUpdateTick(float fTick);
    // 本次渲染所需要持续的时间，每次加新的点的时候，计时器按此数值重置，计时器归零则重置高度图
    bool SetRenderTime(float fTime);
    // 在到达更新时间后，批量增加新的点的时候，新增的点均按此数值设定水波力量的初始值，取值范围是0~1的左开右闭区间
    bool SetWaterForce(float fForce);
    // 水波的阻尼，会影响当前所有未完成的水波，所以如果需要更新，最好等全部完成再调用。
    // 此值越大阻尼越小，取值范围是0~1的开区间
    bool SetWaterDamp(float fDamp);

private:
    enum RtType
    {
        TEX_HEIGHT0 = 0,
        TEX_HEIGHT1,//
        TEX_HEIGHT2,
        TEX_NORMAL,
        TEX_MAx,
    };

    static DcWaterEffect    *m_pInstance;

    LPDIRECT3DTEXTURE9       m_pTarget[TEX_MAx];
    LPDIRECT3DTEXTURE9       m_pBackBuffer[2];

    int                      m_iRef;

    vector<D3DXVECTOR3>      m_vecPos;
    float                    m_fRenderTime;
    float                    m_fUpdateTime;
    u64                   m_iCurTime;

    float                    m_fRenderPlan;
    float                    m_fUpdateTick;
    float                    m_fDamp;
    float                    m_fForce;

    // 接触点
    void AddImpulse(LPDIRECT3DDEVICE9 pDevice);
    // 下一次的高度
    void WaterSimulation(LPDIRECT3DDEVICE9 pDevice);
    // 法线
    void ConvertNormal(LPDIRECT3DDEVICE9 pDevice);
    // 背景
    void Water(LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DSURFACE9 pBackBuf);
    // 交换
    void SwapHeightmaps();
};

#endif // DcWaterEffect_hpp__