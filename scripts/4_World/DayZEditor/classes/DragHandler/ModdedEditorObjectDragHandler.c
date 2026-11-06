modded class EditorObjectDragHandler
{

    override void OnDragging(notnull EditorObject target, notnull array<EditorObject> additional_drag_targets)
    {
        super.OnDragging(target, additional_drag_targets);
        
        UG_FixDuringDrag(target);
        foreach (EditorObject eo : additional_drag_targets) {
            UG_FixDuringDrag(eo);
        }
    }

    protected vector UG_RotateAroundAxis(vector v, vector k_unit, float theta)
    {
        float c = Math.Cos(theta), s = Math.Sin(theta);
        vector kxv = Math3D.CrossProduct(k_unit, v);
        float kdv  = vector.Dot(k_unit, v);
        return v * c + kxv * s + k_unit * (kdv * (1.0 - c));
    }

    protected void UG_FixDuringDrag(EditorObject eo)
    {
        if (!eo) return;

        Object w = eo.GetWorldObject();
        UGTriggerObject ug = UGTriggerObject.Cast(w);
        if (!ug) return;

        vector bc0 = eo.GetBottomCenter();
        vector m[4];
        w.GetTransform(m);

        vector ax0 = m[0].Normalized();
        vector ax1 = m[1].Normalized();
        vector ax2 = m[2].Normalized();

        if (m_UGStartRoll && m_UGStartRoll.Contains(eo)) {
            float rollNow = UG_ComputeRoll(ax0, ax1, ax2);
            float dRoll   = m_UGStartRoll[eo] - rollNow;
            if (Math.AbsFloat(dRoll) > 1e-6) {
                ax0 = UG_RotateAroundAxis(ax0, ax2, dRoll);
                ax1 = UG_RotateAroundAxis(ax1, ax2, dRoll);
            }
        }

        vector s = ug.GetSize(); 
        m[0] = ax0 * s[0];
        m[1] = ax1 * s[1];
        m[2] = ax2 * s[2];

        w.SetTransform(m);
        w.Update();

        // Don't apply position corrections during CTRL+drag (camera plane placement)
        if (!GetEditor().IsCtrlDown()) {
            vector bc1 = eo.GetBottomCenter();
            vector delta = bc0 - bc1;

            if (GetEditor().IsShiftDown()) {
                if (delta != vector.Zero) {
                    w.GetTransform(m);
                    m[3] = m[3] + delta;
                    w.SetTransform(m);
                    eo.Update();
                }
            } else {
                float dy = delta[1];
                if (Math.AbsFloat(dy) > 0.0001) {
                    vector p = eo.GetPosition();
                    p[1] = p[1] + dy;
                    eo.SetPosition(p);
                    eo.Update();
                }
            }
        }
    }
}