modded class EditorObject
{
    // Remove bounding box for UGTriggerObject.
    // DayZ Editor (1.30 update) replaced ShowBoundingBox() with
    // SetBoundingBoxVisibility(bool): the box is now drawn every frame in
    // OnFrame() while m_BoundingBoxVisible is true. Forcing it off here keeps
    // trigger volumes free of the editor's box on select/show.
    override void SetBoundingBoxVisibility(bool state)
    {
        if (state && UGTriggerObject.Cast(GetWorldObject()))
        {
            state = false;
        }
        super.SetBoundingBoxVisibility(state);
    }
}
