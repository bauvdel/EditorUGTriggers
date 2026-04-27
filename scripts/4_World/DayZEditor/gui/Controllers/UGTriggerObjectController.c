class UGTriggerObjectController: Managed
{
	protected UGTriggerObject m_UGTriggerObject;

	vector UG_SizeVec;
	float UG_EyeAccommodation = 1.0;
	float UG_Interpolation = 1.0;
	int UG_Type = 0;
	int UG_LastType = -1;
	int UG_UseLinePointFade = 0;
	int UG_AmbientSoundType = 0;
	string UG_AmbientSoundSet = "";

	void UGTriggerObjectController(UGTriggerObject ug_object)
	{
		m_UGTriggerObject = ug_object;

		// Initialize values from the object
		UG_SizeVec = ug_object.GetSize();
		UG_EyeAccommodation = ug_object.GetEyeAccommodation();
		UG_Interpolation = ug_object.GetInterpolation();
		UG_Type = ug_object.GetUGType();
		UG_LastType = UG_Type;
		if (ug_object.GetUseLinePointFade())
			UG_UseLinePointFade = 1;
		else
			UG_UseLinePointFade = 0;
		UG_AmbientSoundType = UGTriggerValidator.GetAmbientSoundTypeFromString(ug_object.GetAmbientSoundType());
		UG_AmbientSoundSet = ug_object.GetAmbientSoundSet();
	}

	void PropertyChanged(string property_name)
	{
		if (!m_UGTriggerObject) return;

		UndergroundTrigger trig = m_UGTriggerObject.GetLinkedTrigger();

		switch (property_name) {
			case "UG_SizeVec": {
				UG_SizeVec = UG_ClampSizeVec(UG_SizeVec);
				m_UGTriggerObject.SetSize(UG_SizeVec);
				// Force immediate update
				m_UGTriggerObject.Update();
				break;
			}

			case "UG_Type": {
				if (UG_LastType != UG_Type) {
					if (UG_Type == 0) UG_EyeAccommodation = 1.00;
					else UG_EyeAccommodation = 0.00;
					UG_LastType = UG_Type;
				}
				m_UGTriggerObject.SetUGType(UG_Type);
				m_UGTriggerObject.SetEyeAccommodation(Math.Round(Math.Clamp(UG_EyeAccommodation, 0.0, 1.0) * 100.0) / 100.0);
				// Also update the linked UndergroundTrigger for export
				if (trig) {
					trig.m_Accommodation = UG_EyeAccommodation;
				}
				break;
			}

			case "UG_EyeAccommodation": {
				UG_EyeAccommodation = Math.Clamp(Math.Round(UG_EyeAccommodation * 100.0) / 100.0, 0.0, 1.0);
				m_UGTriggerObject.SetEyeAccommodation(UG_EyeAccommodation);
				// Also update the linked UndergroundTrigger for export
				if (trig) {
					trig.m_Accommodation = UG_EyeAccommodation;
				}
				break;
			}

			case "UG_Interpolation": {
				UG_Interpolation = Math.Clamp(UG_Interpolation, 0.0, 1.0);
				m_UGTriggerObject.SetInterpolation(UG_Interpolation);
				// Also update the linked UndergroundTrigger for export
				if (trig) {
					trig.m_InterpolationSpeed = UG_Interpolation;
				}
				break;
			}

			case "UG_UseLinePointFade": {
				m_UGTriggerObject.SetUseLinePointFade(UG_UseLinePointFade != 0);
				m_UGTriggerObject.Update();
				break;
			}

			case "UG_AmbientSoundType": {
				string soundTypeStr = UGTriggerValidator.GetAmbientSoundTypeString(UG_AmbientSoundType);
				m_UGTriggerObject.SetAmbientSoundType(soundTypeStr);
				m_UGTriggerObject.Update();
				break;
			}

			case "UG_AmbientSoundSet": {
				m_UGTriggerObject.SetAmbientSoundSet(UG_AmbientSoundSet);
				m_UGTriggerObject.Update();
				break;
			}
		}
	}

	protected vector UG_ClampSizeVec(vector v)
	{
		if (v[0] < 1.0) v[0] = 1.0;
		if (v[1] < 1.0) v[1] = 1.0;
		if (v[2] < 1.0) v[2] = 1.0;
		return v;
	}
}