modded class UndergroundTrigger
{
	void SetAccommodation(float v)
    {
        m_Accommodation = v;
    }
	void SetInterpolationSpeed(float v)
    {
        m_InterpolationSpeed = v;
    }
    void SetExtentsFromSize(vector size)
	{
		vector e = size * 0.5;
		SetExtents(-e, e);
	}

    //Override the existing trigger enter and leave events to call the client handler because the Server Handler was set to only run in dev mode, but we need it.
	override protected void OnEnterServerEvent(TriggerInsider insider)
	{
		OnEnterClientEvent(insider);
	}
	override protected void OnLeaveServerEvent(TriggerInsider insider)
	{
		OnLeaveClientEvent(insider);
	}

	override protected void OnEnterClientEvent(TriggerInsider insider)
	{
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		if (player)
		{
			// Ensure m_Data and Breadcrumbs exist before handler processes trigger
			if (!m_Data)
				m_Data = new JsonUndergroundAreaTriggerData();

			if (!m_Data.Breadcrumbs)
				m_Data.Breadcrumbs = new array<ref JsonUndergroundAreaBreadcrumb>();

			UndergroundHandlerClient handler = player.GetUndergroundHandler();
			if (handler)
				handler.OnTriggerEnter(this);
		}
	}

	override protected void OnLeaveClientEvent(TriggerInsider insider)
	{
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		if (player)
		{
			UndergroundHandlerClient handler = player.GetUndergroundHandler();
			if (handler)
				handler.OnTriggerLeave(this);
		}
	}

    void SetTypeForAccommodation()
    {
    if (m_Accommodation == 1)
        {
            m_Type = EUndergroundTriggerType.OUTER;
        }
    else
        {
            m_Type = EUndergroundTriggerType.INNER;
        }
    }

}