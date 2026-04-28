class UGPropSnapshot
{
	int    Id;
	string Type;
	vector Pos;
	vector Ori;
	// Trigger
	vector Size;
	int    UGType;
	float  Eye;
	float  Interp;
	bool   UseLinePointFade;
	string AmbientSoundType;
	string AmbientSoundSet;
	// Breadcrumb
	int    IsBC;
	float  BC_Eye;
	int    BC_Ray;
	float  BC_Rad;
	bool   BC_LightLerp;

	float  ExpireAt;
}

class UGUndoCache
{
	protected static ref array<ref UGPropSnapshot> s_Snaps = new array<ref UGPropSnapshot>();

	static void RememberEO(EditorObject eo, float ttl = 90.0)
	{
		if (!eo) return;
		Object w = eo.GetWorldObject();
		if (!w) return;

		UGTriggerObject ug = UGTriggerObject.Cast(w);
		if (ug) {
			UGPropSnapshot s = new UGPropSnapshot();
			s.Id     = eo.GetID();
			s.Type   = ug.GetType();
			s.Pos    = ug.GetPosition();
			s.Ori    = ug.GetOrientation();
			s.Size   = ug.GetSize();
			s.UGType = ug.GetUGType();
			s.Eye    = ug.GetEyeAccommodation();
			s.Interp = ug.GetInterpolation();
			s.UseLinePointFade = ug.GetUseLinePointFade();
			s.AmbientSoundType = ug.GetAmbientSoundType();
			s.AmbientSoundSet  = ug.GetAmbientSoundSet();
			s.IsBC   = 0;
			s.ExpireAt = UGEditorGameCache.GetCachedTime() + ttl;
			s_Snaps.Insert(s);
			return;
		}

		UGBreadcrumb bc = UGBreadcrumb.Cast(w);
		if (bc) {
			UGPropSnapshot s2 = new UGPropSnapshot();
			s2.Id     = eo.GetID();
			s2.Type   = bc.GetType();
			s2.Pos    = bc.GetPosition();
			s2.Ori    = bc.GetOrientation();
			s2.IsBC   = 1;
			s2.BC_Eye = bc.GetEyeAccommodation();
			s2.BC_Ray = bc.GetUseRaycast();
			s2.BC_Rad = bc.GetRadius();
			s2.BC_LightLerp = bc.GetLightLerp();
			s2.ExpireAt = UGEditorGameCache.GetCachedTime() + ttl;
			s_Snaps.Insert(s2);
		}
	}
	//There is almost certaintly a better way to do this, and im sure someone will tell me I'm an idiot in the comments.
	static void Rehydrate(Editor editor)
	{
		if (!editor || s_Snaps.Count() == 0) return;

		float now = UGEditorGameCache.GetCachedTime();

		// Purge expired
		for (int i = s_Snaps.Count() - 1; i >= 0; i--) {
			if (s_Snaps[i].ExpireAt <= now) s_Snaps.Remove(i);
		}
		if (s_Snaps.Count() == 0) return;

		// Try to rehydrate by ID
		for (int j = s_Snaps.Count() - 1; j >= 0; j--) {
			UGPropSnapshot s = s_Snaps[j];

			EditorObject eo = editor.GetPlacedObjectById(s.Id);
			if (!eo) continue; // not recreated yet

			Object w = eo.GetWorldObject();
			if (!w) { s_Snaps.Remove(j); continue; }

			UGTriggerObject ug = UGTriggerObject.Cast(w);
			if (ug && s.IsBC == 0) {
				vector zero = "0 0 0";
				if (s.Size != zero) ug.SetSize(s.Size);
				ug.SetUGType(s.UGType);
				ug.SetEyeAccommodation(s.Eye);
				ug.SetInterpolation(s.Interp);
				ug.SetUseLinePointFade(s.UseLinePointFade);
				ug.SetAmbientSoundType(s.AmbientSoundType);
				ug.SetAmbientSoundSet(s.AmbientSoundSet);
				s_Snaps.Remove(j);
				continue;
			}

			UGBreadcrumb bc = UGBreadcrumb.Cast(w);
			if (bc && s.IsBC == 1) {
				bc.SetEyeAccommodation(s.BC_Eye);
				bc.SetUseRaycast(s.BC_Ray);
				bc.SetRadius(s.BC_Rad);
				bc.SetLightLerp(s.BC_LightLerp);
				UG_RescanTriggersAround(s.Pos, UGTriggerSettings.GetBreadcrumbScanRadius());
				s_Snaps.Remove(j);
				continue;
			}

			// If type changed or not our class kill it
			if (w.GetType() != s.Type) {
				s_Snaps.Remove(j);
			}
		}
	}
}
