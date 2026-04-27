class UGTriggerObject : Building
{
	protected vector m_Size;
	protected UndergroundTrigger m_UndergroundTrigger;
	protected ref Timer m_SyncTimer;
	protected bool   m_RescanQueued;
	protected vector m_LastPosePos;
	protected vector m_LastPoseOri;
	protected int m_DesiredUGType = -1;

	// New property storage
	protected bool m_UseLinePointFade = false;
	protected string m_AmbientSoundType = "";
	protected string m_AmbientSoundSet = "";
	
	// ----- Type (0=Outer, 1=Inner, 2=Transitional) -----
	int GetUGType()
	{
		if (m_DesiredUGType >= 0) return m_DesiredUGType;
		UndergroundTrigger trig = GetLinkedTrigger();

		if (trig) return trig.m_Type; 
		return 0;
	}

	void SetUGType(int type)
	{
		// Validate input type
		if (!UGTriggerValidator.IsValidTriggerType(type))
		{
			UGTriggerErrorHandler.HandleValidationError("trigger type", type.ToString(), "0-2");
			type = eUGTriggerType.OUTER; // Default to safe value
		}

		m_DesiredUGType = type;

		UndergroundTrigger trig = GetLinkedTrigger();
		if (!trig)
		{
			UGTriggerErrorHandler.LogError("System", "No linked trigger found for SetUGType");
			return;
		}

		// Apply recommended accommodation for this type
		float recommendedAcc = UGTriggerValidator.GetRecommendedAccommodation(type);
		SetEyeAccommodation(recommendedAcc);

		// Clear breadcrumbs if type doesn't support them
		if (!UGTriggerValidator.SupportsBreadcrumbs(type) && trig.m_Data)
		{
			trig.m_Data.Breadcrumbs = null;
		}
		trig.m_Type = type;

		UGTriggerErrorHandler.LogInfo("System",
			string.Format("Trigger type set to %1", UGTriggerValidator.GetTriggerTypeName(type)));
	}

	void SetEyeAccommodation(float v)
	{
		// Validate and clamp accommodation value
		if (!UGTriggerValidator.IsValidEyeAccommodation(v))
		{
			UGTriggerErrorHandler.HandleValidationError("eye accommodation", v.ToString(), "0.0-1.0");
		}
		v = UGTriggerValidator.ClampEyeAccommodation(UG_Round2(v));

		UndergroundTrigger trig = GetLinkedTrigger();
		if (!trig)
		{
			UGTriggerErrorHandler.LogError("System", "No linked trigger found for SetEyeAccommodation");
			return;
		}

		trig.m_Accommodation = v;
		if (m_DesiredUGType == eUGTriggerType.TRANSITIONAL)
			trig.m_Type = EUndergroundTriggerType.TRANSITIONING;
	}

	float GetEyeAccommodation()
	{
		UndergroundTrigger trig = GetLinkedTrigger();
		if (trig) return trig.m_Accommodation;
		return UGTriggerSettings.GetDefaultOuterAccommodation();
	}

	void SetInterpolation(float v)
	{
		// Validate and clamp interpolation speed
		if (!UGTriggerValidator.IsValidInterpolationSpeed(v))
		{
			UGTriggerErrorHandler.HandleValidationError("interpolation speed", v.ToString(), "0.0-1.0");
		}
		v = UGTriggerValidator.ClampInterpolationSpeed(v);

		UndergroundTrigger trig = GetLinkedTrigger();
		if (!trig)
		{
			UGTriggerErrorHandler.LogError("System", "No linked trigger found for SetInterpolation");
			return;
		}

		trig.m_InterpolationSpeed = v;
	}

	float GetInterpolation()
	{
		UndergroundTrigger trig = GetLinkedTrigger();
		if (trig) return trig.m_InterpolationSpeed;
		return UGTriggerSettings.GetDefaultInterpolation();
	}

	// UseLinePointFade property
	void SetUseLinePointFade(bool value)
	{
		m_UseLinePointFade = value;

		UndergroundTrigger trig = GetLinkedTrigger();
		if (trig)
		{
			if (!trig.m_Data)
				trig.m_Data = new JsonUndergroundAreaTriggerData();
			trig.m_Data.UseLinePointFade = value;
		}

		UGTriggerErrorHandler.LogInfo("System", string.Format("UseLinePointFade set to %1", value));
	}

	bool GetUseLinePointFade()
	{
		UndergroundTrigger trig = GetLinkedTrigger();
		if (trig && trig.m_Data)
			return trig.m_Data.UseLinePointFade;
		return m_UseLinePointFade;
	}

	// AmbientSoundType property
	void SetAmbientSoundType(string soundType)
	{
		if (!UGTriggerValidator.IsValidAmbientSoundType(soundType))
		{
			UGTriggerErrorHandler.HandleValidationError("ambient sound type", soundType, "valid sound controller name (see GetAmbientSoundTypeOptions)");
			soundType = "";
		}

		m_AmbientSoundType = soundType;

		UndergroundTrigger trig = GetLinkedTrigger();
		if (trig)
		{
			if (!trig.m_Data)
				trig.m_Data = new JsonUndergroundAreaTriggerData();
			trig.m_Data.AmbientSoundType = soundType;
		}
		else
		{
			Print("[UGTriggers] SetAmbientSoundType - No linked trigger found!");
		}

		UGTriggerErrorHandler.LogInfo("System", string.Format("AmbientSoundType set to '%1'", soundType));
	}

	string GetAmbientSoundType()
	{
		UndergroundTrigger trig = GetLinkedTrigger();
		if (trig && trig.m_Data)
			return trig.m_Data.AmbientSoundType;
		return m_AmbientSoundType;
	}

	void SetAmbientSoundSet(string soundSet)
	{
		m_AmbientSoundSet = soundSet;

		UndergroundTrigger trig = GetLinkedTrigger();
		if (trig)
		{
			if (!trig.m_Data)
				trig.m_Data = new JsonUndergroundAreaTriggerData();
			trig.m_Data.AmbientSoundSet = soundSet;
		}
		else
		{
			Print("[UGTriggers] SetAmbientSoundSet - No linked trigger found!");
		}

		UGTriggerErrorHandler.LogInfo("System", string.Format("AmbientSoundSet set to '%1'", soundSet));
	}

	string GetAmbientSoundSet()
	{
		UndergroundTrigger trig = GetLinkedTrigger();
		if (trig && trig.m_Data)
			return trig.m_Data.AmbientSoundSet;
		return m_AmbientSoundSet;
	}

	void UGTriggerObject()
	{
		InitializeDefaultSettings();
		InitializeSyncTimer();
	}

	protected void InitializeDefaultSettings()
	{
		m_Size = Vector(1,1,1);
		ApplySizeTransform();

		// Initialize new properties with defaults
		m_UseLinePointFade = UGTriggerSettings.GetDefaultUseLinePointFade();
		m_AmbientSoundType = "";
		m_AmbientSoundSet = "";

		CreateTriggerIfMissing();
		UpdateTrigger();

		m_LastPosePos = GetPosition();
   		m_LastPoseOri = GetOrientation();
	}

	protected void InitializeSyncTimer()
	{
		m_SyncTimer = new Timer(CALL_CATEGORY_SYSTEM);
		m_SyncTimer.Run(UGTriggerSettings.GetSyncTimerInterval(), this, "UpdateTriggerPoseOnly", null, true);
	}

	void ~UGTriggerObject()
	{
		if (m_SyncTimer) m_SyncTimer.Stop();
		if (m_UndergroundTrigger && !m_UndergroundTrigger.IsSetForDeletion())
			m_UndergroundTrigger.Delete();
	}

	void SetSize(vector sizeMeters)
	{
		// Validate and sanitize size
		if (!UGTriggerValidator.IsValidTriggerSize(sizeMeters))
		{
			string sizeStr = string.Format("(%1, %2, %3)", sizeMeters[0], sizeMeters[1], sizeMeters[2]);
			string minStr = string.Format(">= %1 for all axes", UGTriggerSettings.GetMinDimension());
			UGTriggerErrorHandler.HandleValidationError("trigger size", sizeStr, minStr);
		}

		m_Size = UGTriggerValidator.SanitizeTriggerSize(sizeMeters);
		ApplySizeTransform();
		UpdateTrigger();
		QueueCrumbRescan();
	}

	void TrigSize(float dx, float dy, float dz)
	{
		float minDim = UGTriggerSettings.GetMinDimension();
		m_Size[0] = Math.Max(m_Size[0] + dx, minDim);
		m_Size[1] = Math.Max(m_Size[1] + dy, minDim);
		m_Size[2] = Math.Max(m_Size[2] + dz, minDim);
		ApplySizeTransform();
		UpdateTrigger();
		QueueCrumbRescan();
	}

	void SetTransformData()
	{
		vector m[4];
		GetTransform(m);
		m_Size = Vector(m[0].Length(), m[1].Length(), m[2].Length());
		UpdateTrigger(); 
	}

	vector GetSize() { return m_Size; }
	UndergroundTrigger GetLinkedTrigger() { return m_UndergroundTrigger; }

	// Ensure m_Data and Breadcrumbs array are initialized to prevent null pointer exceptions
	protected void EnsureDataInitialized(UndergroundTrigger trig)
	{
		if (!trig)
			return;

		// Create m_Data if it doesn't exist
		if (!trig.m_Data)
		{
			trig.m_Data = new JsonUndergroundAreaTriggerData();
			Print("[UGTriggers] EnsureDataInitialized - Created new m_Data");
		}

		// Always ensure Breadcrumbs array exists to prevent null pointer in UndergroundHandlerClient
		if (!trig.m_Data.Breadcrumbs)
		{
			trig.m_Data.Breadcrumbs = new array<ref JsonUndergroundAreaBreadcrumb>();
			Print("[UGTriggers] EnsureDataInitialized - Created empty Breadcrumbs array");
		}
	}

	protected void CreateTriggerIfMissing()
	{
		if (m_UndergroundTrigger)
		{
			// If trigger already exists, ensure m_Data and Breadcrumbs are initialized
			EnsureDataInitialized(m_UndergroundTrigger);
			// Then sync local properties FROM m_Data
			LoadPropertiesFromData();
			return;
		}

		DayZGame game = UGEditorGameCache.GetCachedGame();
		m_UndergroundTrigger = UndergroundTrigger.Cast(game.CreateObjectEx("UndergroundTrigger", GetPosition(), ECE_LOCAL));
		if (!m_UndergroundTrigger)
		{
			UGTriggerErrorHandler.HandleSystemError("UndergroundTrigger", "Failed to spawn trigger object");
			return;
		}

		// Initialize with default settings
		m_UndergroundTrigger.m_Accommodation      = UGTriggerSettings.GetDefaultOuterAccommodation();
		m_UndergroundTrigger.m_InterpolationSpeed = UGTriggerSettings.GetDefaultInterpolation();
		m_UndergroundTrigger.m_Type               = EUndergroundTriggerType.OUTER;
		m_DesiredUGType = -1;
		m_UndergroundTrigger.SetPosition(GetPosition());
		m_UndergroundTrigger.SetOrientation(GetOrientation());

		// Ensure m_Data and Breadcrumbs are initialized for new trigger
		EnsureDataInitialized(m_UndergroundTrigger);
	}

	// Load properties from m_UndergroundTrigger.m_Data into local storage
	protected void LoadPropertiesFromData()
	{
		if (!m_UndergroundTrigger || !m_UndergroundTrigger.m_Data)
			return;

		m_UseLinePointFade = m_UndergroundTrigger.m_Data.UseLinePointFade;
		m_AmbientSoundType = m_UndergroundTrigger.m_Data.AmbientSoundType;
		m_AmbientSoundSet = m_UndergroundTrigger.m_Data.AmbientSoundSet;
		UGTriggerErrorHandler.LogInfo("System", string.Format("Loaded from m_Data - UseLinePointFade:%1, SoundType:'%2', SoundSet:'%3'", m_UseLinePointFade, m_AmbientSoundType, m_AmbientSoundSet));
	}

	protected bool IsPointInsideOBB(vector p, out vector right, out vector up, out vector fwd, out vector pos, out vector half)
	{
		vector T[4];
		GetTransform(T);
		right = T[0].Normalized();
		up    = T[1].Normalized();
		fwd   = T[2].Normalized();
		pos   = T[3];
		half  = m_Size * 0.5;

		vector d = p - pos;
		float lx = d * right;
		float ly = d * up;
		float lz = d * fwd;

		return (Math.AbsFloat(lx) <= half[0] + 1e-3 && Math.AbsFloat(ly) <= half[1] + 1e-3 && Math.AbsFloat(lz) <= half[2] + 1e-3);
	}

	void GetBreadcrumbs()
	{
		Print("[EditorUGTriggers] GetBreadcrumbs: " + this);

		UndergroundTrigger t = GetLinkedTrigger();
		if (!t) { CreateTriggerIfMissing(); t = GetLinkedTrigger(); if (!t) { Print("[UG][ERR] no trigger; abort"); return; } }

		// Only Transitional collects crumbs; otherwise clear and exit
		if (GetUGType() != 2)
		{
			if (t.m_Data) { t.m_Data.Breadcrumbs = null; }
			Print("[EditorUGTriggers] Not Transitional; cleared crumbs");
			return;
		}

		ref array<Object> results = new array<Object>();
		float r = Math.Max(Math.Max(m_Size[0], m_Size[1]), m_Size[2]) * 1.5;
		GetGame().GetObjectsAtPosition3D(GetPosition(), r, results, null);

		ref array<ref JsonUndergroundAreaBreadcrumb> crumbs = new array<ref JsonUndergroundAreaBreadcrumb>();

		vector right, up, fwd, pos, half;
		foreach (Object obj : results)
		{
			if (!obj) continue;
			if (obj.GetType() != "UGBreadcrumb") continue;

			vector wp = obj.GetPosition();
			if (!IsPointInsideOBB(wp, right, up, fwd, pos, half)) continue;

			UGBreadcrumb crumb = UGBreadcrumb.Cast(obj);
			JsonUndergroundAreaBreadcrumb bc = new JsonUndergroundAreaBreadcrumb();
			bc.Position = new array<float>();
			bc.Position.Insert(wp[0]); bc.Position.Insert(wp[1]); bc.Position.Insert(wp[2]);
			if (crumb) {
				bc.EyeAccommodation = crumb.GetEyeAccommodation();
				bc.UseRaycast = crumb.GetUseRaycast();
				bc.Radius    = crumb.GetRadius();

			} else {
				bc.EyeAccommodation = 1.0;
				bc.UseRaycast = 0;
				bc.Radius    = -1.0;

			}

			crumbs.Insert(bc);
		}

		if (!t.m_Data)
		{
			JsonUndergroundAreaTriggerData d = BuildJsonFromUG(this);
			t.Init(d);
		}

		if (crumbs.Count() >= 2)
		{
			t.m_Data.Breadcrumbs = crumbs;

			if (m_DesiredUGType == 2)
				t.m_Type = EUndergroundTriggerType.TRANSITIONING;

			Print("[EditorUGTriggers] attached " + crumbs.Count() + " breadcrumbs");
		}
		else
		{
			t.m_Data.Breadcrumbs = null;
			Print("[EditorUGTriggers][WARN] need >=2 breadcrumbs; cleared breadcrumb mode");
		}
	}

	void CollectBreadcrumbs()
	{
		UndergroundTrigger t = GetLinkedTrigger();
		if (!t) return;

		if (GetUGType() != 2)
		{
			if (t.m_Data) { t.m_Data.Breadcrumbs = null; }
			return;
		}

		ref array<Object> objs = new array<Object>();
		float r = Math.Max(Math.Max(m_Size[0], m_Size[1]), m_Size[2]) * 1.5;
		GetGame().GetObjectsAtPosition3D(GetPosition(), r, objs, null);

		ref array<ref JsonUndergroundAreaBreadcrumb> crumbs = new array<ref JsonUndergroundAreaBreadcrumb>();
		vector right, up, fwd, pos, half;

		for (int i = 0; i < objs.Count(); i++)
		{
			Object o = objs[i];
			if (!o) continue;
			if (o.GetType() != "UGBreadcrumb") continue;

			vector p = o.GetPosition();
			if (!IsPointInsideOBB(p, right, up, fwd, pos, half)) continue;

			UGBreadcrumb crumb = UGBreadcrumb.Cast(o);
			JsonUndergroundAreaBreadcrumb bc = new JsonUndergroundAreaBreadcrumb();
			bc.Position = new array<float>();
			bc.Position.Insert(p[0]); bc.Position.Insert(p[1]); bc.Position.Insert(p[2]);
			if (crumb) {
				bc.EyeAccommodation = crumb.GetEyeAccommodation();
				bc.UseRaycast = crumb.GetUseRaycast();
				bc.Radius    = crumb.GetRadius();
			} else {
				bc.EyeAccommodation = 1.0;
				bc.UseRaycast = 0;
				bc.Radius    = -1.0;
			}

			crumbs.Insert(bc);
		}

		if (!t.m_Data) t.m_Data = new JsonUndergroundAreaTriggerData();

		if (crumbs.Count() >= 2)
		{
			t.m_Data.Breadcrumbs = crumbs;

			// are you sure?
			if (m_DesiredUGType == 2)
				t.m_Type = EUndergroundTriggerType.TRANSITIONING;
		}
		else
		{
			t.m_Data.Breadcrumbs = null;
		}
	}

	void QueueCrumbRescan()
	{
	    if (m_RescanQueued) return;
	    m_RescanQueued = true;
	    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.DoCrumbRescan, 100, false);
	}

	void DoCrumbRescan()
	{
	    m_RescanQueued = false;
	    if (GetUGType() == 2) { CollectBreadcrumbs(); }
	}

	protected float UG_MapScaleToEyeAcco(float sc)
	{
		if (sc < 0.0) sc = 0.0;
		if (sc > 1.0) sc = 1.0;
		sc = Math.Round(sc * 100.0) / 100.0;
		return sc;
	}

	protected void UpdateTriggerPoseOnly()
	{
		if (!m_UndergroundTrigger) return;

		vector P[4];
		GetTransform(P);

		vector ax0 = P[0].Normalized();
		vector ax1 = P[1].Normalized();
		vector ax2 = Math3D.CrossProduct(ax0, ax1); ax2.Normalize();
		ax1 = Math3D.CrossProduct(ax2, ax0);        ax1.Normalize();

		vector M[4];
		M[0] = ax0;
		M[1] = ax1;
		M[2] = ax2;
		M[3] = GetPosition();

		m_UndergroundTrigger.SetTransform(M);
	}

	protected void UpdateTriggerExtentsOnly()
	{
		if (!m_UndergroundTrigger) return;
		vector e = m_Size * 0.5;
		m_UndergroundTrigger.SetExtents(-e, e);
		m_UndergroundTrigger.SetPosition(m_UndergroundTrigger.GetPosition());
	}

	protected void UpdateTrigger()
	{
		CreateTriggerIfMissing();
		UpdateTriggerPoseOnly();
		UpdateTriggerExtentsOnly();
		UpdateTriggerData();
	}

	// Sync ambient sound properties to m_UndergroundTrigger.m_Data for live playback
	protected void UpdateTriggerData()
	{
		UndergroundTrigger trig = GetLinkedTrigger();
		if (!trig)
		{
			UGTriggerErrorHandler.LogError("System", "No linked trigger found for UpdateTriggerData");
			return;
		}

		// Ensure m_Data and Breadcrumbs are initialized
		EnsureDataInitialized(trig);

		// Sync ambient sound properties from local storage to m_Data
		trig.m_Data.UseLinePointFade = m_UseLinePointFade;
		trig.m_Data.AmbientSoundType = m_AmbientSoundType;
		trig.m_Data.AmbientSoundSet = m_AmbientSoundSet;
		Print(string.Format("[UGTriggers] UpdateTriggerData - UseLinePointFade:%1, SoundType:'%2', SoundSet:'%3'", m_UseLinePointFade, m_AmbientSoundType, m_AmbientSoundSet));
	}

	protected void ApplySizeTransform()
	{
		vector T[4];
		GetTransform(T);

		vector ax0 = T[0].Normalized();
		vector ax1 = T[1].Normalized();
		vector ax2 = T[2].Normalized();

		T[0] = ax0 * m_Size[0];
		T[1] = ax1 * m_Size[1];
		T[2] = ax2 * m_Size[2];

		SetTransform(T);
	}
}
void UG_RescanTriggersAround(vector center, float radius)
{
    ref array<Object> objs = new array<Object>();
    GetGame().GetObjectsAtPosition3D(center, radius, objs, null);
    for (int i = 0; i < objs.Count(); i++) {
        UGTriggerObject ug = UGTriggerObject.Cast(objs[i]);
        if (!ug) continue;
        if (ug.GetUGType() != 2) continue;
        ug.QueueCrumbRescan();
    }
}

