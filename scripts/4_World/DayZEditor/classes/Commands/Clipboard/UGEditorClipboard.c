// - Copy/Cut/Paste/Duplicate logic for Triggers and Breadcrumbs
class UGObjectClip
{
	string Type;
	vector Position;
	vector Orientation;
	vector Size;
	string Name;

	int    UGType = 0;
	float  EyeAccommodation = 1.0;
	float  Interpolation    = 7.0;
	bool   UseLinePointFade = false;
	string AmbientSoundType = "";
	string AmbientSoundSet = "";
	bool   IsBreadcrumb = false;
	float  BC_EyeAccommodation = 1.0;
	int    BC_UseRaycast = 0;
	float  BC_Radius = -1.0;
	bool   BC_LightLerp = false;
};

class UGClipboard
{
	protected static ref array<ref UGObjectClip> s_Buffer = new array<ref UGObjectClip>();
	protected static vector s_CopyCenter = "0 0 0";

	static void Clear()
	{
		s_Buffer.Clear();
		s_CopyCenter = "0 0 0";
	}

	static int Count()
	{
		return s_Buffer.Count();
	}

	static int Copy(array<EditorObject> selection)
	{
		Clear();
		if (!selection || selection.Count() == 0) {
			return 0;
		}

		s_CopyCenter = ComputeCenter(selection);

		foreach (EditorObject eo : selection) {
			if (!eo) continue;

			Object w = eo.GetWorldObject();
			if (!w) continue;

			UGObjectClip clip = new UGObjectClip();
			clip.Type        = eo.GetType();
			clip.Position    = eo.GetPosition();
			clip.Orientation = eo.GetOrientation();
			clip.Name        = eo.GetDisplayName(); 

			UGTriggerObject ug = UGTriggerObject.Cast(w);
			if (ug) {
				clip.Size             = ug.GetSize();
				clip.UGType           = ug.GetUGType();
				clip.EyeAccommodation = ug.GetEyeAccommodation();
				clip.Interpolation    = ug.GetInterpolation();
				clip.UseLinePointFade = ug.GetUseLinePointFade();
				clip.AmbientSoundType = ug.GetAmbientSoundType();
				clip.AmbientSoundSet  = ug.GetAmbientSoundSet();
				s_Buffer.Insert(clip);
				continue;
			}

			UGBreadcrumb bc = UGBreadcrumb.Cast(w);
			if (bc) {
				clip.IsBreadcrumb        = true;
				clip.BC_EyeAccommodation = bc.GetEyeAccommodation();
				clip.BC_UseRaycast       = bc.GetUseRaycast();
				clip.BC_Radius           = bc.GetRadius();
				clip.BC_LightLerp        = bc.GetLightLerp();
				s_Buffer.Insert(clip);
				continue;
			}

			s_Buffer.Insert(clip);
		}

		return s_Buffer.Count();
	}

	static int Cut(array<EditorObject> selection, Editor editor)
	{
		int n = Copy(selection);
		if (n == 0) {
			return 0;
		}

		array<EditorObject> objects_to_delete = new array<EditorObject>();
		foreach (EditorObject eo : selection) {
			if (eo) objects_to_delete.Insert(eo);
		}
		if (objects_to_delete.Count() > 0) {
			editor.DeleteObjects(objects_to_delete);
		}
		return n;
	}

	static int Paste(Editor editor, int pasteMode = 1, vector offsetPerIndex = "0 0 0", out array<EditorObject> created_out = null)
	{
		if (s_Buffer.Count() == 0) {
			return 0;
		}
		if (!created_out) {
			created_out = new array<EditorObject>();
		}

		vector anchor;
		if (pasteMode == 1) {
			anchor = GetCursorAnchor(editor);
			vector zero = "0 0 0";
			if (anchor == zero) {
				// Fallback if raycast failed
				anchor = s_CopyCenter;
			}
		} else {
			anchor = s_CopyCenter;
		}

		return SpawnFromBuffer(editor, anchor, pasteMode == 1, offsetPerIndex, created_out);
	}

	static int PasteAt(Editor editor, vector anchor, vector offsetPerIndex = "0 0 0", out array<EditorObject> created_out = null)
	{
		if (s_Buffer.Count() == 0) {
			return 0;
		}
		if (!created_out) {
			created_out = new array<EditorObject>();
		}
		return SpawnFromBuffer(editor, anchor, true, offsetPerIndex, created_out);
	}

