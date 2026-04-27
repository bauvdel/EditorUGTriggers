modded class EditorObjectPropertiesDialog : EditorDialogBase
{
	protected ref UGTriggerObjectController m_UGController;
	protected ref UGBreadcrumbController m_BCController;

	void ~EditorObjectPropertiesDialog()
	{
		// Ensure UG objects are properly saved when dialog closes
		if (m_EditorObject) {
			m_EditorObject.UpdateNet();
		}
		if (m_EditorObjects) {
			foreach (EditorObject editor_object: m_EditorObjects) {
				editor_object.UpdateNet();
			}
		}
	}         

	override void SetEditorObject(EditorObject editor_object)
	{
		Object w = editor_object.GetWorldObject();
		bool isUG     = UGTriggerObject.Cast(w) != null;
		bool isCrumb  = UGBreadcrumb.Cast(w)    != null;
		//Return normal GroupPrefab if not UGTriggerObject or UGBreadcrumb
		if (!isUG && !isCrumb) {
			super.SetEditorObject(editor_object); 
			return;
		}
		//Build modded GroupPrefab without scale
		m_EditorObject = editor_object;
		EditorObjectController controller = m_EditorObject.GetController();

		GroupPrefab general_group = new GroupPrefab("#STR_EDITOR_GENERAL", this, string.Empty);
    	general_group.Insert(new CheckBoxPrefab("#STR_EDITOR_SHOW", controller, "Show"));
    	general_group.Insert(new EditBoxPrefab("#STR_EDITOR_NAME", controller, "Name"));
    	general_group.Insert(new VectorPrefab("#STR_EDITOR_POSITION", controller, "Position"));
    	general_group.Insert(new VectorPrefab("#STR_EDITOR_ORIENTATION", controller, "Orientation"));

		AddContent(general_group);
		//Build UGTriggerObject GroupPrefab
		if (isUG)
		{
			UGTriggerObject ug = UGTriggerObject.Cast(w);
			m_UGController = new UGTriggerObjectController(ug);

			GroupPrefab ug_group = new GroupPrefab("Underground Trigger", this, string.Empty);
			ug_group.Insert(new VectorPrefab("Size", m_UGController, "UG_SizeVec"));

			DropdownListPrefab<int> type_dropdown = new DropdownListPrefab<int>("Type", m_UGController, "UG_Type");
			type_dropdown["Outer"] = 0;
			type_dropdown["Inner"] = 1;
			type_dropdown["Transitional"] = 2;
			ug_group.Insert(type_dropdown);

			ug_group.Insert(new EditBoxNumberPrefab("Eye Accommodation", m_UGController, "UG_EyeAccommodation", 0.01, 0.0, 1.0));
			ug_group.Insert(new EditBoxNumberPrefab("Interpolation Speed", m_UGController, "UG_Interpolation", 0.01, 0.0, 1.0));

			DropdownListPrefab<int> line_fade_dropdown = new DropdownListPrefab<int>("Use Line Point Fade", m_UGController, "UG_UseLinePointFade");
			line_fade_dropdown["False"] = 0;
			line_fade_dropdown["True"] = 1;
			ug_group.Insert(line_fade_dropdown);

			AddContent(ug_group);

			// Separate group for ambient sound - gives dropdown more vertical space
			GroupPrefab ug_sound_group = new GroupPrefab("Ambient Sound", this, string.Empty);

			DropdownListPrefab<int> sound_type_dropdown = new DropdownListPrefab<int>("Sound Type", m_UGController, "UG_AmbientSoundType");
			array<string> soundOptions = new array<string>();
			array<int> soundValues = new array<int>();
			UGTriggerValidator.GetAmbientSoundTypeOptions(soundOptions, soundValues);
			for (int i = 0; i < soundOptions.Count(); i++) {
				sound_type_dropdown[soundOptions[i]] = soundValues[i];
			}
			ug_sound_group.Insert(sound_type_dropdown);

			ug_sound_group.Insert(new EditBoxPrefab("Sound Set", m_UGController, "UG_AmbientSoundSet"));

			AddContent(ug_sound_group);
		}

		//Build UGBreadcrumb GroupPrefab
		if (isCrumb)
		{
			UGBreadcrumb bc_obj = UGBreadcrumb.Cast(w);
			m_BCController = new UGBreadcrumbController(bc_obj);

			GroupPrefab bc_group = new GroupPrefab("Breadcrumb Properties", this, string.Empty);
			bc_group.Insert(new EditBoxNumberPrefab("Eye Accommodation", m_BCController, "BC_EyeAccommodation", 0.01, 0.0, 1.0));

			DropdownListPrefab<int> bc_raycast = new DropdownListPrefab<int>("Use Raycast", m_BCController, "BC_UseRaycast");
			bc_raycast["No"] = 0;  bc_raycast["Yes"] = 1;
			bc_group.Insert(bc_raycast);

			bc_group.Insert(new EditBoxNumberPrefab("Radius", m_BCController, "BC_Radius", 0.1, -1.0, 1000.0));

			DropdownListPrefab<int> bc_lightlerp = new DropdownListPrefab<int>("Light Lerp", m_BCController, "BC_LightLerp");
			bc_lightlerp["False"] = 0;
			bc_lightlerp["True"] = 1;
			bc_group.Insert(bc_lightlerp);

			AddContent(bc_group);
		}
	}

	override void SetMultipleEditorObjects(array<EditorObject> editor_objects)
	//Same as above, but for multi-select
	{
		int ugCount = 0;
		int bcCount = 0;
		foreach (EditorObject eo : editor_objects) {
			Object w = eo.GetWorldObject();
			if (UGTriggerObject.Cast(w)) ugCount++;
			else if (UGBreadcrumb.Cast(w)) bcCount++;
		}

	if ((ugCount > 0 && bcCount > 0) || (ugCount == 0 && bcCount == 0)) {
		super.SetMultipleEditorObjects(editor_objects);
		return;
	}


	m_EditorObjects = editor_objects;
	m_EditorMultiObjectCommandController = new EditorMultiObjectCommandController(editor_objects);

	GroupPrefab general_group = new GroupPrefab("#STR_EDITOR_GENERAL", m_EditorMultiObjectCommandController, string.Empty);
    general_group.Insert(new CheckBoxPrefab("#STR_EDITOR_SHOW", m_EditorMultiObjectCommandController, "Show"));
    general_group.Insert(new EditBoxPrefab("#STR_EDITOR_NAME", m_EditorMultiObjectCommandController, "Name"));
    general_group.Insert(new VectorPrefab("#STR_EDITOR_POSITION", m_EditorMultiObjectCommandController, "Position"));
    general_group.Insert(new VectorPrefab("#STR_EDITOR_ORIENTATION", m_EditorMultiObjectCommandController, "Orientation"));

    AddContent(general_group);

	// ----- All UG triggers -----
	if (ugCount > 0)
	{
		bool seeded = false;
		vector firstSize;
		float firstAcc = 1.0;
			float firstInterp = 1.0;
			int   firstType = 0;
			int   firstUseLinePointFade = 0;
			int   firstAmbientSoundType = 0;
			string firstAmbientSoundSet = "";

			foreach (EditorObject eo2 : editor_objects) {
				UGTriggerObject ug = UGTriggerObject.Cast(eo2.GetWorldObject());
				if (!ug) continue;
				if (!seeded) {
					firstSize   = ug.GetSize();
					firstAcc    = ug.GetEyeAccommodation();
					firstInterp = ug.GetInterpolation();
					firstType   = ug.GetUGType();
					if (ug.GetUseLinePointFade())
						firstUseLinePointFade = 1;
					else
						firstUseLinePointFade = 0;
					firstAmbientSoundType = UGTriggerValidator.GetAmbientSoundTypeFromString(ug.GetAmbientSoundType());
					firstAmbientSoundSet = ug.GetAmbientSoundSet();
					seeded = true;
				}
			}
			if (!seeded) return;

			// For multi-select, create a temporary UG object to use as controller
			UGTriggerObject temp_ug = UGTriggerObject.Cast(editor_objects[0].GetWorldObject());
			if (temp_ug) {
				m_UGController = new UGTriggerObjectController(temp_ug);
				m_UGController.UG_SizeVec = firstSize;
				m_UGController.UG_EyeAccommodation = firstAcc;
				m_UGController.UG_Interpolation = firstInterp;
				m_UGController.UG_Type = firstType;
				m_UGController.UG_LastType = firstType;
				m_UGController.UG_UseLinePointFade = firstUseLinePointFade;
				m_UGController.UG_AmbientSoundType = firstAmbientSoundType;
				m_UGController.UG_AmbientSoundSet = firstAmbientSoundSet;

				GroupPrefab ug_group_multi = new GroupPrefab("Underground Trigger (Selection)", this, string.Empty);
				ug_group_multi.Insert(new VectorPrefab("Size (X,Y,Z)", m_UGController, "UG_SizeVec"));

				DropdownListPrefab<int> type_dropdown = new DropdownListPrefab<int>("Type", m_UGController, "UG_Type");
				type_dropdown["Outer"] = 0;
				type_dropdown["Inner"] = 1;
				type_dropdown["Transitional"] = 2;
				ug_group_multi.Insert(type_dropdown);

				ug_group_multi.Insert(new EditBoxNumberPrefab("Eye Accommodation", m_UGController, "UG_EyeAccommodation", 0.01, 0.0, 1.0));
				ug_group_multi.Insert(new EditBoxNumberPrefab("Interpolation Speed", m_UGController, "UG_Interpolation", 0.01, 0.0, 1.0));

				DropdownListPrefab<int> line_fade_dropdown_multi = new DropdownListPrefab<int>("Use Line Point Fade", m_UGController, "UG_UseLinePointFade");
				line_fade_dropdown_multi["False"] = 0;
				line_fade_dropdown_multi["True"] = 1;
				ug_group_multi.Insert(line_fade_dropdown_multi);

				AddContent(ug_group_multi);

				// Separate group for ambient sound - gives dropdown more vertical space
				GroupPrefab ug_sound_group_multi = new GroupPrefab("Ambient Sound (Selection)", this, string.Empty);

				DropdownListPrefab<int> sound_type_dropdown_multi = new DropdownListPrefab<int>("Sound Type", m_UGController, "UG_AmbientSoundType");
				array<string> soundOptionsMulti = new array<string>();
				array<int> soundValuesMulti = new array<int>();
				UGTriggerValidator.GetAmbientSoundTypeOptions(soundOptionsMulti, soundValuesMulti);
				for (int j = 0; j < soundOptionsMulti.Count(); j++) {
					sound_type_dropdown_multi[soundOptionsMulti[j]] = soundValuesMulti[j];
				}
				ug_sound_group_multi.Insert(sound_type_dropdown_multi);

				ug_sound_group_multi.Insert(new EditBoxPrefab("Sound Set", m_UGController, "UG_AmbientSoundSet"));

				AddContent(ug_sound_group_multi);
			}
			return;
		}

		// ----- All breadcrumbs -----
		if (bcCount > 0)
		{
			// Seed from first crumb
			UGBreadcrumb bc0;
			foreach (EditorObject eo3 : editor_objects) { bc0 = UGBreadcrumb.Cast(eo3.GetWorldObject()); if (bc0) break; }
			if (!bc0) return;

			m_BCController = new UGBreadcrumbController(bc0);

			GroupPrefab bc_group = new GroupPrefab("Breadcrumb Properties (Selection)", this, string.Empty);
			bc_group.Insert(new EditBoxNumberPrefab("Eye Accommodation", m_BCController, "BC_EyeAccommodation", 0.01, 0.0, 1.0));
			DropdownListPrefab<int> bc_raycast = new DropdownListPrefab<int>("Use Raycast", m_BCController, "BC_UseRaycast");
			bc_raycast["No"] = 0; bc_raycast["Yes"] = 1; bc_group.Insert(bc_raycast);
			bc_group.Insert(new EditBoxNumberPrefab("Radius", m_BCController, "BC_Radius", 0.1, -1.0, 10000.0));
			DropdownListPrefab<int> bc_lightlerp_multi = new DropdownListPrefab<int>("Light Lerp", m_BCController, "BC_LightLerp");
			bc_lightlerp_multi["False"] = 0;
			bc_lightlerp_multi["True"] = 1;
			bc_group.Insert(bc_lightlerp_multi);
			//bc_group.Insert(new ButtonPrefab("Apply to Selection", this, "BC_ApplyToSelection"));
			AddContent(bc_group);
			return;
		}
	}
}