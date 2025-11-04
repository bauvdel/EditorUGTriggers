class UGBreadcrumbController: Managed
{
	protected UGBreadcrumb m_UGBreadcrumb;

	float BC_EyeAccommodation = 1.0;
	int BC_UseRaycast = 0;
	float BC_Radius = -1.0;
	int BC_LightLerp = 0;

	void UGBreadcrumbController(UGBreadcrumb bc_object)
	{
		m_UGBreadcrumb = bc_object;

		// Initialize values from the object
		BC_EyeAccommodation = bc_object.GetEyeAccommodation();
		BC_UseRaycast = bc_object.GetUseRaycast();
		BC_Radius = bc_object.GetRadius();
		if (bc_object.GetLightLerp())
			BC_LightLerp = 1;
		else
			BC_LightLerp = 0;
	}

	void PropertyChanged(string property_name)
	{
		if (!m_UGBreadcrumb) return;

		switch (property_name) {
			case "BC_EyeAccommodation": {
				if (BC_EyeAccommodation < 0.0) BC_EyeAccommodation = 0.0;
				if (BC_EyeAccommodation > 1.0) BC_EyeAccommodation = 1.0;
				m_UGBreadcrumb.SetEyeAccommodation(BC_EyeAccommodation);
				// Force update to ensure data is saved
				m_UGBreadcrumb.Update();
				break;
			}

			case "BC_UseRaycast": {
				m_UGBreadcrumb.SetUseRaycast(BC_UseRaycast);
				// Force update to ensure data is saved
				m_UGBreadcrumb.Update();
				break;
			}

			case "BC_Radius": {
				m_UGBreadcrumb.SetRadius(BC_Radius);
				// Force update to ensure data is saved
				m_UGBreadcrumb.Update();
				break;
			}

			case "BC_LightLerp": {
				m_UGBreadcrumb.SetLightLerp(BC_LightLerp != 0);
				// Force update to ensure data is saved
				m_UGBreadcrumb.Update();
				break;
			}
		}
	}
}