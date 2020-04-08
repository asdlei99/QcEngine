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
    // ˮ����ɢʱ������߶����õĸ���ʱ�䣬�������ݴ�ʱ��������´���
    bool SetUpdateTick(float fTick);
    // ������Ⱦ����Ҫ������ʱ�䣬ÿ�μ��µĵ��ʱ�򣬼�ʱ��������ֵ���ã���ʱ�����������ø߶�ͼ
    bool SetRenderTime(float fTime);
    // �ڵ������ʱ������������µĵ��ʱ�������ĵ��������ֵ�趨ˮ�������ĳ�ʼֵ��ȡֵ��Χ��0~1�����ұ�����
    bool SetWaterForce(float fForce);
    // ˮ�������ᣬ��Ӱ�쵱ǰ����δ��ɵ�ˮ�������������Ҫ���£���õ�ȫ������ٵ��á�
    // ��ֵԽ������ԽС��ȡֵ��Χ��0~1�Ŀ�����
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

    // �Ӵ���
    void AddImpulse(LPDIRECT3DDEVICE9 pDevice);
    // ��һ�εĸ߶�
    void WaterSimulation(LPDIRECT3DDEVICE9 pDevice);
    // ����
    void ConvertNormal(LPDIRECT3DDEVICE9 pDevice);
    // ����
    void Water(LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DSURFACE9 pBackBuf);
    // ����
    void SwapHeightmaps();
};

#endif // DcWaterEffect_hpp__