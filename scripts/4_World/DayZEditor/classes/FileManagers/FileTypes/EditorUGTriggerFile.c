class EditorUGTriggerFile : EditorFileType
{
        // Export triggers to JSON
    override void Export(EditorSaveData data, string file, ExportSettings settings, eDialogExtraSetting dialog_setting)
    {
        UGTriggersExportRoot root = new UGTriggersExportRoot();

        if (!ValidateExportPreconditions(file, root))
            return;

        Editor editor = UGEditorGameCache.GetCachedEditor();
        EditorObjectMap allObjects = editor.GetPlacedObjects();

        if (!allObjects) {
            FinalizeExport(file, root);
            return;
        }

        ProcessTriggerObjects(data, allObjects, root);
        FinalizeExport(file, root);
    }

        // Validate export preconditions
    protected bool ValidateExportPreconditions(string file, UGTriggersExportRoot root)
    {
        Editor editor = UGEditorGameCache.GetCachedEditor();
        if (!editor) {
            ScriptCallQueue guiQueue = UGEditorGameCache.GetCachedGUICallQueue();
            UIManager uiManager = UGEditorGameCache.GetCachedUIManager();
            if (guiQueue && uiManager) {
                guiQueue.CallLater(uiManager.CloseDialog, UGTriggerSettings.GetDialogCloseDelay());
            }
            return false;
        }
        return true;
    }

        // Process trigger objects for export
    protected void ProcessTriggerObjects(EditorSaveData data, EditorObjectMap allObjects, UGTriggersExportRoot root)
    {
        foreach (EditorObjectData obj_data : data.EditorObjects)
        {
            if (!obj_data || obj_data.Type != "UGTriggerObject") continue;

            UGTriggerObject ug = FindMatchingTriggerObject(obj_data, allObjects);
            if (!ug) continue;

            UGTriggersExport triggerExport = CreateTriggerExport(obj_data, ug);
            ProcessBreadcrumbs(ug, triggerExport);

            root.Triggers.Insert(triggerExport);
        }
    }

    // Find trigger object by position matching
    protected UGTriggerObject FindMatchingTriggerObject(EditorObjectData obj_data, EditorObjectMap allObjects)
    {
        float tolerance = UGTriggerSettings.GetPositionTolerance();

        foreach (int id, EditorObject placed_obj : allObjects) {
            if (!placed_obj || !placed_obj.GetWorldObject()) continue;

            vector objPos = placed_obj.GetPosition();
            if (vector.Distance(objPos, obj_data.Position) < tolerance) {
                Object worldObj = placed_obj.GetWorldObject();
                if (worldObj && worldObj.IsKindOf("UGTriggerObject")) {
                    UGTriggerObject ug = UGTriggerObject.Cast(worldObj);
                    if (ug) return ug;
                }
            }
        }
        return null;
    }

        // Create trigger export object
    protected UGTriggersExport CreateTriggerExport(EditorObjectData obj_data, UGTriggerObject ug)
    {
        vector pos = obj_data.Position;
        vector orient = obj_data.Orientation;
        vector size = UGTriggerSettings.GetDefaultTriggerSize();
        float acc = UGTriggerSettings.GetDefaultOuterAccommodation();
        float interp = UGTriggerSettings.GetDefaultInterpolation();

        if (ug) {
            size = ug.GetSize();
            acc = UG_Round2(ug.GetEyeAccommodation());
            interp = UG_Round2(ug.GetInterpolation());
        }

        UGTriggersExport triggerExport = new UGTriggersExport(pos, orient, size, acc, interp);

        if (ug) {
            if (ug.GetUseLinePointFade())
                triggerExport.UseLinePointFade = 1;
            else
                triggerExport.UseLinePointFade = 0;
            triggerExport.AmbientSoundType = ug.GetAmbientSoundType();
            #ifdef DAYZ_1_29
            triggerExport.AmbientSoundSet = ug.GetAmbientSoundSet();
            #endif
        }

        return triggerExport;
    }

        // Process breadcrumbs for transitional triggers
    protected void ProcessBreadcrumbs(UGTriggerObject ug, UGTriggersExport triggerExport)
    {
        if (ug.GetUGType() != UGTriggerSettings.GetTransitionalType()) return;

        UndergroundTrigger trig = ug.GetLinkedTrigger();
        if (!trig || !trig.m_Data || !trig.m_Data.Breadcrumbs || trig.m_Data.Breadcrumbs.Count() < 2)
            return;

        triggerExport.Breadcrumbs = new array<ref UGBreadcrumbExport>();
        foreach (JsonUndergroundAreaBreadcrumb b : trig.m_Data.Breadcrumbs)
        {
            UGBreadcrumbExport eb = CreateBreadcrumbExport(b);
            triggerExport.Breadcrumbs.Insert(eb);
        }
    }

        // Create breadcrumb export object
    protected UGBreadcrumbExport CreateBreadcrumbExport(JsonUndergroundAreaBreadcrumb b)
    {
        UGBreadcrumbExport eb = new UGBreadcrumbExport();
        eb.Position = new array<float>();
        eb.Position.Insert(b.Position.Get(0));
        eb.Position.Insert(b.Position.Get(1));
        eb.Position.Insert(b.Position.Get(2));
        eb.EyeAccommodation = UG_Round2(b.EyeAccommodation);
        eb.UseRaycast = b.UseRaycast;
        eb.Radius = b.Radius;
        if (b.LightLerp)
            eb.LightLerp = 1;
        else
            eb.LightLerp = 0;
        return eb;
    }

        // Finalize export operation
    protected void FinalizeExport(string file, UGTriggersExportRoot root)
    {

        JsonFileLoader<UGTriggersExportRoot>.JsonSaveFile(file, root);

        HandlePostExportCleanup();
    }

        // Handle post-export cleanup
    protected void HandlePostExportCleanup()
    {
        bool removeUG = UGExportState.RemoveUGAfterExport;
        UGExportState.RemoveUGAfterExport = false; // Reset state

        Editor editor = UGEditorGameCache.GetCachedEditor();
        if (!editor) return;

        if (removeUG) {
            int removed = UG_DeleteAllUGObjects(editor);
            editor.GetEditorHud().CreateNotification(
                string.Format("[UG Triggers] Exported and removed %1 UG objects", removed));
        } else {
            editor.GetEditorHud().CreateNotification("[UG Triggers] Export complete");
        }
    }
    //Import UGTriggers from JSON
    override EditorSaveData Import(string file, ImportSettings settings)
    {
        EditorSaveData save_data = new EditorSaveData();
        UGTriggersExportRoot import_data = new UGTriggersExportRoot();
        JsonFileLoader<UGTriggersExportRoot>.JsonLoadFile(file, import_data);

        if (!import_data || !import_data.Triggers || import_data.Triggers.Count() == 0) {
            return save_data;
        }

        foreach (UGTriggersExport t : import_data.Triggers)
        {
            vector pos    = Vector(t.Position[0], t.Position[1], t.Position[2]);
            vector orient = Vector(t.Orientation[0], t.Orientation[1], t.Orientation[2]);
            vector size   = Vector(t.Size[0], t.Size[1], t.Size[2]);
            float acc = t.EyeAccommodation;
            acc = UG_Round2(Math.Clamp(acc, 0.0, 1.0));
            float interp = t.InterpolationSpeed;
            interp = UG_Round2(Math.Clamp(interp, 0.0, 1.0));

            // Determine trigger type based on import data characteristics
            int ugType = DetermineTriggerTypeFromImportData(t, acc);

            UGTriggerApplyRec arec = new UGTriggerApplyRec();
            arec.Pos    = pos;
            arec.Size   = size;
            arec.EyeAcc = acc;
            arec.Interp = interp;
            arec.Type   = ugType;
            arec.UseLinePointFade = t.UseLinePointFade != 0;
            arec.AmbientSoundType = t.AmbientSoundType;
            #ifdef DAYZ_1_29
            arec.AmbientSoundSet = t.AmbientSoundSet;
            #endif
            g_UG_ToApply.Insert(arec);

            EditorObjectData dta = EditorObjectData.Create("UGTriggerObject", pos, orient, 1.0, EFE_DEFAULT);
            save_data.EditorObjects.Insert(dta);

            if (t.Breadcrumbs)
            {
                foreach (UGBreadcrumbExport be : t.Breadcrumbs)
                {
                    vector bpos    = Vector(be.Position[0], be.Position[1], be.Position[2]);
                    vector borient = Vector(0, 0, 0);

                    UGBreadcrumbApplyRec bcrec = new UGBreadcrumbApplyRec();
                    float bAcc = UG_Round2(Math.Clamp(be.EyeAccommodation, 0.0, 1.0));
                    bcrec.Pos        = bpos;
                    bcrec.EyeAcc     = bAcc;
                    bcrec.UseRaycast = be.UseRaycast;
                    bcrec.Radius     = be.Radius;
                    bcrec.LightLerp  = be.LightLerp != 0;
                    g_BC_ToApply.Insert(bcrec);

                    EditorObjectData bcDta = EditorObjectData.Create("UGBreadcrumb", bpos, borient, 1.0, EFE_DEFAULT);
                    save_data.EditorObjects.Insert(bcDta);
                }
            }
        }

        UG_PostImportApplier.Start();

        return save_data;
    }

        // Determine trigger type from import data
    protected int DetermineTriggerTypeFromImportData(UGTriggersExport t, float acc)
    {
        // Transitional triggers have breadcrumbs
        if (t.Breadcrumbs && t.Breadcrumbs.Count() >= 2) {
            return UGTriggerSettings.GetTransitionalType();
        }
        // Inner triggers typically have low accommodation
        else if (acc <= 0.5) {
            return UGTriggerSettings.GetInnerType();
        }
        // Default to outer type
        else {
            return UGTriggerSettings.GetOuterType();
        }
    }

    override string GetExtension()
	{
		return ".json";
	}

    override void GetValidExtensions(notnull inout array<ref Param2<string, string>> valid_extensions)
	{
		super.GetValidExtensions(valid_extensions);
		valid_extensions.Insert(new Param2<string, string>("Object Spawner", "*.json"));
	}
}