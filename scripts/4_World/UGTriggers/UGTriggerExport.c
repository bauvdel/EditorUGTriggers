class UGTriggersExport
{
	ref array<float> Position;
	ref array<float> Orientation;
	ref array<float> Size;
	float            EyeAccommodation;
	float            InterpolationSpeed;
	int              UseLinePointFade;
	string           AmbientSoundType;
	string           AmbientSoundSet;
	ref array<ref UGBreadcrumbExport> Breadcrumbs;

	void UGTriggersExport(vector pos, vector orient, vector size, float acc, float interp)
	{
		if (size[0] < 1) size[0] = 1;
		if (size[1] < 1) size[1] = 1;
		if (size[2] < 1) size[2] = 1;

		Position = new array<float>();
		Position.Insert(pos[0]);    Position.Insert(pos[1]);    Position.Insert(pos[2]);

		Orientation = new array<float>();
		Orientation.Insert(orient[0]); Orientation.Insert(orient[1]); Orientation.Insert(orient[2]);

		Size = new array<float>();
		Size.Insert(size[0]); Size.Insert(size[1]); Size.Insert(size[2]);

		EyeAccommodation   = Math.Clamp(acc, 0.0, 1.0);
		InterpolationSpeed = Math.Clamp(interp, 0.0, 1.0);
		UseLinePointFade   = 0;
		AmbientSoundType   = "";
		AmbientSoundSet    = "";
	}
}

class UGBreadcrumbExport
{
	ref array<float> Position;
	float EyeAccommodation;
	int   UseRaycast;
	float Radius;
	int   LightLerp;
}
class UGTriggersExportRoot
{
    ref array<ref UGTriggersExport> Triggers = new array<ref UGTriggersExport>();
}

static JsonUndergroundAreaTriggerData BuildJsonFromUG(UGTriggerObject ug)
{
    if (!ug) return null;

    UndergroundTrigger t = ug.GetLinkedTrigger();

    vector pos    = ug.GetPosition();
    vector orient = ug.GetOrientation();
    vector size   = ug.GetSize();

    float acc = 1.0;
    float interp = 1.0;
    if (t) {
        acc    = t.m_Accommodation;
        interp = t.m_InterpolationSpeed;
    }

    JsonUndergroundAreaTriggerData d = new JsonUndergroundAreaTriggerData();

    // Position
    d.Position = new array<float>();
    d.Position.Insert(pos[0]);
    d.Position.Insert(pos[1]);
    d.Position.Insert(pos[2]);

    // Orientation
    d.Orientation = new array<float>();
    d.Orientation.Insert(orient[0]);
    d.Orientation.Insert(orient[1]);
    d.Orientation.Insert(orient[2]);

    // Size
    d.Size = new array<float>();
    d.Size.Insert(size[0]);
    d.Size.Insert(size[1]);
    d.Size.Insert(size[2]);

    //Darkness
    d.EyeAccommodation = acc;
    d.InterpolationSpeed = interp;

    // Ambient sound properties
    d.UseLinePointFade = ug.GetUseLinePointFade();
    d.AmbientSoundType = ug.GetAmbientSoundType();
    d.AmbientSoundSet = ug.GetAmbientSoundSet();

    return d;
}