	protected static vector ComputeCenter(array<EditorObject> objs)
	{
		if (!objs || objs.Count() == 0) {
			vector zeroEarly = "0 0 0";
			return zeroEarly;
		}

		vector sum = "0 0 0";
		int n = 0;

		foreach (EditorObject eo : objs) {
			if (!eo) continue;
			sum = sum + eo.GetPosition();
			n++;
		}

		if (n > 0) {
			float invN = 1.0 / n;
			vector center;
			center[0] = sum[0] * invN;
			center[1] = sum[1] * invN;
			center[2] = sum[2] * invN;
			return center;
		} else {
			vector zeroLate = "0 0 0";
			return zeroLate;
		}
	}

	protected static vector GetCursorAnchor(Editor editor)
	{
		if (!editor) {
			vector zero = "0 0 0";
			return zero;
		}

		Raycast rc = editor.GetCursorRaycastModeSafe();
		if (rc && rc.Bounce) {
			return rc.Bounce.Position;
		}

		rc = editor.GetCameraRaycast();
		if (rc && rc.Bounce) {
			return rc.Bounce.Position;
		}

		vector zero2 = "0 0 0";
		return zero2;
	}

	protected static vector ScaleVec(vector v, float f)
	{
		vector r;
		r[0] = v[0] * f;
		r[1] = v[1] * f;
		r[2] = v[2] * f;
		return r;
	}

	protected static int SpawnFromBuffer(Editor editor, vector anchor, bool relativeToCenter, vector offsetPerIndex, out array<EditorObject> created_out)
	{
		ref array<ref EditorObjectData> write_data = new array<ref EditorObjectData>();

		int idx = 0;
		foreach (UGObjectClip clip : s_Buffer) {
			if (!clip) continue;

			vector pos = clip.Position;
			if (relativeToCenter) {
				vector delta = clip.Position - s_CopyCenter;
				float fidx = idx;
				vector step = ScaleVec(offsetPerIndex, fidx);
				pos = anchor + delta + step;
			}

			EditorObjectData data = EditorObjectData.Create(clip.Type, pos, clip.Orientation, 1.0, EFE_DEFAULT);
			if (data) {
				write_data.Insert(data);
			}
			idx++;
		}

		if (write_data.Count() == 0) {
			return 0;
		}

		EditorObjectMap created_map = editor.CreateObjects(write_data);
		if (!created_map) {
			return 0;
		}

		ref array<EditorObject> created_list = created_map.GetValueArray();
		if (!created_out) {
			created_out = new array<EditorObject>();
		}
		if (created_list) {
			for (int i = 0; i < created_list.Count(); i++) {
				created_out.Insert(created_list[i]);
			}
		}

		int minCount = Math.Min(created_out.Count(), s_Buffer.Count());
		for (int j = 0; j < minCount; j++) {
			EditorObject eo = created_out[j];
			if (!eo) continue;

			UGObjectClip clipj = s_Buffer[j];
			if (!clipj) continue;

			vector posj = eo.GetPosition();
			ApplyCustomProps(eo, clipj, posj);
		}

		editor.ClearSelection();
		foreach (EditorObject eoc : created_out) {
			editor.SelectObject(eoc);
		}

		return created_out.Count();
	}

	protected static void ApplyCustomProps(EditorObject eo, UGObjectClip clip, vector worldPos)
	{
		if (!eo) return;

		Object w = eo.GetWorldObject();
		if (!w) return;

		UGTriggerObject ug = UGTriggerObject.Cast(w);
		if (ug) {
			vector zeroSize = "0 0 0";
			if (clip.Size != zeroSize) ug.SetSize(clip.Size);
			ug.SetUGType(clip.UGType);
			ug.SetEyeAccommodation(clip.EyeAccommodation);
			ug.SetInterpolation(clip.Interpolation);
			ug.SetUseLinePointFade(clip.UseLinePointFade);
			ug.SetAmbientSoundType(clip.AmbientSoundType);
			ug.SetAmbientSoundSet(clip.AmbientSoundSet);
			return;
		}

		UGBreadcrumb bc = UGBreadcrumb.Cast(w);
		if (bc && clip.IsBreadcrumb) {
			bc.SetEyeAccommodation(clip.BC_EyeAccommodation);
			bc.SetUseRaycast(clip.BC_UseRaycast);
			bc.SetRadius(clip.BC_Radius);
			bc.SetLightLerp(clip.BC_LightLerp);
			UG_RescanTriggersAround(worldPos, 200.0);
			return;
		}
	}
}