float UG_Round2(float v)
{
	v = Math.Clamp(v, 0.0, 1.0);
	return Math.Round(v * 100.0) / 100.0;
}

//IMPORT to Editor
class UGTriggerApplyRec
{
    vector Pos;
    vector Size;
    float  EyeAcc;
    float  Interp;
    int    Type;
    bool   UseLinePointFade;
    string AmbientSoundType;
    string AmbientSoundSet;
}

class UGBreadcrumbApplyRec
{
    vector Pos;
    float  EyeAcc;
    int    UseRaycast;
    float  Radius;
    bool   LightLerp;
}

ref array<ref UGTriggerApplyRec>     g_UG_ToApply = new array<ref UGTriggerApplyRec>();
ref array<ref UGBreadcrumbApplyRec>  g_BC_ToApply = new array<ref UGBreadcrumbApplyRec>();

class UG_PostImportApplier
{
    static int s_Attempts = 20;

    static void Start()
    {
        s_Attempts = 20;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UG_PostImportApplier.Tick, 200, false);
    }

    static void Tick()
    {
        for (int i = g_UG_ToApply.Count() - 1; i >= 0; i--)
        {
            UGTriggerApplyRec rec = g_UG_ToApply[i];
            UGTriggerObject ug = FindNearestUG(rec.Pos, 3.0);
            if (!ug) continue;

            // Apply in correct order
            ug.SetUGType(rec.Type);
            ug.SetSize(rec.Size);
            ug.SetEyeAccommodation(rec.EyeAcc);
            ug.SetInterpolation(rec.Interp);
            ug.SetUseLinePointFade(rec.UseLinePointFade);
            ug.SetAmbientSoundType(rec.AmbientSoundType);
            ug.SetAmbientSoundSet(rec.AmbientSoundSet);

            if (rec.Type == 2) ug.QueueCrumbRescan();

            g_UG_ToApply.Remove(i);
        }

        for (int j = g_BC_ToApply.Count() - 1; j >= 0; j--)
        {
            UGBreadcrumbApplyRec bc = g_BC_ToApply[j];
            UGBreadcrumb obj = FindNearestBC(bc.Pos, 3.0);
            if (!obj) continue;

            obj.SetEyeAccommodation(bc.EyeAcc);
            obj.SetUseRaycast(bc.UseRaycast);
            obj.SetRadius(bc.Radius);
            obj.SetLightLerp(bc.LightLerp);

            g_BC_ToApply.Remove(j);
        }

        if (g_UG_ToApply.Count() == 0 && g_BC_ToApply.Count() == 0) {
            return;
        }

        s_Attempts--;
        if (s_Attempts > 0) {
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(UG_PostImportApplier.Tick, 200, false);
        }
    }

    static UGTriggerObject FindNearestUG(vector pos, float radius)
    {
        ref array<Object> objs = new array<Object>();
        GetGame().GetObjectsAtPosition3D(pos, radius, objs, null);

        float bestD2 = 1e12;
        UGTriggerObject best;
        foreach (Object o : objs)
        {
            UGTriggerObject ug = UGTriggerObject.Cast(o);
            if (!ug) continue;
            float d2 = vector.DistanceSq(ug.GetPosition(), pos);
            if (d2 < bestD2) { bestD2 = d2; best = ug; }
        }
        return best;
    }

    static UGBreadcrumb FindNearestBC(vector pos, float radius)
    {
        ref array<Object> objs = new array<Object>();
        GetGame().GetObjectsAtPosition3D(pos, radius, objs, null);

        float bestD2 = 1e12;
        UGBreadcrumb best;
        foreach (Object o : objs)
        {
            UGBreadcrumb bc = UGBreadcrumb.Cast(o);
            if (!bc) continue;
            float d2 = vector.DistanceSq(bc.GetPosition(), pos);
            if (d2 < bestD2) { bestD2 = d2; best = bc; }
        }
        return best;
    }
}