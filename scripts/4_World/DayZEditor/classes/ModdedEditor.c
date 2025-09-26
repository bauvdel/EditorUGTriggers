modded class Editor
{
    override void ProcessInput(float dt, Input input)
    {
        super.ProcessInput(dt, input);

        UAInputAPI input_api = GetUApi();
        if (!input_api) return;
        // - Select All
        if (input_api.GetInputByName("UGTRIG_SelectAll").LocalPress()) {
            UG_SelectAllUGObjects();
        }

        // - Copy
        if (input_api.GetInputByName("UGTRIG_Copy").LocalPress()) {
            EditorObjectMap selected = GetSelectedObjects();
            if (selected && selected.Count() > 0) {
                array<EditorObject> selection_array = selected.GetValueArray();
                int copied = UGClipboard.Copy(selection_array);
                GetEditorHud().CreateNotification(string.Format("[UG Triggers] Copied %1 entities ", copied));
            } else {
                GetEditorHud().CreateNotification("[UG Triggers] Nothing selected to Copy");
            }
        }

        // - Paste
        if (input_api.GetInputByName("UGTRIG_Paste").LocalPress()) {
            ref array<EditorObject> created = new array<EditorObject>();
            int pasted = UGClipboard.Paste(this, /*pasteMode*/ 1, /*offsetPerIndex*/ "0 0 0", created);

            if (pasted <= 0) {
                GetEditorHud().CreateNotification("[UG Triggers] Nothing to Paste");
            } else if (pasted == 1) {
                GetEditorHud().CreateNotification("[UG Triggers] Pasted 1 Entity");
            } else {
                GetEditorHud().CreateNotification(string.Format("[UG Triggers] Pasted %1 entities", pasted));
            }
        }

        // - Cut
        if (input_api.GetInputByName("UGTRIG_Cut").LocalPress()) {
            EditorObjectMap selC = GetSelectedObjects();
            if (selC && selC.Count() > 0) {
                array<EditorObject> arrC = selC.GetValueArray();
                int cut = UGClipboard.Cut(arrC, this);
                ClearSelection();
                GetEditorHud().CreateNotification(string.Format("[UG Triggers] Cut %1 Entities", cut));
            } else {
                GetEditorHud().CreateNotification("[UG Triggers] Nothing selected to Cut");
            }
        }

        // - Duplicate
        if (input_api.GetInputByName("UGTRIG_Duplicate").LocalPress()) {
            EditorObjectMap selD = GetSelectedObjects();
            if (selD && selD.Count() > 0) {
                array<EditorObject> arrD = selD.GetValueArray();

                UGClipboard.Copy(arrD);

                ref array<EditorObject> dup_out = new array<EditorObject>();
                int dup = UGClipboard.Paste(this, /*pasteMode*/ 0, /*offsetPerIndex*/ "0 0 0", dup_out);

                if (dup > 0) {
                    ClearSelection();
                    foreach (EditorObject eoc : dup_out) {
                        SelectObject(eoc);
                    }
                    GetEditorHud().CreateNotification(string.Format("[UG Triggers] Duplicated %1 Entities", dup));
                } else {
                    GetEditorHud().CreateNotification("[UG Triggers] Duplicate failed");
                }
            } else {
                GetEditorHud().CreateNotification("[UG Triggers] Nothing selected to Duplicate");
            }
        }
        // -  Scaling the Size of the UGTriggerObject - X Y Z
        EditorObjectMap selectedNow = GetSelectedObjects();
        if (selectedNow && selectedNow.Count() > 0)
        {
            foreach (int sid, EditorObject seo : selectedNow)
            {
                if (!seo) continue;

                Object sw = seo.GetWorldObject();
                UGTriggerObject trig = UGTriggerObject.Cast(sw);
                if (!trig) continue;

                float stepSize = UGTriggerSettings.GetStepSize();
                if (input_api.GetInputByName("UGTRIG_IncLength").LocalPress())  trig.TrigSize(stepSize, 0, 0);
                if (input_api.GetInputByName("UGTRIG_DecLength").LocalPress())  trig.TrigSize(-stepSize, 0, 0);
                if (input_api.GetInputByName("UGTRIG_IncWidth").LocalPress())   trig.TrigSize(0, 0, stepSize);
                if (input_api.GetInputByName("UGTRIG_DecWidth").LocalPress())   trig.TrigSize(0, 0, -stepSize);
                if (input_api.GetInputByName("UGTRIG_IncHeight").LocalPress())  trig.TrigSize(0, stepSize, 0);
                if (input_api.GetInputByName("UGTRIG_DecHeight").LocalPress())  trig.TrigSize(0, -stepSize, 0);
            }
        }

        UG_ReapplyScaleForSelection();
        UGUndoCache.Rehydrate(this);
    }
    // - Keep things scaled after moving
    void UG_ReapplyScaleForSelection()
    {
        EditorObjectMap selected = GetSelectedObjects();
        if (!selected || selected.Count() == 0) return;

        foreach (int id, EditorObject eo : selected)
        {
            Object w = eo.GetWorldObject();
            UGTriggerObject ug = UGTriggerObject.Cast(w);
            if (!ug) continue;

            vector m[4];
            w.GetTransform(m);

            vector ax0 = m[0].Normalized();
            vector ax1 = m[1].Normalized();
            vector ax2 = m[2].Normalized();

            vector s = ug.GetSize();
            m[0] = ax0 * s[0];
            m[1] = ax1 * s[1];
            m[2] = ax2 * s[2];

            w.SetTransform(m);
            w.Update();
        }
    }
    
    override int DeleteObjects(notnull array<EditorObject> editor_objects, bool create_undo = true, bool send_net_message = true)
    {
        foreach (EditorObject eo : editor_objects) {
            if (!eo) continue;
            if (eo.IsLocked()) continue;
            UGUndoCache.RememberEO(eo);
        }
        return super.DeleteObjects(editor_objects, create_undo, send_net_message);
    }

    override int DeleteObjects(EditorObjectMap editor_object_map, bool create_undo = true, bool send_net_message = true)
    {
        foreach (int id, EditorObject eo : editor_object_map) {
            if (!eo) continue;
            if (!eo.IsVisible() || eo.IsLocked()) continue;
            UGUndoCache.RememberEO(eo);
        }
        return super.DeleteObjects(editor_object_map, create_undo, send_net_message);
    }
}