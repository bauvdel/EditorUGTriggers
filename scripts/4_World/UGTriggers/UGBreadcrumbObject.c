class UGBreadcrumb : Building
{
    protected float m_UG_EyeAccommodation = 1.0;
    protected int   m_UG_UseRaycast = 0;
    protected float m_UG_Radius = -1.0;
    protected bool  m_UG_LightLerp = false;
    protected ref Timer m_BCWatch;
    protected vector    m_LastPos;
    protected float m_NativeRadius;

    private static ref array<UGBreadcrumb> s_AllBreadcrumbs = new array<UGBreadcrumb>();

    static array<UGBreadcrumb> GetAll() { return s_AllBreadcrumbs; }

    void UGBreadcrumb()
    {
        m_BCWatch = null;

        s_AllBreadcrumbs.Insert(this);

        m_LastPos = GetPosition();
        m_BCWatch = new Timer(CALL_CATEGORY_SYSTEM);
        m_BCWatch.Run(0.10, this, "BC_Poll", null, true);

        vector minMax[2];
        m_NativeRadius = ClippingInfo(minMax);
        if (m_NativeRadius <= 0)
            m_NativeRadius = 1.0;

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.UpdateVisualScale, 10, false);
    }
    void ~UGBreadcrumb()
    {
        int idx = s_AllBreadcrumbs.Find(this);
        if (idx >= 0) s_AllBreadcrumbs.Remove(idx);
        if (m_BCWatch) m_BCWatch.Stop();
    }

    void BC_Poll()
    {
    vector p = GetPosition();
    if (vector.Distance(p, m_LastPos) > 0.001) 
        {
        m_LastPos = p;
        UG_RescanTriggersAround(p, 200.0);
        }
    }

    void SetEyeAccommodation(float v)
    {
        if (v < 0.0) v = 0.0;
        if (v > 1.0) v = 1.0;
        m_UG_EyeAccommodation = UG_Round2(v);
        UG_RescanTriggersAround(GetPosition(), 200.0);
    }
    float GetEyeAccommodation() { return m_UG_EyeAccommodation; }

    void SetUseRaycast(int v) 
    { 
        m_UG_UseRaycast = v != 0;
        UG_RescanTriggersAround(GetPosition(), 200.0); 
    }
    int  GetUseRaycast() { return m_UG_UseRaycast; }

    void SetRadius(float r)
    {
        m_UG_Radius = r;
        UpdateVisualScale();
        UG_RescanTriggersAround(GetPosition(), 200.0);
    }
    
    float GetRadius() { return m_UG_Radius; }

    void UpdateVisualScale()
    {
        float effectiveRadius;

        if (m_UG_Radius == -1.0)
            effectiveRadius = 5.0;
        else
            effectiveRadius = m_UG_Radius;

        if (effectiveRadius <= 0)
            effectiveRadius = 0.1;

        SetScale(effectiveRadius / m_NativeRadius);
    }

    void SetLightLerp(bool value)
    {
        m_UG_LightLerp = value;
        UG_RescanTriggersAround(GetPosition(), 200.0);
    }
    bool GetLightLerp() { return m_UG_LightLerp; }

    float GetNativeRadius() { return m_NativeRadius; }
};